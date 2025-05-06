/*
 * tl_usb_comm.c
 *
 * 塔燈通訊控制函式庫 - USB通訊實現 (含更嚴謹的先初始化 + 再檢查)
 *
 * 功能:
 *  - tl_usb_open_device()      : 先完成 WinUsb_Initialize & GetAssociatedInterface, 再做 tl_usb_is_device_ready()
 *  - tl_usb_close_device()     : 關閉裝置
 *  - tl_usb_is_device_ready()  : 檢查 handle 是否都非NULL, 可再執行 ephemeral WinUsb_Initialize測試
 *  - tl_usb_write_data()       : 寫入
 *  - tl_usb_read_data()        : 讀取
 *
 * 版本: 1.0.5
 * 日期: 2025-04-02
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tl_internal.h"

#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>

 /* WinUSB 相關型態與函式指標 */
typedef void* WINUSB_INTERFACE_HANDLE;
typedef unsigned char  BOOLEAN;
typedef unsigned long  ULONG;

typedef BOOLEAN(__stdcall* WinUsb_Initialize_t)(HANDLE, WINUSB_INTERFACE_HANDLE*);
typedef BOOLEAN(__stdcall* WinUsb_Free_t)(WINUSB_INTERFACE_HANDLE);
typedef BOOLEAN(__stdcall* WinUsb_GetAssociatedInterface_t)(WINUSB_INTERFACE_HANDLE, UCHAR, WINUSB_INTERFACE_HANDLE*);
typedef BOOLEAN(__stdcall* WinUsb_WritePipe_t)(WINUSB_INTERFACE_HANDLE, UCHAR, PUCHAR, ULONG, PULONG, LPOVERLAPPED);
typedef BOOLEAN(__stdcall* WinUsb_ReadPipe_t)(WINUSB_INTERFACE_HANDLE, UCHAR, PUCHAR, ULONG, PULONG, LPOVERLAPPED);

/* 動態載入 */
static HMODULE hWinUSBLib = NULL;
static WinUsb_Initialize_t            pWinUsb_Initialize = NULL;
static WinUsb_Free_t                  pWinUsb_Free = NULL;
static WinUsb_GetAssociatedInterface_t pWinUsb_GetAssociatedInterface = NULL;
static WinUsb_WritePipe_t             pWinUsb_WritePipe = NULL;
static WinUsb_ReadPipe_t              pWinUsb_ReadPipe = NULL;

static TL_ERROR_CODE load_winusb_library(void);
static void unload_winusb_library(void);

#else  /* 非Windows平台 - 可另行實作 */
#include <unistd.h>
#include <time.h>
#endif

 /* extern 由其他檔案提供 */
extern TL_InternalState* tl_get_internal_state(void);
extern void tl_delay_ms(unsigned long ms);

/* -------------------------------------------------------------------------
 * 動態載入/卸載winusb.dll
 */
#ifdef _WIN32
static TL_ERROR_CODE load_winusb_library(void)
{
    if (hWinUSBLib) {
        return TL_SUCCESS; /* 已載入 */
    }
    hWinUSBLib = LoadLibraryA("winusb.dll");
    if (!hWinUSBLib) {
        DWORD err = GetLastError();
#ifdef BUILD_TEST_EXE 
        printf("[load_winusb_library] LoadLibrary('winusb.dll') 失敗, error=%lu\n", err);
#endif
        return TL_ERROR_GENERAL;
    }

    pWinUsb_Initialize = (WinUsb_Initialize_t)GetProcAddress(hWinUSBLib, "WinUsb_Initialize");
    pWinUsb_Free = (WinUsb_Free_t)GetProcAddress(hWinUSBLib, "WinUsb_Free");
    pWinUsb_GetAssociatedInterface = (WinUsb_GetAssociatedInterface_t)GetProcAddress(hWinUSBLib, "WinUsb_GetAssociatedInterface");
    pWinUsb_WritePipe = (WinUsb_WritePipe_t)GetProcAddress(hWinUSBLib, "WinUsb_WritePipe");
    pWinUsb_ReadPipe = (WinUsb_ReadPipe_t)GetProcAddress(hWinUSBLib, "WinUsb_ReadPipe");

    if (!pWinUsb_Initialize || !pWinUsb_Free ||
        !pWinUsb_GetAssociatedInterface || !pWinUsb_WritePipe || !pWinUsb_ReadPipe) {
#ifdef BUILD_TEST_EXE 
        printf("[load_winusb_library] GetProcAddress - 部分函式為NULL\n");
#endif
        FreeLibrary(hWinUSBLib);
        hWinUSBLib = NULL;
        return TL_ERROR_GENERAL;
    }
    return TL_SUCCESS;
}

static void unload_winusb_library(void)
{
    if (hWinUSBLib) {
        FreeLibrary(hWinUSBLib);
        hWinUSBLib = NULL;
    }
    pWinUsb_Initialize = NULL;
    pWinUsb_Free = NULL;
    pWinUsb_GetAssociatedInterface = NULL;
    pWinUsb_WritePipe = NULL;
    pWinUsb_ReadPipe = NULL;
}
#endif /* _WIN32 */

/* -------------------------------------------------------------------------
 * 檢查裝置是否就緒
 * 可在此恢復檢查 if (!device_handle || !interface_handle) ...
 */
TL_BOOL tl_usb_is_device_ready(void)
{
#ifdef _WIN32
    TL_InternalState* state = tl_get_internal_state();

    /* 檢查 */
    if (!state->device_handle || !state->interface_handle) {
#ifdef BUILD_TEST_EXE 
        printf("[tl_usb_is_device_ready] device_handle 或 interface_handle 為NULL => 不就緒\n");
#endif
        return TL_FALSE;
    }

    /* 
     * ephemeral WinUsb_Initialize 重試,
     * 確保裝置是真的可被初始化(在某些裝置剛上電需輪詢):
     */
    WINUSB_INTERFACE_HANDLE temp_handle;
    for (int attempt = 0; attempt < TL_MAX_DEVICE_READY_ATTEMPTS; attempt++) {
        if (pWinUsb_Initialize(state->device_handle, &temp_handle)) {
            pWinUsb_Free(temp_handle);
#ifdef BUILD_TEST_EXE 
            printf("[tl_usb_is_device_ready] 第 %d 次 WinUsb_Initialize 成功 => 就緒\n", attempt + 1);
#endif 
            return TL_TRUE;
        }
        DWORD dwErr = GetLastError();
#ifdef BUILD_TEST_EXE 
        printf("[tl_usb_is_device_ready] 第 %d 次失敗, error=%lu\n", attempt + 1, dwErr);
#endif
        /* 決定是否可重試 */
        if (dwErr != ERROR_GEN_FAILURE &&
            dwErr != ERROR_IO_PENDING &&
            dwErr != ERROR_DEVICE_NOT_CONNECTED)
        {
#ifdef BUILD_TEST_EXE 
            printf("[tl_usb_is_device_ready] 不可重試錯誤 => 中止\n");
#endif
            break;
        }
        tl_delay_ms(TL_DEVICE_READY_WAIT_MS);
    }
#ifdef BUILD_TEST_EXE 
    printf("[tl_usb_is_device_ready] 重試多次仍失敗 => 不就緒\n");
#endif
    return TL_FALSE;

#else
    /* 非Windows平台 => 假設已就緒 */
    return TL_TRUE;
#endif
}

/* -------------------------------------------------------------------------
 * 開啟 USB 裝置
 *  - 步驟:
 *      1. SetupDi... / CreateFile
 *      2. WinUsb_Initialize -> primaryInterface
 *      3. GetAssociatedInterface -> secondaryInterface
 *      4. tl_usb_is_device_ready() => 檢查
 */
TL_ERROR_CODE tl_usb_open_device(void)
{
#ifdef _WIN32
    TL_InternalState* state = tl_get_internal_state();
    TL_ERROR_CODE result;
    HDEVINFO deviceInfoSet = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DATA interfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
    DWORD detailSize = 0;

    GUID deviceGuidStruct;

    /* 1. 載入winusb.dll */
    result = load_winusb_library();
    if (result != TL_SUCCESS) {
        tl_set_last_error(result);
        return result;
    }

    /* 2. 組合GUID */
    deviceGuidStruct.Data1 = TL_GUID_DATA1;
    deviceGuidStruct.Data2 = TL_GUID_DATA2;
    deviceGuidStruct.Data3 = TL_GUID_DATA3;
    deviceGuidStruct.Data4[0] = TL_GUID_DATA4_0;
    deviceGuidStruct.Data4[1] = TL_GUID_DATA4_1;
    deviceGuidStruct.Data4[2] = TL_GUID_DATA4_2;
    deviceGuidStruct.Data4[3] = TL_GUID_DATA4_3;
    deviceGuidStruct.Data4[4] = TL_GUID_DATA4_4;
    deviceGuidStruct.Data4[5] = TL_GUID_DATA4_5;
    deviceGuidStruct.Data4[6] = TL_GUID_DATA4_6;
    deviceGuidStruct.Data4[7] = TL_GUID_DATA4_7;

    /* 3. SetupDiGetClassDevs */
    deviceInfoSet = SetupDiGetClassDevs(&deviceGuidStruct, NULL, NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        DWORD dwErr = GetLastError();
#ifdef BUILD_TEST_EXE 
        printf("[tl_usb_open_device] SetupDiGetClassDevs失敗, error=%lu\n", dwErr);
#endif
        tl_set_last_error(TL_ERROR_DEVICE_NOT_FOUND);
        unload_winusb_library();
        return TL_ERROR_DEVICE_NOT_FOUND;
    }

    /* 4. 枚舉介面 */
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    if (!SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL,
        &deviceGuidStruct,
        0, &interfaceData)) {
        DWORD dwErr = GetLastError();
#ifdef BUILD_TEST_EXE 
        printf("[tl_usb_open_device] SetupDiEnumDeviceInterfaces失敗, error=%lu\n", dwErr);
#endif
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
        tl_set_last_error(TL_ERROR_DEVICE_NOT_FOUND);
        unload_winusb_library();
        return TL_ERROR_DEVICE_NOT_FOUND;
    }

    SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &interfaceData,
        NULL, 0, &detailSize, NULL);

    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(detailSize);
    if (!detailData) {
#ifdef BUILD_TEST_EXE 
        printf("[tl_usb_open_device] malloc介面資訊失敗\n");
#endif
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
        tl_set_last_error(TL_ERROR_MEMORY_ALLOCATION);
        unload_winusb_library();
        return TL_ERROR_MEMORY_ALLOCATION;
    }
    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &interfaceData,
        detailData, detailSize,
        NULL, NULL)) {
#ifdef BUILD_TEST_EXE 
        DWORD dwErr = GetLastError();
        printf("[tl_usb_open_device] SetupDiGetDeviceInterfaceDetail失敗, error=%lu\n", dwErr);
#endif
        free(detailData);
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
        tl_set_last_error(TL_ERROR_DEVICE_NOT_FOUND);
        unload_winusb_library();
        return TL_ERROR_DEVICE_NOT_FOUND;
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    /* 5. CreateFile */
    state->device_handle = CreateFile(detailData->DevicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL);
#ifdef BUILD_TEST_EXE 
    printf("[tl_usb_open_device] DevicePath=%s\n", detailData->DevicePath);
#endif
    free(detailData);

    if (state->device_handle == INVALID_HANDLE_VALUE) {
        state->device_handle = NULL;
#ifdef BUILD_TEST_EXE 
        DWORD dwErr = GetLastError();
        printf("[tl_usb_open_device] CreateFile失敗, error=%lu => TL_ERROR_DEVICE_OPEN_FAILED\n", dwErr);
#endif
        tl_set_last_error(TL_ERROR_DEVICE_OPEN_FAILED);
        unload_winusb_library();
        return TL_ERROR_DEVICE_OPEN_FAILED;
    }

    /* 6. WinUsb_Initialize -> primary interface */
    WINUSB_INTERFACE_HANDLE primaryInterface = NULL;
    if (!pWinUsb_Initialize(state->device_handle, &primaryInterface)) {
#ifdef BUILD_TEST_EXE 
        DWORD dwErr = GetLastError();
        printf("[tl_usb_open_device] WinUsb_Initialize失敗, error=%lu => TL_ERROR_DEVICE_OPEN_FAILED\n", dwErr);
#endif
        CloseHandle(state->device_handle);
        state->device_handle = NULL;
        tl_set_last_error(TL_ERROR_DEVICE_OPEN_FAILED);
        unload_winusb_library();
        return TL_ERROR_DEVICE_OPEN_FAILED;
    }

    /* 7. GetAssociatedInterface -> secondary interface */
    WINUSB_INTERFACE_HANDLE secondaryInterface = NULL;
    if (!pWinUsb_GetAssociatedInterface(primaryInterface, 0, &secondaryInterface)) {
#ifdef BUILD_TEST_EXE 
        DWORD dwErr = GetLastError();
        printf("[tl_usb_open_device] WinUsb_GetAssociatedInterface失敗, error=%lu => TL_ERROR_DEVICE_OPEN_FAILED\n", dwErr);
#endif
        pWinUsb_Free(primaryInterface);
        CloseHandle(state->device_handle);
        state->device_handle = NULL;
        tl_set_last_error(TL_ERROR_DEVICE_OPEN_FAILED);
        unload_winusb_library();
        return TL_ERROR_DEVICE_OPEN_FAILED;
    }

    /* 存到 state */
    state->interface_handle = secondaryInterface;

    /* 8. 檢查裝置是否就緒 (已擁有 interface_handle, 可嚴謹檢查) */
    if (!tl_usb_is_device_ready()) {
#ifdef BUILD_TEST_EXE 
        printf("[tl_usb_open_device] 裝置未就緒 => 關閉 handle.\n");
#endif
        pWinUsb_Free(state->interface_handle);
        state->interface_handle = NULL;
        CloseHandle(state->device_handle);
        state->device_handle = NULL;
        unload_winusb_library();
        tl_set_last_error(TL_ERROR_DEVICE_OPEN_FAILED);
        return TL_ERROR_DEVICE_OPEN_FAILED;
    }
#ifdef BUILD_TEST_EXE 
    printf("[tl_usb_open_device] 開啟裝置成功. (secondary interface 已取得)\n");
#endif
    return TL_SUCCESS;

#else
    /* 非Windows平台 => 模擬成功 */
    TL_InternalState* state = tl_get_internal_state();
    state->device_handle = (void*)1;
    state->interface_handle = (void*)1;
    return TL_SUCCESS;
#endif
}

/* -------------------------------------------------------------------------
 * 關閉裝置
 */
TL_ERROR_CODE tl_usb_close_device(void)
{
    TL_InternalState* state = tl_get_internal_state();
#ifdef _WIN32
    if (state->device_handle) {
        if (state->interface_handle) {
#ifdef BUILD_TEST_EXE 
            printf("[tl_usb_close_device] WinUsb_Free interface.\n");
#endif
            pWinUsb_Free((WINUSB_INTERFACE_HANDLE)state->interface_handle);
            state->interface_handle = NULL;
        }
#ifdef BUILD_TEST_EXE 
        printf("[tl_usb_close_device] CloseHandle device.\n");
#endif
        CloseHandle(state->device_handle);
        state->device_handle = NULL;
        unload_winusb_library();
    }
#endif
    return TL_SUCCESS;
}

/* -------------------------------------------------------------------------
 * 寫入資料
 */
TL_ERROR_CODE tl_usb_write_data(TL_BYTE pipe_id, const TL_BYTE* buffer, size_t buffer_size)
{
    TL_InternalState* state = tl_get_internal_state();

    if (!buffer || buffer_size == 0) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    if (!state->device_handle || !state->interface_handle) {
        tl_set_last_error(TL_ERROR_DEVICE_NOT_OPEN);
        return TL_ERROR_DEVICE_NOT_OPEN;
    }

#ifdef _WIN32
    ULONG bytesTransferred = 0;
    BOOL success = pWinUsb_WritePipe(
        (WINUSB_INTERFACE_HANDLE)state->interface_handle,
        pipe_id,
        (PUCHAR)buffer,
        (ULONG)buffer_size,
        &bytesTransferred,
        NULL
    );

    if (!success || bytesTransferred == 0) {
#ifdef BUILD_TEST_EXE 
        DWORD err = GetLastError();
        printf("[tl_usb_write_data] WritePipe失敗, error=%lu\n", err);
#endif
        tl_set_last_error(TL_ERROR_WRITE_FAILED);
        return TL_ERROR_WRITE_FAILED;
    }
    return TL_SUCCESS;
#else
    return TL_SUCCESS;
#endif
}

/* -------------------------------------------------------------------------
 * 讀取資料
 */
TL_ERROR_CODE tl_usb_read_data(TL_BYTE pipe_id, TL_BYTE* buffer, size_t buffer_size, size_t* bytes_read)
{
    TL_InternalState* state = tl_get_internal_state();

    if (!buffer || buffer_size == 0 || !bytes_read) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    *bytes_read = 0;

    if (!state->device_handle || !state->interface_handle) {
        tl_set_last_error(TL_ERROR_DEVICE_NOT_OPEN);
        return TL_ERROR_DEVICE_NOT_OPEN;
    }

#ifdef _WIN32
    ULONG bytesReceived = 0;
    BOOL success = pWinUsb_ReadPipe(
        (WINUSB_INTERFACE_HANDLE)state->interface_handle,
        pipe_id,
        (PUCHAR)buffer,
        (ULONG)buffer_size,
        &bytesReceived,
        NULL
    );
    if (!success) {
#ifdef BUILD_TEST_EXE
        DWORD err = GetLastError();
        printf("[tl_usb_read_data] ReadPipe失敗, error=%lu\n", err);
#endif
        tl_set_last_error(TL_ERROR_READ_FAILED);
        return TL_ERROR_READ_FAILED;
    }
    *bytes_read = (size_t)bytesReceived;
    return TL_SUCCESS;
#else
    return TL_SUCCESS;
#endif
}

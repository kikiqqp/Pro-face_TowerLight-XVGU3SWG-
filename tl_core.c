/*
 * tl_core.c
 *
 * 塔燈通訊控制函式庫 - 核心功能實現 (含除錯訊息)
 *
 * 本檔案實現了塔燈函式庫的核心功能，包括初始化、釋放、連接管理等。
 *
 * 版本: 1.0.0
 * 日期: 2025-03-25
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "tl_internal.h"

 /* 全局狀態變數 */
static TL_InternalState g_tl_state = {
    TL_FALSE,  /* is_initialized */
    TL_FALSE,  /* is_device_open */
    NULL,      /* device_handle */
    NULL,      /* interface_handle */
    TL_SUCCESS /* last_error */
};

/*
 * 錯誤訊息表 (若你有自己的 tl_messages.c 系統，也可以不用這裡)
 * 這裡只是示範簡單對應。
 */
static const char* g_error_messages[] = {
    "操作成功",                        /* TL_SUCCESS */
    "一般錯誤",                        /* TL_ERROR_GENERAL */
    "函式庫未初始化",                  /* TL_ERROR_NOT_INITIALIZED */
    "函式庫已初始化",                  /* TL_ERROR_ALREADY_INITIALIZED */
    "找不到塔燈裝置",                  /* TL_ERROR_DEVICE_NOT_FOUND */
    "無法開啟塔燈裝置",                /* TL_ERROR_DEVICE_OPEN_FAILED */
    "塔燈裝置未開啟",                  /* TL_ERROR_DEVICE_NOT_OPEN */
    "寫入操作失敗",                    /* TL_ERROR_WRITE_FAILED */
    "讀取操作失敗",                    /* TL_ERROR_READ_FAILED */
    "操作逾時",                        /* TL_ERROR_TIMEOUT */
    "無效的參數",                      /* TL_ERROR_INVALID_PARAMETER */
    "記憶體配置錯誤",                  /* TL_ERROR_MEMORY_ALLOCATION */
    "回應格式錯誤",                    /* TL_ERROR_RESPONSE_FORMAT */
    "回應校驗和錯誤",                  /* TL_ERROR_RESPONSE_CHECKSUM */
    "裝置拒絕命令",                    /* TL_ERROR_RESPONSE_NACK */
    "參數超出範圍"                     /* TL_ERROR_OUT_OF_RANGE */
};

/*
 * 設定最後一次的錯誤碼
 */
void tl_set_last_error(TL_ERROR_CODE error_code)
{
    g_tl_state.last_error = error_code;
}

/*
 * 檢查函式庫是否已初始化
 */
static TL_BOOL tl_is_initialized(void)
{
    return g_tl_state.is_initialized;
}

/*
 * 檢查裝置是否已開啟
 */
static TL_BOOL tl_is_device_open(void)
{
    return g_tl_state.is_device_open;
}

/*
 * 初始化塔燈函式庫
 */
TL_ERROR_CODE TL_Initialize(void)
{
    /* 檢查是否已初始化 */
    if (tl_is_initialized()) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_Initialize] 已初始化 => TL_ERROR_ALREADY_INITIALIZED\n");
#endif
        tl_set_last_error(TL_ERROR_ALREADY_INITIALIZED);
        return TL_ERROR_ALREADY_INITIALIZED;
    }

    /* 初始化內部狀態 */
    g_tl_state.is_initialized = TL_TRUE;
    g_tl_state.is_device_open = TL_FALSE;
    g_tl_state.device_handle = NULL;
    g_tl_state.interface_handle = NULL;
    g_tl_state.last_error = TL_SUCCESS;
#ifdef BUILD_TEST_EXE 
    printf("[TL_Initialize] 成功 => TL_SUCCESS\n");
#endif
    return TL_SUCCESS;
}

/*
 * 釋放塔燈函式庫資源
 */
TL_ERROR_CODE TL_Finalize(void)
{
    /* 檢查是否已初始化 */
    if (!tl_is_initialized()) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_Finalize] 未初始化 => TL_ERROR_NOT_INITIALIZED\n");
#endif
        tl_set_last_error(TL_ERROR_NOT_INITIALIZED);
        return TL_ERROR_NOT_INITIALIZED;
    }

    /* 如果裝置已開啟，先關閉它 */
    if (tl_is_device_open()) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_Finalize] 裝置目前已開啟, 呼叫TL_CloseConnection\n");
#endif
        TL_CloseConnection();
    }

    /* 重置內部狀態 */
    g_tl_state.is_initialized = TL_FALSE;
    g_tl_state.last_error = TL_SUCCESS;
#ifdef BUILD_TEST_EXE 
    printf("[TL_Finalize] 完成 => TL_SUCCESS\n");
#endif
    return TL_SUCCESS;
}

/*
 * 開啟塔燈連接
 */
TL_ERROR_CODE TL_OpenConnection(TL_BOOL clear_state)
{
    TL_ERROR_CODE error;

    /* 檢查是否已初始化 */
    if (!tl_is_initialized()) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_OpenConnection] 未初始化 => TL_ERROR_NOT_INITIALIZED\n");
#endif
        tl_set_last_error(TL_ERROR_NOT_INITIALIZED);
        return TL_ERROR_NOT_INITIALIZED;
    }

    /* 如果已經開啟，則直接返回成功 */
    if (tl_is_device_open()) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_OpenConnection] 已是開啟狀態 => 直接TL_SUCCESS\n");
#endif
        return TL_SUCCESS;
    }

    /* 開啟USB裝置 */
    error = tl_usb_open_device();
    if (error != TL_SUCCESS) {
        /* 失敗就回傳 */
#ifdef BUILD_TEST_EXE 
        printf("[TL_OpenConnection] tl_usb_open_device失敗 => 回傳=%d\n", error);
#endif
        return error;
    }

    /* 標記裝置已開啟 */
    g_tl_state.is_device_open = TL_TRUE;
#ifdef BUILD_TEST_EXE 
    printf("[TL_OpenConnection] 裝置開啟成功 => is_device_open=TRUE\n");
#endif

    /* 如果需要清除狀態 */
    if (clear_state) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_OpenConnection] clear_state=TRUE => 呼叫TL_ClearTowerLight\n");
#endif
        error = TL_ClearTowerLight();
        if (error != TL_SUCCESS) {
#ifdef BUILD_TEST_EXE 
            printf("[TL_OpenConnection] 清除塔燈狀態失敗, 但裝置已開啟. err=%d\n", error);
#endif
            /* 只記錄錯誤, 不關裝置 */
            tl_set_last_error(error);
        }
    }

    return TL_SUCCESS;
}

/*
 * 關閉塔燈連接
 */
TL_ERROR_CODE TL_CloseConnection(void)
{
    /* 檢查是否已初始化 */
    if (!tl_is_initialized()) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_CloseConnection] 未初始化 => TL_ERROR_NOT_INITIALIZED\n");
#endif
        tl_set_last_error(TL_ERROR_NOT_INITIALIZED);
        return TL_ERROR_NOT_INITIALIZED;
    }

    /* 檢查裝置是否已開啟 */
    if (!tl_is_device_open()) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_CloseConnection] 裝置本就沒開啟 => 視為成功\n");
#endif
        return TL_SUCCESS;
    }

    /* 關閉USB裝置 */
#ifdef BUILD_TEST_EXE 
    printf("[TL_CloseConnection] 呼叫 tl_usb_close_device\n");
#endif
    tl_usb_close_device();

    /* 重置裝置狀態 */
    g_tl_state.is_device_open = TL_FALSE;
    g_tl_state.device_handle = NULL;
    g_tl_state.interface_handle = NULL;
#ifdef BUILD_TEST_EXE 
    printf("[TL_CloseConnection] 完成 => TL_SUCCESS\n");
#endif
    return TL_SUCCESS;
}

/*
 * 檢查塔燈連接狀態
 */
TL_BOOL TL_IsConnected(void)
{
    /* 檢查是否已初始化且裝置已開啟 */
    if (!tl_is_initialized() || !tl_is_device_open()) {
        return TL_FALSE;
    }

    /* 這裡我們假設裝置已開啟就是已連接 */
    return TL_TRUE;
}

/*
 * 清除塔燈 (LED全部關、蜂鳴器停止)
 */
TL_ERROR_CODE TL_ClearTowerLight(void)
{
    TL_ERROR_CODE error;
#ifdef BUILD_TEST_EXE 
    printf("[TL_ClearTowerLight] 執行TL_ClearAllLEDs\n");
#endif
    error = TL_ClearAllLEDs();
    if (error != TL_SUCCESS) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_ClearTowerLight] TL_ClearAllLEDs失敗 => err=%d\n", error);
#endif
        return error;
    }
#ifdef BUILD_TEST_EXE 
    printf("[TL_ClearTowerLight] 執行TL_StopBuzzer\n");
#endif
    error = TL_StopBuzzer();
    if (error != TL_SUCCESS) {
#ifdef BUILD_TEST_EXE 
        printf("[TL_ClearTowerLight] TL_StopBuzzer失敗 => err=%d\n", error);
#endif
        return error;
    }
#ifdef BUILD_TEST_EXE 
    printf("[TL_ClearTowerLight] 完成 => TL_SUCCESS\n");
#endif
    return TL_SUCCESS;
}

/*
 * 獲取最後一次發生的錯誤碼
 */
TL_ERROR_CODE TL_GetLastError(void)
{
    return g_tl_state.last_error;
}

/*
 * 獲取錯誤碼對應的錯誤訊息
 */
TL_ERROR_CODE TL_GetErrorMessage(TL_ERROR_CODE error_code, char* buffer, size_t buffer_size)
{
    /* 驗證參數 */
    if (buffer == NULL || buffer_size == 0) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }

    /* 檢查錯誤碼是否在有效範圍內 */
    size_t count = sizeof(g_error_messages) / sizeof(g_error_messages[0]);
    if (error_code >= 0 && (size_t)error_code < count) {
        strncpy(buffer, g_error_messages[error_code], buffer_size);
        buffer[buffer_size - 1] = '\0'; /* 確保字串結尾 */
    }
    else {
        strncpy(buffer, "未知錯誤", buffer_size);
        buffer[buffer_size - 1] = '\0';
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }

    return TL_SUCCESS;
}

/*
 * 供其他模組 (例如 tl_command.c) 取得內部狀態指標
 */
TL_InternalState* tl_get_internal_state(void)
{
    return &g_tl_state;
}

/*
 * 延遲指定的毫秒數
 */
void tl_delay_ms(unsigned long ms)
{
#ifdef _WIN32
    /* Windows平台 */
    Sleep(ms);
#else
    /* 非Windows平台 - 使用標準C的方式 */
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
#endif
}


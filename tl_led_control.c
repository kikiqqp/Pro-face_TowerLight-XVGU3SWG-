/*
 * tl_led_control.c
 * 
 * 塔燈通訊控制函式庫 - LED控制功能實現
 * 
 * 本檔案實現了塔燈函式庫的LED控制相關功能，包括設置LED狀態、
 * 讀取LED狀態等。
 * 
 * 版本: 1.0.0
 * 日期: 2025-03-25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tl_internal.h"

/* 獲取內部狀態 - 使用外部聲明的函數 */
extern TL_InternalState* tl_get_internal_state(void);

/*
 * 設定特定層LED的狀態
 */
TL_ERROR_CODE TL_SetLED(TL_LAYER layer, const TL_LEDStatus* status) {
    TL_InternalState* state;
    TL_BYTE command[TL_MAX_BUFFER_SIZE];
    size_t command_length;
    TL_BYTE response[TL_MAX_BUFFER_SIZE];
    size_t response_length;
    TL_ERROR_CODE result;
    
    /* 參數驗證 */
    if (status == NULL) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 檢查層級是否有效 */
    if (layer < TL_LAYER_ONE || layer > TL_LAYER_THREE) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 獲取內部狀態 */
    state = tl_get_internal_state();
    
    /* 檢查函式庫是否已初始化 */
    if (!state->is_initialized) {
        tl_set_last_error(TL_ERROR_NOT_INITIALIZED);
        return TL_ERROR_NOT_INITIALIZED;
    }
    
    /* 檢查裝置是否已開啟 */
    if (!state->is_device_open) {
        tl_set_last_error(TL_ERROR_DEVICE_NOT_OPEN);
        return TL_ERROR_DEVICE_NOT_OPEN;
    }
    
    /* 構建設定命令 */
    command_length = tl_cmd_build_led_command(layer, status, command, TL_MAX_BUFFER_SIZE);
    if (command_length == 0) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 發送命令並接收回應 */
    result = tl_cmd_send_and_receive(command, command_length, response, TL_MAX_BUFFER_SIZE, &response_length);
    if (result != TL_SUCCESS) {
        return result;
    }
    
    /* 檢查回應格式 */
    result = tl_cmd_check_response_format(response, response_length);
    if (result != TL_SUCCESS) {
        return result;
    }
    
    return TL_SUCCESS;
}

/*
 * 獲取特定層LED的狀態
 */
TL_ERROR_CODE TL_GetLEDStatus(TL_LAYER layer, TL_LEDStatus* status) {
    TL_InternalState* state;
    TL_BYTE command[TL_MAX_BUFFER_SIZE];
    size_t command_length;
    TL_BYTE response[TL_MAX_BUFFER_SIZE];
    size_t response_length;
    TL_ERROR_CODE result;
    
    /* 參數驗證 */
    if (status == NULL) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 檢查層級是否有效 */
    if (layer < TL_LAYER_ONE || layer > TL_LAYER_THREE) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 獲取內部狀態 */
    state = tl_get_internal_state();
    
    /* 檢查函式庫是否已初始化 */
    if (!state->is_initialized) {
        tl_set_last_error(TL_ERROR_NOT_INITIALIZED);
        return TL_ERROR_NOT_INITIALIZED;
    }
    
    /* 檢查裝置是否已開啟 */
    if (!state->is_device_open) {
        tl_set_last_error(TL_ERROR_DEVICE_NOT_OPEN);
        return TL_ERROR_DEVICE_NOT_OPEN;
    }
    
    /* 構建狀態讀取命令 */
    command_length = tl_cmd_build_status_read_command((TL_BYTE)layer, command, TL_MAX_BUFFER_SIZE);
    if (command_length == 0) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 發送命令並接收回應 */
    result = tl_cmd_send_and_receive(command, command_length, response, TL_MAX_BUFFER_SIZE, &response_length);
    if (result != TL_SUCCESS) {
        return result;
    }
    
    /* 檢查回應格式 */
    result = tl_cmd_check_response_format(response, response_length);
    if (result != TL_SUCCESS) {
        return result;
    }
    
    /* 解析回應並填充狀態結構 */
    result = tl_cmd_parse_led_status(response, response_length, status);
    if (result != TL_SUCCESS) {
        return result;
    }
    
    return TL_SUCCESS;
}

/*
 * 清除所有LED
 */
TL_ERROR_CODE TL_ClearAllLEDs(void) {
    TL_ERROR_CODE result;
    TL_LEDStatus status;
    int i;
    
    /* 初始化狀態結構，設定所有LED為關閉 */
    status.red_status = TL_LED_OFF;
    status.green_status = TL_LED_OFF;
    status.blue_status = TL_LED_OFF;
    status.pattern = TL_LED_PATTERN_OFF;
    
    /* 清除每一層的LED */
    for (i = TL_LAYER_ONE; i <= TL_LAYER_THREE; i++) {
        result = TL_SetLED((TL_LAYER)i, &status);
        if (result != TL_SUCCESS) {
            return result;
        }
    }
    
    return TL_SUCCESS;
}
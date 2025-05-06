/*
 * tl_buzzer_control.c
 * 
 * 塔燈通訊控制函式庫 - 蜂鳴器控制功能實現
 * 
 * 本檔案實現了塔燈函式庫的蜂鳴器控制相關功能，包括設置蜂鳴器狀態、
 * 讀取蜂鳴器狀態、停止蜂鳴器等。
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
 * 設定蜂鳴器狀態
 */
TL_ERROR_CODE TL_SetBuzzer(const TL_BuzzerStatus* status) {
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
    command_length = tl_cmd_build_buzzer_command(status, command, TL_MAX_BUFFER_SIZE);
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
 * 獲取蜂鳴器狀態
 */
TL_ERROR_CODE TL_GetBuzzerStatus(TL_BuzzerStatus* status) {
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
    
    /* 構建狀態讀取命令 - 使用固定值3表示讀取蜂鳴器狀態 */
    command_length = tl_cmd_build_status_read_command(3, command, TL_MAX_BUFFER_SIZE);
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
    result = tl_cmd_parse_buzzer_status(response, response_length, status);
    if (result != TL_SUCCESS) {
        return result;
    }
    
    return TL_SUCCESS;
}

/*
 * 停止蜂鳴器
 */
TL_ERROR_CODE TL_StopBuzzer(void) {
    TL_BuzzerStatus status;
    
    /* 初始化狀態結構，設定蜂鳴器為關閉 */
    status.tone = TL_BUZZER_TONE_HIGH;
    status.volume = TL_BUZZER_VOLUME_MEDIUM;
    status.pattern = TL_BUZZER_PATTERN_OFF;
    
    /* 通過設定命令停止蜂鳴器 */
    return TL_SetBuzzer(&status);
}
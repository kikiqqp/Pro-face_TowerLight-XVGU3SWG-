/*
 * tl_error.c
 * 
 * 塔燈通訊控制函式庫 - 錯誤處理功能實現
 * 
 * 本檔案實現了塔燈函式庫的錯誤處理相關功能，包括錯誤訊息管理等。
 * 
 * 版本: 1.0.0
 * 日期: 2025-03-25
 */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl_internal.h"
#include "tl_messages.h"

/*
 * 獲取錯誤訊息
 *
 * 返回指定錯誤碼對應的錯誤訊息。
 *
 * 參數：error_code 錯誤碼
 * 參數：buffer 用於存儲錯誤訊息的緩衝區
 * 參數：buffer_size 緩衝區大小
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_get_error_message(TL_ERROR_CODE error_code, char* buffer, size_t buffer_size) {
    const char* message;
    size_t message_length;
    
    /* 參數驗證 */
    if (buffer == NULL || buffer_size == 0) {
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 確保緩衝區最後一個字元為 NULL */
    buffer[buffer_size - 1] = '\0';
    
    /* 錯誤碼直接對應訊息ID */
    if (error_code >= 0 && error_code < TL_MSG_ID_COUNT) {
        message = tl_messages_get(error_code);
        message_length = strlen(message);
        
        /* 複製錯誤訊息到緩衝區 */
        if (message_length < buffer_size - 1) {
            strcpy(buffer, message);
        } else {
            /* 緩衝區不夠大，只複製部分訊息 */
            strncpy(buffer, message, buffer_size - 1);
        }
    } else {
        /* 未知錯誤碼 */
        message = tl_messages_get(TL_MSG_ID_UNKNOWN_ERROR);
        strncpy(buffer, message, buffer_size - 1);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    return TL_SUCCESS;
}
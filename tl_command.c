/*
 * tl_command.c
 * 
 * 塔燈通訊控制函式庫 - 命令處理功能實現
 * 
 * 本檔案實現了塔燈函式庫的命令處理相關功能，包括命令構建、
 * 解析回應、校驗和計算等。
 * 
 * 版本: 1.0.0
 * 日期: 2025-03-25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tl_internal.h"
#include "tl_messages.h"

/* 獲取內部狀態 - 使用外部聲明的函數 */
extern TL_InternalState* tl_get_internal_state(void);

/*
 * 計算校驗和
 */
TL_BYTE tl_cmd_calculate_checksum(const TL_BYTE* buffer, size_t length) {
    TL_BYTE checksum = 0;
    size_t i;
    
    /* 如果參數無效，則返回0 */
    if (buffer == NULL || length == 0) {
        return 0;
    }
    
    /* 計算校驗和 - 簡單地將所有位元組相加 */
    for (i = 0; i < length; i++) {
        checksum += buffer[i];
    }
    
    return checksum;
}

/*
 * 構建LED設定命令
 */
size_t tl_cmd_build_led_command(TL_LAYER layer, const TL_LEDStatus* status, 
                               TL_BYTE* buffer, size_t buffer_size) {
    TL_BYTE checksum;
    TL_WORD data_length = 5; /* LED設定命令數據長度固定為5位元組 */
    size_t total_length;
    
    /* 參數驗證 */
    if (status == NULL || buffer == NULL || buffer_size < 11) {
        return 0;
    }
    
    /* 檢查層級是否有效 */
    if (layer < TL_LAYER_ONE || layer > TL_LAYER_THREE) {
        return 0;
    }
    
    /* 檢查其他參數是否有效 */
    if (status->red_status > TL_LED_DUTY || 
        status->green_status > TL_LED_DUTY || 
        status->blue_status > TL_LED_DUTY || 
        status->pattern > TL_LED_PATTERN_BLINK2) {
        return 0;
    }
    
    /* 構建命令 */
    /* 封包結構: [ESC] [CMD] [DataLen-H] [DataLen-L] [Layer] [Red] [Green] [Blue] [Pattern] [Checksum] [CR] */
    
    /* 封包起始符 ESC (0x1B) */
    buffer[0] = TL_PKT_START;
    
    /* 命令類型 - LED設定 (0x01) */
    buffer[1] = TL_CMD_LED_SET;
    
    /* 數據長度 - 高位元組 */
    buffer[2] = (TL_BYTE)((data_length >> 8) & 0xFF);
    
    /* 數據長度 - 低位元組 */
    buffer[3] = (TL_BYTE)(data_length & 0xFF);
    
    /* 層級 */
    buffer[4] = (TL_BYTE)layer;
    
    /* 紅色LED狀態 */
    buffer[5] = (TL_BYTE)status->red_status;
    
    /* 綠色LED狀態 */
    buffer[6] = (TL_BYTE)status->green_status;
    
    /* 藍色LED狀態 */
    buffer[7] = (TL_BYTE)status->blue_status;
    
    /* 閃爍模式 */
    buffer[8] = (TL_BYTE)status->pattern;
    
    /* 計算校驗和 - 從命令類型開始到數據結束 */
    checksum = tl_cmd_calculate_checksum(&buffer[1], 8);
    buffer[9] = checksum;
    
    /* 封包結束符 CR (0x0D) */
    buffer[10] = TL_PKT_END;
    
    /* 返回總長度 */
    total_length = 11;
    return total_length;
}

/*
 * 構建蜂鳴器設定命令
 */
size_t tl_cmd_build_buzzer_command(const TL_BuzzerStatus* status, 
                                  TL_BYTE* buffer, size_t buffer_size) {
    TL_BYTE checksum;
    TL_WORD data_length = 3; /* 蜂鳴器設定命令數據長度固定為3位元組 */
    size_t total_length;
    
    /* 參數驗證 */
    if (status == NULL || buffer == NULL || buffer_size < 9) {
        return 0;
    }
    
    /* 檢查參數是否有效 */
    if (status->tone > TL_BUZZER_TONE_LOW || 
        status->volume > TL_BUZZER_VOLUME_SMALL || 
        status->pattern > TL_BUZZER_PATTERN_4) {
        return 0;
    }
    
    /* 構建命令 */
    /* 封包結構: [ESC] [CMD] [DataLen-H] [DataLen-L] [Tone] [Volume] [Pattern] [Checksum] [CR] */
    
    /* 封包起始符 ESC (0x1B) */
    buffer[0] = TL_PKT_START;
    
    /* 命令類型 - 蜂鳴器設定 (0x02) */
    buffer[1] = TL_CMD_BUZZER_SET;
    
    /* 數據長度 - 高位元組 */
    buffer[2] = (TL_BYTE)((data_length >> 8) & 0xFF);
    
    /* 數據長度 - 低位元組 */
    buffer[3] = (TL_BYTE)(data_length & 0xFF);
    
    /* 音調 */
    buffer[4] = (TL_BYTE)status->tone;
    
    /* 音量 */
    buffer[5] = (TL_BYTE)status->volume;
    
    /* 模式 */
    buffer[6] = (TL_BYTE)status->pattern;
    
    /* 計算校驗和 - 從命令類型開始到數據結束 */
    checksum = tl_cmd_calculate_checksum(&buffer[1], 6);
    buffer[7] = checksum;
    
    /* 封包結束符 CR (0x0D) */
    buffer[8] = TL_PKT_END;
    
    /* 返回總長度 */
    total_length = 9;
    return total_length;
}

/*
 * 構建狀態讀取命令
 */
size_t tl_cmd_build_status_read_command(TL_BYTE type, TL_BYTE* buffer, size_t buffer_size) {
    TL_BYTE checksum;
    TL_WORD data_length = 1; /* 狀態讀取命令數據長度固定為1位元組 */
    size_t total_length;
    
    /* 參數驗證 */
    if (buffer == NULL || buffer_size < 7) {
        return 0;
    }
    
    /* 檢查類型是否有效 */
    if (type > 3) { /* 0-2: LED層級, 3: 蜂鳴器 */
        return 0;
    }
    
    /* 構建命令 */
    /* 封包結構: [ESC] [CMD] [DataLen-H] [DataLen-L] [Type] [Checksum] [CR] */
    
    /* 封包起始符 ESC (0x1B) */
    buffer[0] = TL_PKT_START;
    
    /* 命令類型 - 狀態讀取 (0x03) */
    buffer[1] = TL_CMD_STATUS_READ;
    
    /* 數據長度 - 高位元組 */
    buffer[2] = (TL_BYTE)((data_length >> 8) & 0xFF);
    
    /* 數據長度 - 低位元組 */
    buffer[3] = (TL_BYTE)(data_length & 0xFF);
    
    /* 狀態類型 */
    buffer[4] = type;
    
    /* 計算校驗和 - 從命令類型開始到數據結束 */
    checksum = tl_cmd_calculate_checksum(&buffer[1], 4);
    buffer[5] = checksum;
    
    /* 封包結束符 CR (0x0D) */
    buffer[6] = TL_PKT_END;
    
    /* 返回總長度 */
    total_length = 7;
    return total_length;
}

/*
 * 檢查回應格式
 */
TL_ERROR_CODE tl_cmd_check_response_format(const TL_BYTE* response, size_t response_length) {
    TL_BYTE calculated_checksum;
    TL_WORD data_length;
    size_t expected_length;
    
    /* 參數驗證 */
    if (response == NULL || response_length < 6) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 檢查起始符 */
    if (response[0] != TL_PKT_START) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 獲取數據長度 */
    data_length = (response[2] << 8) | response[3];
    
    /* 計算預期的總長度 */
    expected_length = 4 + data_length + 2; /* 頭部(4) + 數據(data_length) + 校驗和(1) + 結束符(1) */
    
    /* 檢查長度是否匹配 */
    if (response_length != expected_length) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 計算校驗和 */
    calculated_checksum = tl_cmd_calculate_checksum(&response[1], 3 + data_length);
    
    /* 檢查校驗和是否匹配 */
    if (calculated_checksum != response[response_length - 2]) {
        tl_set_last_error(TL_ERROR_RESPONSE_CHECKSUM);
        return TL_ERROR_RESPONSE_CHECKSUM;
    }
    
    /* 檢查結束符 */
    if (response[response_length - 1] != TL_PKT_END) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 檢查回應類型 - 應為ACK(0x06) */
    if (response[4] != TL_RSP_ACK) {
        tl_set_last_error(TL_ERROR_RESPONSE_NACK);
        return TL_ERROR_RESPONSE_NACK;
    }
    
    return TL_SUCCESS;
}

/*
 * 解析LED狀態回應
 */
TL_ERROR_CODE tl_cmd_parse_led_status(const TL_BYTE* response, size_t response_length, TL_LEDStatus* status) {
    /* 參數驗證 */
    if (response == NULL || status == NULL || response_length < 12) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 確認回應格式正確且為LED狀態回應 */
    if (response[1] != TL_CMD_STATUS_READ) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 獲取紅色LED狀態 */
    status->red_status = (TL_LED_STATE)response[6];
    if (status->red_status > TL_LED_DUTY) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 獲取綠色LED狀態 */
    status->green_status = (TL_LED_STATE)response[7];
    if (status->green_status > TL_LED_DUTY) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 獲取藍色LED狀態 */
    status->blue_status = (TL_LED_STATE)response[8];
    if (status->blue_status > TL_LED_DUTY) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 獲取LED閃爍模式 */
    status->pattern = (TL_LED_PATTERN)response[9];
    if (status->pattern > TL_LED_PATTERN_BLINK2) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    return TL_SUCCESS;
}

/*
 * 解析蜂鳴器狀態回應
 */
TL_ERROR_CODE tl_cmd_parse_buzzer_status(const TL_BYTE* response, size_t response_length, TL_BuzzerStatus* status) {
    /* 參數驗證 */
    if (response == NULL || status == NULL || response_length < 10) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 確認回應格式正確且為蜂鳴器狀態回應 */
    if (response[1] != TL_CMD_STATUS_READ) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 獲取音調 */
    status->tone = (TL_BUZZER_TONE)response[5];
    if (status->tone > TL_BUZZER_TONE_LOW) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 獲取音量 */
    status->volume = (TL_BUZZER_VOLUME)response[6];
    if (status->volume > TL_BUZZER_VOLUME_SMALL) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    /* 獲取模式 */
    status->pattern = (TL_BUZZER_PATTERN)response[7];
    if (status->pattern > TL_BUZZER_PATTERN_4) {
        tl_set_last_error(TL_ERROR_RESPONSE_FORMAT);
        return TL_ERROR_RESPONSE_FORMAT;
    }
    
    return TL_SUCCESS;
}

/*
 * 發送命令並接收回應
 */
TL_ERROR_CODE tl_cmd_send_and_receive(const TL_BYTE* command, size_t command_length,
                                     TL_BYTE* response, size_t response_size,
                                     size_t* response_length) {
    TL_InternalState* state;
    TL_ERROR_CODE result;
    int timeout_counter = 0;
    size_t received_header_size = 0;
    size_t received_data_size = 0;
    TL_BYTE header_buffer[4];
    size_t bytes_read;
    size_t total_data_size = 0;
    
    /* 參數驗證 */
    if (command == NULL || command_length == 0 || response == NULL || 
        response_size < 6 || response_length == NULL) {
        tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
        return TL_ERROR_INVALID_PARAMETER;
    }
    
    /* 獲取內部狀態 */
    state = tl_get_internal_state();
    
    /* 檢查裝置是否已開啟 */
    if (state->device_handle == NULL || state->interface_handle == NULL) {
        tl_set_last_error(TL_ERROR_DEVICE_NOT_OPEN);
        return TL_ERROR_DEVICE_NOT_OPEN;
    }
    
    /* 發送命令 */
    result = tl_usb_write_data(TL_PIPE_ID, command, command_length);
    if (result != TL_SUCCESS) {
        return result;
    }
    
    /* 接收回應 */
    /* 首先讀取回應頭部 (4位元組) */
    *response_length = 0;
    
    while (1) {
        /* 讀取頭部 */
        if (received_header_size < 4) {
            result = tl_usb_read_data(TL_RESPONSE_PIPE, header_buffer + received_header_size, 
                                    4 - received_header_size, &bytes_read);
            if (result != TL_SUCCESS) {
                return result;
            }
            
            received_header_size += bytes_read;
            
            /* 頭部已完全讀取，計算數據部分大小 */
            if (received_header_size == 4) {
                total_data_size = header_buffer[2] * 256 + header_buffer[3] + 2; /* 數據長度 + 校驗和 + 結束符 */
                
                /* 檢查緩衝區大小是否足夠 */
                if (response_size < 4 + total_data_size) {
                    tl_set_last_error(TL_ERROR_INVALID_PARAMETER);
                    return TL_ERROR_INVALID_PARAMETER;
                }
                
                /* 將頭部複製到回應緩衝區 */
                memcpy(response, header_buffer, 4);
                *response_length = 4;
            }
        }
        
        /* 讀取數據部分 */
        if (received_header_size == 4 && total_data_size > 0) {
            result = tl_usb_read_data(TL_RESPONSE_PIPE, response + *response_length, 
                                    total_data_size, &bytes_read);
            if (result != TL_SUCCESS) {
                return result;
            }
            
            *response_length += bytes_read;
            
            /* 檢查是否已完全讀取 */
            if (*response_length == 4 + total_data_size) {

#ifdef BUILD_TEST_EXE 
                /* 在成功讀取頭部 & 數據後: */
                printf("[tl_cmd_send_and_receive] response_length=%zu\n", *response_length);
                printf("[tl_cmd_send_and_receive] response data:");
                for (size_t i = 0; i < *response_length; i++) {
                    printf(" %02X", response[i]);
                }
                printf("\n");
#endif

                return TL_SUCCESS;
            }
        }
        
        /* 檢查逾時 */
        timeout_counter++;
        if (timeout_counter >= TL_READ_TIMEOUT / 10) { /* 10毫秒一次檢查，共檢查TL_READ_TIMEOUT/10次 */
            tl_set_last_error(TL_ERROR_TIMEOUT);
            return TL_ERROR_TIMEOUT;
        }
        
        /* 短暫等待後再次嘗試 */
        tl_delay_ms(10);
    }
    
    /* 理論上不應該執行到這裡 */
    return TL_ERROR_GENERAL;
}

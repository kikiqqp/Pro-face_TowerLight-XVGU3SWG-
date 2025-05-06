/*
 * tl_internal.h
 * 
 * 塔燈通訊控制函式庫 - 內部使用標頭檔
 * 
 * 本檔案定義了內部使用的函式和資料結構，不應被應用程式直接引用。
 * 
 * 版本: 1.0.0
 * 日期: 2025-03-25
 */

#ifndef TL_INTERNAL_H
#define TL_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tl_tower_light.h"

/* 塔燈通訊相關常數 */
#define TL_DEVICE_ID      "Vid_16DE&Pid_000C"  /* 塔燈裝置識別碼 */
#define TL_PIPE_ID        2                     /* 寫入管道ID */
#define TL_RESPONSE_PIPE  130                   /* 回應管道ID */
#define TL_GUID_DATA1     1490055696           /* GUID Data1 部分 */
#define TL_GUID_DATA2     10177                /* GUID Data2 部分 */
#define TL_GUID_DATA3     4573                 /* GUID Data3 部分 */
#define TL_GUID_DATA4_0   189                  /* GUID Data4 部分[0] */
#define TL_GUID_DATA4_1   11                   /* GUID Data4 部分[1] */
#define TL_GUID_DATA4_2   8                    /* GUID Data4 部分[2] */
#define TL_GUID_DATA4_3   0                    /* GUID Data4 部分[3] */
#define TL_GUID_DATA4_4   32                   /* GUID Data4 部分[4] */
#define TL_GUID_DATA4_5   12                   /* GUID Data4 部分[5] */
#define TL_GUID_DATA4_6   154                  /* GUID Data4 部分[6] */
#define TL_GUID_DATA4_7   102                  /* GUID Data4 部分[7] */

/* 收發緩衝區最大大小 */
#define TL_MAX_BUFFER_SIZE 256

/* 命令類型定義 */
#define TL_CMD_LED_SET    1  /* LED設定命令 */
#define TL_CMD_BUZZER_SET 2  /* 蜂鳴器設定命令 */
#define TL_CMD_STATUS_READ 3  /* 狀態讀取命令 */

/* 封包格式相關常數 */
#define TL_PKT_START      27  /* 封包起始符 ESC (0x1B) */
#define TL_PKT_END        13  /* 封包結束符 CR (0x0D) */
#define TL_RSP_ACK        6   /* 回應確認碼 ACK (0x06) */
#define TL_RSP_NAK        21  /* 回應否認碼 NAK (0x15) */

/* 讀取超時時間 (毫秒) */
#define TL_READ_TIMEOUT   1000  /* 1秒 */

/* 裝置準備狀態檢查的最大重試次數 */
#define TL_MAX_DEVICE_READY_ATTEMPTS  5

/* 每次重試的等待時間 (毫秒) */
#define TL_DEVICE_READY_WAIT_MS  10

/* 全局狀態資訊 */
typedef struct {
    TL_BOOL is_initialized;    /* 函式庫是否已初始化 */
    TL_BOOL is_device_open;    /* 裝置是否已開啟 */
    void*   device_handle;     /* 裝置控制代碼 */
    void*   interface_handle;  /* 介面控制代碼 */
    TL_ERROR_CODE last_error;  /* 最後一次錯誤碼 */
} TL_InternalState;

/* 命令封包結構 */
typedef struct {
    TL_BYTE cmd_type;    /* 命令類型 */
    TL_BYTE* data;       /* 命令數據 */
    size_t data_length;  /* 數據長度 */
} TL_CommandPacket;

/* 回應封包結構 */
typedef struct {
    TL_BYTE* buffer;     /* 回應數據緩衝區 */
    size_t buffer_size;  /* 緩衝區大小 */
    size_t data_length;  /* 實際數據長度 */
} TL_ResponsePacket;

/*
 * 設定最後一次的錯誤碼
 * 
 * 設定最後一次操作的錯誤碼。
 *
 * 參數：error_code 要設定的錯誤碼
 */
void tl_set_last_error(TL_ERROR_CODE error_code);

/*
 * 開啟USB裝置
 * 
 * 嘗試開啟USB裝置。
 * 
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_usb_open_device(void);

/*
 * 關閉USB裝置
 * 
 * 關閉已開啟的USB裝置。
 * 
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_usb_close_device(void);

/*
 * 檢查USB裝置是否準備就緒
 * 
 * 檢查裝置是否已初始化且準備接收命令。
 * 
 * 返回值：TL_TRUE 表示準備就緒，TL_FALSE 表示未準備就緒
 */
TL_BOOL tl_usb_is_device_ready(void);

/*
 * 寫入資料到USB裝置
 * 
 * 將數據寫入USB裝置。
 * 
 * 參數：pipe_id 管道ID
 * 參數：buffer 要寫入的數據緩衝區
 * 參數：buffer_size 緩衝區大小
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_usb_write_data(TL_BYTE pipe_id, const TL_BYTE* buffer, size_t buffer_size);

/*
 * 從USB裝置讀取資料
 * 
 * 從USB裝置讀取數據。
 * 
 * 參數：pipe_id 管道ID
 * 參數：buffer 用於存儲讀取數據的緩衝區
 * 參數：buffer_size 緩衝區大小
 * 參數：bytes_read 實際讀取的字節數
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_usb_read_data(TL_BYTE pipe_id, TL_BYTE* buffer, size_t buffer_size, size_t* bytes_read);

/*
 * 延遲指定的毫秒數
 *
 * 提供一個跨平台的延遲實現。
 *
 * 參數：ms 要延遲的毫秒數
 */
void tl_delay_ms(unsigned long ms);

/*
 * 構建LED設定命令
 * 
 * 根據LED狀態構建設定命令。
 * 
 * 參數：layer 要設定的層級
 * 參數：status LED狀態結構
 * 參數：buffer 用於存儲命令的緩衝區
 * 參數：buffer_size 緩衝區大小
 * 返回值：構建的命令長度，0表示失敗
 */
size_t tl_cmd_build_led_command(TL_LAYER layer, const TL_LEDStatus* status, 
                               TL_BYTE* buffer, size_t buffer_size);

/*
 * 構建蜂鳴器設定命令
 * 
 * 根據蜂鳴器狀態構建設定命令。
 * 
 * 參數：status 蜂鳴器狀態結構
 * 參數：buffer 用於存儲命令的緩衝區
 * 參數：buffer_size 緩衝區大小
 * 返回值：構建的命令長度，0表示失敗
 */
size_t tl_cmd_build_buzzer_command(const TL_BuzzerStatus* status, 
                                  TL_BYTE* buffer, size_t buffer_size);

/*
 * 構建狀態讀取命令
 * 
 * 構建用於讀取特定狀態的命令。
 * 
 * 參數：type 要讀取的狀態類型
 * 參數：buffer 用於存儲命令的緩衝區
 * 參數：buffer_size 緩衝區大小
 * 返回值：構建的命令長度，0表示失敗
 */
size_t tl_cmd_build_status_read_command(TL_BYTE type, TL_BYTE* buffer, size_t buffer_size);

/*
 * 計算校驗和
 * 
 * 計算數據緩衝區的校驗和。
 * 
 * 參數：buffer 數據緩衝區
 * 參數：length 緩衝區長度
 * 返回值：計算出的校驗和
 */
TL_BYTE tl_cmd_calculate_checksum(const TL_BYTE* buffer, size_t length);

/*
 * 解析LED狀態回應
 * 
 * 解析從裝置收到的LED狀態回應。
 * 
 * 參數：response 回應數據緩衝區
 * 參數：response_length 回應數據長度
 * 參數：status 用於存儲LED狀態的結構
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_cmd_parse_led_status(const TL_BYTE* response, size_t response_length, TL_LEDStatus* status);

/*
 * 解析蜂鳴器狀態回應
 * 
 * 解析從裝置收到的蜂鳴器狀態回應。
 * 
 * 參數：response 回應數據緩衝區
 * 參數：response_length 回應數據長度
 * 參數：status 用於存儲蜂鳴器狀態的結構
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_cmd_parse_buzzer_status(const TL_BYTE* response, size_t response_length, TL_BuzzerStatus* status);

/*
 * 檢查回應格式
 * 
 * 檢查回應數據的格式是否有效。
 * 
 * 參數：response 回應數據緩衝區
 * 參數：response_length 回應數據長度
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_cmd_check_response_format(const TL_BYTE* response, size_t response_length);

/*
 * 發送命令並接收回應
 * 
 * 發送命令給塔燈裝置並等待回應。
 * 
 * 參數：command 命令緩衝區
 * 參數：command_length 命令長度
 * 參數：response 回應緩衝區
 * 參數：response_size 回應緩衝區大小
 * 參數：response_length 實際接收到的回應長度
 * 返回值：TL_SUCCESS 表示成功，其他值表示錯誤碼
 */
TL_ERROR_CODE tl_cmd_send_and_receive(const TL_BYTE* command, size_t command_length,
                                     TL_BYTE* response, size_t response_size,
                                     size_t* response_length);


#ifdef __cplusplus
}
#endif

#endif /* TL_INTERNAL_H */
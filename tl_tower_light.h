/**
 * tl_tower_light.h
 *
 * 塔燈通訊控制函式庫
 *
 */

#ifndef TL_TOWER_LIGHT_H
#define TL_TOWER_LIGHT_H

#ifdef __cplusplus
extern "C" {
#endif

    /* 標準類型定義 */
#ifndef TL_TYPES_DEFINED
#define TL_TYPES_DEFINED

#include <stddef.h>

/* 基本資料類型定義 */
    typedef unsigned char TL_BYTE;
    typedef unsigned short TL_WORD;
    typedef unsigned long TL_DWORD;
    typedef int TL_BOOL;

    /* 布林值常數 */
#define TL_FALSE 0
#define TL_TRUE 1
#define FALSE 0
#define TRUE 1

#endif /* TL_TYPES_DEFINED */

/* 導出符號定義 */
#ifdef _WIN32
#ifdef TL_TOWER_LIGHT_EXPORTS
#define TL_API __declspec(dllexport)
#else
#define TL_API __declspec(dllimport)
#endif
#else
#define TL_API
#endif

/* 錯誤碼定義 */
    typedef enum {
        TL_SUCCESS = 0,     /* 操作成功 */
        TL_ERROR_GENERAL = 1,     /* 一般錯誤 */
        TL_ERROR_NOT_INITIALIZED = 2,     /* 函式庫未初始化 */
        TL_ERROR_ALREADY_INITIALIZED = 3,    /* 函式庫已初始化 */
        TL_ERROR_DEVICE_NOT_FOUND = 4,     /* 找不到塔燈裝置 */
        TL_ERROR_DEVICE_OPEN_FAILED = 5,     /* 無法開啟塔燈裝置 */
        TL_ERROR_DEVICE_NOT_OPEN = 6,     /* 塔燈裝置未開啟 */
        TL_ERROR_WRITE_FAILED = 7,     /* 寫入操作失敗 */
        TL_ERROR_READ_FAILED = 8,     /* 讀取操作失敗 */
        TL_ERROR_TIMEOUT = 9,     /* 操作逾時 */
        TL_ERROR_INVALID_PARAMETER = 10,    /* 無效的參數 */
        TL_ERROR_MEMORY_ALLOCATION = 11,    /* 記憶體配置錯誤 */
        TL_ERROR_RESPONSE_FORMAT = 12,    /* 回應格式錯誤 */
        TL_ERROR_RESPONSE_CHECKSUM = 13,    /* 回應校驗和錯誤 */
        TL_ERROR_RESPONSE_NACK = 14,    /* 裝置拒絕命令 */
        TL_ERROR_OUT_OF_RANGE = 15     /* 參數超出範圍 */
    } TL_ERROR_CODE;

    /* LED 狀態定義 */
    typedef enum {
        TL_LED_OFF = 0,    /* LED 關閉 */
        TL_LED_ON = 1,    /* LED 開啟 */
        TL_LED_DUTY = 2     /* LED DUTY模式 */
    } TL_LED_STATE;

    /* LED 閃爍模式定義 */
    typedef enum {
        TL_LED_PATTERN_OFF = 0,   /* LED 關閉 */
        TL_LED_PATTERN_ON = 1,   /* LED 開啟 */
        TL_LED_PATTERN_BLINK1 = 2,   /* LED 閃爍模式1 */
        TL_LED_PATTERN_BLINK2 = 3    /* LED 閃爍模式2 */
    } TL_LED_PATTERN;

    /* 塔燈層級定義 */
    typedef enum {
        TL_LAYER_ONE = 0,   /* 第一層 */
        TL_LAYER_TWO = 1,   /* 第二層 */
        TL_LAYER_THREE = 2    /* 第三層 */
    } TL_LAYER;

    /* 蜂鳴器音調定義 */
    typedef enum {
        TL_BUZZER_TONE_HIGH = 0,   /* 高音調 */
        TL_BUZZER_TONE_LOW = 1    /* 低音調 */
    } TL_BUZZER_TONE;

    /* 蜂鳴器音量定義 */
    typedef enum {
        TL_BUZZER_VOLUME_BIG = 0,   /* 大音量 */
        TL_BUZZER_VOLUME_MEDIUM = 1,   /* 中音量 */
        TL_BUZZER_VOLUME_SMALL = 2    /* 小音量 */
    } TL_BUZZER_VOLUME;

    /* 蜂鳴器模式定義 */
    typedef enum {
        TL_BUZZER_PATTERN_OFF = 0,   /* 蜂鳴器關閉 */
        TL_BUZZER_PATTERN_1 = 1,   /* 蜂鳴器模式1 */
        TL_BUZZER_PATTERN_2 = 2,   /* 蜂鳴器模式2 */
        TL_BUZZER_PATTERN_3 = 3,   /* 蜂鳴器模式3 */
        TL_BUZZER_PATTERN_4 = 4    /* 蜂鳴器模式4 */
    } TL_BUZZER_PATTERN;

    /* LED 狀態結構 */
    typedef struct {
        TL_LED_STATE red_status;     /* 紅色LED狀態 */
        TL_LED_STATE green_status;   /* 綠色LED狀態 */
        TL_LED_STATE blue_status;    /* 藍色LED狀態 */
        TL_LED_PATTERN pattern;      /* 閃爍模式 */
    } TL_LEDStatus;

    /* 蜂鳴器狀態結構 */
    typedef struct {
        TL_BUZZER_TONE tone;         /* 音調 */
        TL_BUZZER_VOLUME volume;     /* 音量 */
        TL_BUZZER_PATTERN pattern;   /* 模式 */
    } TL_BuzzerStatus;

    /**
     * 初始化塔燈函式庫
     *
     * 初始化函式庫的內部資源，必須在使用任何其他函式之前呼叫。
     *
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_Initialize(void);

    /**
     * 釋放塔燈函式庫資源
     *
     * 釋放函式庫分配的所有資源，應用程式結束前調用。
     *
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_Finalize(void);

    /**
     * 開啟塔燈連接
     *
     * 嘗試開啟與塔燈裝置的連接。
     *
     * @param clear_state 連接後是否清除塔燈狀態 (TL_TRUE 或 TL_FALSE)
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_OpenConnection(TL_BOOL clear_state);

    /**
     * 關閉塔燈連接
     *
     * 關閉與塔燈裝置的連接。
     *
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_CloseConnection(void);

    /**
     * 檢查塔燈連接狀態
     *
     * 檢查塔燈是否已連接。
     *
     * @return TL_TRUE 表示已連接，TL_FALSE 表示未連接
     */
    TL_API TL_BOOL TL_IsConnected(void);

    /**
     * 設定特定層LED的狀態
     *
     * 設定塔燈特定層的LED狀態。
     *
     * @param layer 要設定的層級 (TL_LAYER_ONE, TL_LAYER_TWO, TL_LAYER_THREE)
     * @param status LED狀態結構，包含紅綠藍LED及閃爍模式設定
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_SetLED(TL_LAYER layer, const TL_LEDStatus* status);

    /**
     * 取得特定層LED的狀態
     *
     * 讀取塔燈特定層的LED當前狀態。
     *
     * @param layer 要讀取的層級 (TL_LAYER_ONE, TL_LAYER_TWO, TL_LAYER_THREE)
     * @param status 用於儲存LED狀態的結構指標
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_GetLEDStatus(TL_LAYER layer, TL_LEDStatus* status);

    /**
     * 清除所有LED
     *
     * 關閉塔燈所有層級的所有LED。
     *
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_ClearAllLEDs(void);

    /**
     * 設定蜂鳴器狀態
     *
     * 設定塔燈蜂鳴器的狀態。
     *
     * @param status 蜂鳴器狀態結構，包含音調、音量和模式設定
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_SetBuzzer(const TL_BuzzerStatus* status);

    /**
     * 取得蜂鳴器狀態
     *
     * 讀取塔燈蜂鳴器的當前狀態。
     *
     * @param status 用於儲存蜂鳴器狀態的結構指標
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_GetBuzzerStatus(TL_BuzzerStatus* status);

    /**
     * 停止蜂鳴器
     *
     * 關閉塔燈蜂鳴器。
     *
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_StopBuzzer(void);

    /**
     * 清除塔燈
     *
     * 關閉塔燈所有LED和蜂鳴器。
     *
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_ClearTowerLight(void);

    /**
     * 取得最後一次發生的錯誤碼
     *
     * 返回最後一次操作產生的錯誤碼。
     *
     * @return 錯誤碼
     */
    TL_API TL_ERROR_CODE TL_GetLastError(void);

    /**
     * 取得錯誤碼對應的錯誤訊息
     *
     * 返回指定錯誤碼的說明文字。
     *
     * @param error_code 要查詢的錯誤碼
     * @param buffer 用於儲存錯誤訊息的緩衝區
     * @param buffer_size 緩衝區大小
     * @return TL_SUCCESS 表示成功，其他值表示錯誤碼
     */
    TL_API TL_ERROR_CODE TL_GetErrorMessage(TL_ERROR_CODE error_code, char* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* TL_TOWER_LIGHT_H */


/*
 * tl_messages.h
 *
 * 塔燈通訊控制函式庫 - 訊息資源定義
 *
 * 本檔案定義了函式庫使用的文字訊息ID，以及獲取訊息的函數。
 * 使用者可以通過自訂訊息文件來支援不同語言。
 *
 * 版本: 1.0.0
 * 日期: 2025-03-25
 */

#ifndef TL_MESSAGES_H
#define TL_MESSAGES_H

 /* 訊息ID定義 - 與錯誤碼一一對應 */
enum {
    TL_MSG_ID_SUCCESS = 0,
    TL_MSG_ID_GENERAL_ERROR,
    TL_MSG_ID_NOT_INITIALIZED,
    TL_MSG_ID_ALREADY_INITIALIZED,
    TL_MSG_ID_DEVICE_NOT_FOUND,
    TL_MSG_ID_DEVICE_OPEN_FAILED,
    TL_MSG_ID_DEVICE_NOT_OPEN,
    TL_MSG_ID_WRITE_FAILED,
    TL_MSG_ID_READ_FAILED,
    TL_MSG_ID_TIMEOUT,
    TL_MSG_ID_INVALID_PARAMETER,
    TL_MSG_ID_MEMORY_ALLOCATION,
    TL_MSG_ID_RESPONSE_FORMAT,
    TL_MSG_ID_RESPONSE_CHECKSUM,
    TL_MSG_ID_RESPONSE_NACK,
    TL_MSG_ID_OUT_OF_RANGE,
    TL_MSG_ID_UNKNOWN_ERROR,

    /* 最後一個ID，用於確定訊息數量 */
    TL_MSG_ID_COUNT
};

/* 初始化訊息系統 */
void tl_messages_init(void);

/* 載入自訂訊息文件 */
int tl_messages_load_from_file(const char* filename);

/* 獲取指定ID的訊息 */
const char* tl_messages_get(int msg_id);

#endif /* TL_MESSAGES_H */

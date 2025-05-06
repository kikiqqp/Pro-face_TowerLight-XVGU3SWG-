/*
 * tl_messages.c
 *
 * 塔燈通訊控制函式庫 - 訊息資源實現
 *
 * 本檔案實現了函式庫使用的文字訊息資源管理。
 *
 * 版本: 1.0.0
 * 日期: 2025-03-25
 */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tl_messages.h"

 /* 預設的繁體中文訊息 - 單一來源 */
static const char* g_default_messages[TL_MSG_ID_COUNT] = {
    "操作成功",                     /* TL_MSG_ID_SUCCESS */
    "一般錯誤",                     /* TL_MSG_ID_GENERAL_ERROR */
    "函式庫未初始化",               /* TL_MSG_ID_NOT_INITIALIZED */
    "函式庫已初始化",               /* TL_MSG_ID_ALREADY_INITIALIZED */
    "找不到塔燈裝置",               /* TL_MSG_ID_DEVICE_NOT_FOUND */
    "無法開啟塔燈裝置",             /* TL_MSG_ID_DEVICE_OPEN_FAILED */
    "塔燈裝置未開啟",               /* TL_MSG_ID_DEVICE_NOT_OPEN */
    "寫入操作失敗",                 /* TL_MSG_ID_WRITE_FAILED */
    "讀取操作失敗",                 /* TL_MSG_ID_READ_FAILED */
    "操作逾時",                     /* TL_MSG_ID_TIMEOUT */
    "無效的參數",                   /* TL_MSG_ID_INVALID_PARAMETER */
    "記憶體配置錯誤",               /* TL_MSG_ID_MEMORY_ALLOCATION */
    "回應格式錯誤",                 /* TL_MSG_ID_RESPONSE_FORMAT */
    "回應校驗和錯誤",               /* TL_MSG_ID_RESPONSE_CHECKSUM */
    "裝置拒絕命令",                 /* TL_MSG_ID_RESPONSE_NACK */
    "參數超出範圍",                 /* TL_MSG_ID_OUT_OF_RANGE */
    "未知錯誤"                      /* TL_MSG_ID_UNKNOWN_ERROR */
};

/* 目前使用的訊息 */
static const char* g_active_messages[TL_MSG_ID_COUNT];

/* 訊息系統是否已初始化 */
static int g_is_initialized = 0;

/*
 * 初始化訊息系統
 */
void tl_messages_init(void) {
    int i;

    /* 複製預設訊息 */
    for (i = 0; i < TL_MSG_ID_COUNT; i++) {
        g_active_messages[i] = g_default_messages[i];
    }

    g_is_initialized = 1;
}

/*
 * 載入自訂訊息文件
 *
 * 文件格式為純文本，每行一個訊息，按順序對應訊息ID
 *
 * 返回值：成功載入的訊息數量，如果為0表示失敗
 */
int tl_messages_load_from_file(const char* filename) {
    FILE* file;
    char buffer[256];
    char** new_messages;
    int count = 0;
    int i;

    /* 確保訊息系統已初始化 */
    if (!g_is_initialized) {
        tl_messages_init();
    }

    /* 參數檢查 */
    if (filename == NULL) {
        return 0;
    }

    /* 開啟文件 */
    file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }

    /* 分配臨時訊息陣列 */
    new_messages = (char**)malloc(sizeof(char*) * TL_MSG_ID_COUNT);
    if (new_messages == NULL) {
        fclose(file);
        return 0;
    }

    /* 初始化為NULL */
    memset(new_messages, 0, sizeof(char*) * TL_MSG_ID_COUNT);

    /* 讀取每一行 */
    while (fgets(buffer, sizeof(buffer), file) && count < TL_MSG_ID_COUNT) {
        int len;

        /* 移除行尾的換行符 */
        len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';

            /* 處理可能的CR+LF */
            if (len > 1 && buffer[len - 2] == '\r') {
                buffer[len - 2] = '\0';
            }
        }

        /* 分配並複製字符串 */
        new_messages[count] = (char*)malloc(strlen(buffer) + 1);
        if (new_messages[count] != NULL) {
            strcpy(new_messages[count], buffer);
            count++;
        }
    }

    /* 關閉文件 */
    fclose(file);

    /* 如果讀取了足夠的訊息，則更新全域訊息陣列 */
    if (count == TL_MSG_ID_COUNT) {
        for (i = 0; i < TL_MSG_ID_COUNT; i++) {
            g_active_messages[i] = new_messages[i];
        }
        return count;
    }

    /* 清理臨時分配的內存 */
    for (i = 0; i < count; i++) {
        if (new_messages[i] != NULL) {
            free(new_messages[i]);
        }
    }
    free(new_messages);

    return 0;
}

/*
 * 獲取訊息文本
 */
const char* tl_messages_get(int msg_id) {
    /* 確保訊息系統已初始化 */
    if (!g_is_initialized) {
        tl_messages_init();
    }

    /* 檢查ID是否有效 */
    if (msg_id >= 0 && msg_id < TL_MSG_ID_COUNT) {
        return g_active_messages[msg_id];
    }

    /* 無效ID返回未知錯誤訊息 */
    return g_active_messages[TL_MSG_ID_UNKNOWN_ERROR];
}


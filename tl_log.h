#ifndef TL_LOG_H
#define TL_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

/* 開啟這個宏即可啟用 logging，否則所有 LOG_* 都會被排除 */
#ifdef ENABLE_LOG


#ifdef __cplusplus
extern "C" {
#endif


/* 日誌等級定義 */
typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

/* 可設定當前最低輸出等級，低於者不輸出 */
extern LogLevel g_log_level;

/* 核心輸出函式 */
void tl_log(LogLevel level, const char* fmt, ...);

/* 宏封裝，簡化呼叫 */
#define LOG_DEBUG(...)  tl_log(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)   tl_log(LOG_LEVEL_INFO,  __VA_ARGS__)
#define LOG_WARN(...)   tl_log(LOG_LEVEL_WARN,  __VA_ARGS__)
#define LOG_ERROR(...)  tl_log(LOG_LEVEL_ERROR, __VA_ARGS__)

#else

/* 如果沒有啟用 ENABLE_LOG，則 LOG_* 為空操作 */
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)

#endif /* ENABLE_LOG */

#ifdef __cplusplus
}
#endif

#endif /* TL_LOG_H */

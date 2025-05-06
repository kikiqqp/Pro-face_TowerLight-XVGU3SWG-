#include "tl_log.h"

#ifdef ENABLE_LOG

/* 預設最低輸出為 DEBUG（全部顯示） */
LogLevel g_log_level = LOG_LEVEL_DEBUG;

/* 等級標籤 */
static const char* log_level_str[] = {
    "DEBUG", "INFO", "WARN", "ERROR"
};

void tl_log(LogLevel level, const char* fmt, ...)
{
    if (level < g_log_level)
        return;

    /* 顯示時間 */
    time_t t;
    char time_str[20];
    t = time(NULL);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&t));

    fprintf(stderr, "[%s] [%s] ", time_str, log_level_str[level]);

    /* 可變參數格式化輸出 */
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    fflush(stderr);
}

#endif

#include <stdio.h>
#include "tl_tower_light.h"
#include "tl_internal.h"

#ifdef BUILD_TEST_EXE 

int main(void)
{
    TL_ERROR_CODE ret;

    /* 測試流程：初始化 -> 開啟連線 -> 清除LED -> 設定LED -> 等待 -> 結束 */

    printf("--------------- 開啟設備連線 ---------------\n");

    ret = TL_Initialize();
    if (ret != TL_SUCCESS) 
    {
        printf("TL_Initialize失敗, err=%d\n", ret);
        return 1;
    }

    ret = TL_OpenConnection(TL_FALSE);
    if (ret != TL_SUCCESS) 
    {
        printf("TL_OpenConnection失敗, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    printf("\n--------------- 清除 LED ---------------\n");
    /* 先清除所有LED */
    ret = TL_ClearAllLEDs();
    if (ret != TL_SUCCESS) 
    {
        printf("TL_ClearAllLEDs失敗, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    printf("\n--------------- 設定測試 LED ---------------\n");
    /* 再依序測試設定：第一層紅、第二層藍、第三層綠 */
    TL_LEDStatus status;
    /* 第1層紅 */
    tl_delay_ms(1000);
    printf("紅\n");
    status.red_status = TL_LED_ON;
    status.green_status = TL_LED_OFF;
    status.blue_status = TL_LED_OFF;
    status.pattern = TL_LED_PATTERN_ON;
    ret = TL_SetLED(TL_LAYER_ONE, &status);
    if (ret != TL_SUCCESS) 
    {
        printf("Set LED1失敗, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    /* 第2層藍 */
    tl_delay_ms(1000);
    printf("藍\n");
    status.red_status = TL_LED_OFF;
    status.green_status = TL_LED_OFF;
    status.blue_status = TL_LED_ON;
    status.pattern = TL_LED_PATTERN_ON;
    ret = TL_SetLED(TL_LAYER_TWO, &status);
    if (ret != TL_SUCCESS) 
    {
        printf("Set LED2失敗, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    /* 第3層綠 */
    tl_delay_ms(1000);
    printf("綠\n");
    status.red_status = TL_LED_OFF;
    status.green_status = TL_LED_ON;
    status.blue_status = TL_LED_OFF;
    status.pattern = TL_LED_PATTERN_ON;
    ret = TL_SetLED(TL_LAYER_THREE, &status);
    if (ret != TL_SUCCESS) 
    {
        printf("Set LED3失敗, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    printf("\n--------------- 測試完成關閉設備 ---------------\n");
    TL_Finalize();

    printf("\n\n全部測試 OK，按Enter結束...\n");

    return 0;
}

#endif
#include <stdio.h>
#include "tl_tower_light.h"
#include "tl_internal.h"

#ifdef BUILD_TEST_EXE 

int main(void)
{
    TL_ERROR_CODE ret;

    /* ���լy�{�G��l�� -> �}�ҳs�u -> �M��LED -> �]�wLED -> ���� -> ���� */

    printf("--------------- �}�ҳ]�Ƴs�u ---------------\n");

    ret = TL_Initialize();
    if (ret != TL_SUCCESS) 
    {
        printf("TL_Initialize����, err=%d\n", ret);
        return 1;
    }

    ret = TL_OpenConnection(TL_FALSE);
    if (ret != TL_SUCCESS) 
    {
        printf("TL_OpenConnection����, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    printf("\n--------------- �M�� LED ---------------\n");
    /* ���M���Ҧ�LED */
    ret = TL_ClearAllLEDs();
    if (ret != TL_SUCCESS) 
    {
        printf("TL_ClearAllLEDs����, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    printf("\n--------------- �]�w���� LED ---------------\n");
    /* �A�̧Ǵ��ճ]�w�G�Ĥ@�h���B�ĤG�h�šB�ĤT�h�� */
    TL_LEDStatus status;
    /* ��1�h�� */
    tl_delay_ms(1000);
    printf("��\n");
    status.red_status = TL_LED_ON;
    status.green_status = TL_LED_OFF;
    status.blue_status = TL_LED_OFF;
    status.pattern = TL_LED_PATTERN_ON;
    ret = TL_SetLED(TL_LAYER_ONE, &status);
    if (ret != TL_SUCCESS) 
    {
        printf("Set LED1����, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    /* ��2�h�� */
    tl_delay_ms(1000);
    printf("��\n");
    status.red_status = TL_LED_OFF;
    status.green_status = TL_LED_OFF;
    status.blue_status = TL_LED_ON;
    status.pattern = TL_LED_PATTERN_ON;
    ret = TL_SetLED(TL_LAYER_TWO, &status);
    if (ret != TL_SUCCESS) 
    {
        printf("Set LED2����, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    /* ��3�h�� */
    tl_delay_ms(1000);
    printf("��\n");
    status.red_status = TL_LED_OFF;
    status.green_status = TL_LED_ON;
    status.blue_status = TL_LED_OFF;
    status.pattern = TL_LED_PATTERN_ON;
    ret = TL_SetLED(TL_LAYER_THREE, &status);
    if (ret != TL_SUCCESS) 
    {
        printf("Set LED3����, err=%d\n", ret);
        TL_Finalize();
        return 1;
    }

    printf("\n--------------- ���է��������]�� ---------------\n");
    TL_Finalize();

    printf("\n\n�������� OK�A��Enter����...\n");

    return 0;
}

#endif
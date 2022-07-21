#include "buzzer.h"

//��������ʼ��
void Buzzer_Init(void)
{
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
}


//�����������Լ�
void Buzzer_BeforeWork_Check(void)
{
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,90);
    HAL_Delay(100);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_1,0);
    HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);    
}

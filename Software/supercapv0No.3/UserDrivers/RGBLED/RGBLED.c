#include "RGBLED.h"

void RGB_LED_Init(void)
{
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
}

void RGB_B_Light(void)
{
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,500);
}

void RGB_R_Light(void)
{
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_4,500); 
}

void RGB_G_Light(void)
{
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,500);    
}

void RGB_ALL_Unlight(void)
{
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,0);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,0);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_4,0);      
}

//RGBLED¿ª»ú×Ô¼ì
void RGB_LED_BeforeWork_Check(void)
{
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,500);
    HAL_Delay(500);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,0);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_4,500);
    HAL_Delay(500);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_4,0);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,100);
    HAL_Delay(500);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,0);    
}



/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BuckboostControl.h"
#include "RGBLED.h"
#include "buzzer.h"
#include "ADCRead.h"
#include "can_receive.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t ADC1_Read_DMA[2];
uint32_t ADC2_Read_DMA[3];
float ADC_DATA[5];



void sendware(void *wareaddr, uint32_t waresize)
{
	#define CMD_WARE     3
	uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};    
	uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};   
	HAL_UART_Transmit(&huart1, (uint8_t *)cmdf, sizeof(cmdf), 5000);
	HAL_UART_Transmit(&huart1, (uint8_t *)wareaddr, waresize ,5000);
	HAL_UART_Transmit(&huart1, (uint8_t *)cmdr, sizeof(cmdr), 5000);
}

void senddebug(void)
{
    float debug_data[8];
    debug_data[0] = buckboost_control.BUCKBOOST_CAP_V;
    
    debug_data[1] = buckboost_control.Robot_Power_PID.set;
    debug_data[2] = buckboost_control.Robot_Power_PID.fdb;
    
    debug_data[3] = buckboost_control.ratio_pass;
    debug_data[4] = buckboost_control.BuckBoostLoopCcompare_PID.out;
    
    debug_data[5] = buckboost_control.BuckBoostLoopCcompare_PID.set;
    debug_data[6] = buckboost_control.BuckBoostLoopCcompare_PID.fdb;
    
    debug_data[7] = buckboost_control.charge_power;
    sendware(debug_data,sizeof(debug_data));
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
  HAL_TIM_Base_Start(&htim1);
  Buzzer_Init();
  RGB_LED_Init();
  CAN1_Init();
  //开机蜂鸣器自检
  Buzzer_BeforeWork_Check();
  //开机RGB灯自检
  RGB_LED_BeforeWork_Check();
  //开启ADC
  HAL_ADC_Start(&hadc1);
  HAL_Delay(5);
  HAL_ADC_Start(&hadc2);
  HAL_Delay(5);
  //ADC数据结构体初始化
  ELEC_INFO_INIT();
  ELEC_INFO_ADC_Read_Init();
  HAL_Delay(200);
  ELEC_INFO_ADC_Read();
  ADCDATA_To_RealDATA();

  BuckBoostInit();
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint16_t loop_cnt = 0; 
  BuckBoostInit();
  HAL_Delay(1000);
  for(uint8_t i=0; i<10;i++)
  {
      ELEC_INFO_ADC_Read();
      ADCDATA_To_RealDATA();
      HAL_ADC_Stop_DMA(&hadc1);
      HAL_ADC_Stop_DMA(&hadc2);
      HAL_Delay(100);
  }
  
  if(out_c.real_valu1 > 1.6f)
  {
      out_c.a = 13.34;
      out_c.b = -20.868;
  }  

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      
      //HAL_Delay(500);
      
      // buckboost_control.buckboostmode = 2;
      // BuckBoost_OUTControl(950);
      
      ELEC_INFO_ADC_Read();
      ADCDATA_To_RealDATA();

      BuckBoost_FIFO_Feedback();
      BuckBoost_Set_Control();
      BuckBoost_PID_Calculate();
      BuckBoost_OUT();
      
//      printf("VIN  is %.4f V\r\n",in_v.real_valu1);
//      printf("CIN  is %.4f A\r\n",in_c.real_valu1);
//      printf("COUT is %.4f A\r\n",out_c.real_valu1);
//      printf("CCAP is %.2f V\r\n",cap_c.real_valu1);
//      printf("VCAP is %.4f V\r\n",cap_v.real_valu1);
//      printf("/*********************/\r\n");
      
      HAL_ADC_Stop_DMA(&hadc1);
      HAL_ADC_Stop_DMA(&hadc2);

      loop_cnt ++;
      
      if(loop_cnt >= 100)
      {
          HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
          loop_cnt = 0;
      }
      supercap_sendmessage(buckboost_control.BUCKBOOST_CAP_V * 1000);
      senddebug();
      HAL_Delay(1);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

typedef struct __FILE FILE;
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


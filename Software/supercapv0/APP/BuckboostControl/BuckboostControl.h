#ifndef _BUCKBOOSTCONTROL_H
#define _BUCKBOOSTCONTROL_H


#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "ADCRead.h"
#include "pid.h"
#include "supercap_config.h"
#include "filter.h"





/****************************
24V                 CAP
Q1------|     |------Q3
        |-----|
        |  L  |
        |-----|
Q2------|     |------Q4
*****************************/
#define Q1_PWMControl_TIM     htim1
#define Q1_PWMControl_TIM_CH  TIM_CHANNEL_1
#define Q2_PWMControl_TIM     htim1
#define Q2_PWMControl_TIM_CH  TIM_CHANNEL_2
#define Q3_PWMControl_TIM     htim1
#define Q3_PWMControl_TIM_CH  TIM_CHANNEL_3
#define Q4_PWMControl_TIM     htim1
#define Q4_PWMControl_TIM_CH  TIM_CHANNEL_4

typedef struct
{
  float U_now;
  float I_now;
  float P_now;
} CONTROL_STRUCT;

//电流环PID
#define BUCKBOOSTLOOPC_PID_MODE          PID_DELTA
#define BUCKBOOSTLOOPC_PID_MAX_OUT       50
#define BUCKBOOSTLOOPC_PID_MAX_IOUT      15
#define BUCKBOOSTLOOPC_PID_KP            0.6f  
#define BUCKBOOSTLOOPC_PID_KI            0.03f      
#define BUCKBOOSTLOOPC_PID_KD            0.0f 

//电压环PID
#define BUCKBOOSTLOOPV_PID_MODE          PID_DELTA
#define BUCKBOOSTLOOPV_PID_MAX_OUT       50
#define BUCKBOOSTLOOPV_PID_MAX_IOUT      15
#define BUCKBOOSTLOOPV_PID_KP            1.0f   
#define BUCKBOOSTLOOPV_PID_KI            0.06f      
#define BUCKBOOSTLOOPV_PID_KD            0.0f 

//缓冲能量环PID
#define Robot_Buffer_PID_MODE          PID_DELTA
#define Robot_Buffer_PID_MAX_OUT       5
#define Robot_Buffer_PID_MAX_IOUT      2
#define Robot_Buffer_PID_KP            0.7f     
#define Robot_Buffer_PID_KI            0.1f      
#define Robot_Buffer_PID_KD            0.0f

//功率环PID
#define Robot_Power_PID_MODE          PID_DELTA
#define Robot_Power_PID_MAX_OUT       400
#define Robot_Power_PID_MAX_IOUT      50
#define Robot_Power_PID_KP            0.8f     
#define Robot_Power_PID_KI            0.1f      
#define Robot_Power_PID_KD            0.0f


typedef struct
{
  uint8_t data_update;    //1更新
  uint8_t buckboostmode;  //0为初始关闭模式 1为降压模式 2为升压模式
  uint8_t loop_mode;      //环路模式0：cv 1：cc 2：cp
  float efficiency;       //总效率 ，热耗散
  uint8_t power_dir;      //能量方向0正向充电1反向放电
  uint16_t max_power_lim; //裁判系统最大功率
  float power_pass;       //  裁判系统花去功率板与板子测量误差经过pid结果用于传递
  uint8_t referee_power;
  uint8_t referee_buff;
  float powerbuffer ;     //缓冲能量
    
  PID_Regulator_t BuckBoostLoopCcompare_PID;//输出电流环
  PID_Regulator_t BuckBoostLoopVcompare_PID;//输出电压环
  PID_Regulator_t Robot_Buffer_PID;//缓冲能量环
  PID_Regulator_t Robot_Power_PID;//功率环
    
  
  double BUCKBOOST_CAP_C;
  double BUCKBOOST_CAP_V;
  double BUCKBOOST_IN_C;
  double BUCKBOOST_IN_V;
  double BUCKBOOST_OUT_C;
    
  double BUCKBOOST_NEED_C;
        
  float BUCKBOOST_IN_P;
  float BUCKBOOST_OUT_P;
  float BUCKBOOST_CAP_P;
  float BUCKBOOST_NEED_P;
  
  float charge_power;//功率环计算OUT
  float charge_current;// = charge_power/inV
  float cloop;
  float vloop;
  float ratio_pass;//计算升降压比例

  uint8_t POWER_LIMIT_MODE;//超级电容功率限制模式 0使用电容 1不使用电容
    
} BUCKBOOST_STRUCT;




extern BUCKBOOST_STRUCT buckboost_control;

void BuckBoostInit(void);
void BuckBoost_FIFO_Feedback(void);
void BuckBoost_Set_Control(void);
void BuckBoost_PID_Calculate(void);
void BuckBoost_OUT(void);
void BuckBoost_OUTControl(float ratio);
#endif

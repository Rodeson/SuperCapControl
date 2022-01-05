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

//������PID
#define BUCKBOOSTLOOPC_PID_MODE          PID_DELTA
#define BUCKBOOSTLOOPC_PID_MAX_OUT       50
#define BUCKBOOSTLOOPC_PID_MAX_IOUT      15
#define BUCKBOOSTLOOPC_PID_KP            0.6f  
#define BUCKBOOSTLOOPC_PID_KI            0.03f      
#define BUCKBOOSTLOOPC_PID_KD            0.0f 

//��ѹ��PID
#define BUCKBOOSTLOOPV_PID_MODE          PID_DELTA
#define BUCKBOOSTLOOPV_PID_MAX_OUT       50
#define BUCKBOOSTLOOPV_PID_MAX_IOUT      15
#define BUCKBOOSTLOOPV_PID_KP            1.0f   
#define BUCKBOOSTLOOPV_PID_KI            0.06f      
#define BUCKBOOSTLOOPV_PID_KD            0.0f 

//����������PID
#define Robot_Buffer_PID_MODE          PID_DELTA
#define Robot_Buffer_PID_MAX_OUT       5
#define Robot_Buffer_PID_MAX_IOUT      2
#define Robot_Buffer_PID_KP            0.7f     
#define Robot_Buffer_PID_KI            0.1f      
#define Robot_Buffer_PID_KD            0.0f

//���ʻ�PID
#define Robot_Power_PID_MODE          PID_DELTA
#define Robot_Power_PID_MAX_OUT       400
#define Robot_Power_PID_MAX_IOUT      50
#define Robot_Power_PID_KP            0.8f     
#define Robot_Power_PID_KI            0.1f      
#define Robot_Power_PID_KD            0.0f


typedef struct
{
  uint8_t data_update;    //1����
  uint8_t buckboostmode;  //0Ϊ��ʼ�ر�ģʽ 1Ϊ��ѹģʽ 2Ϊ��ѹģʽ
  uint8_t loop_mode;      //��·ģʽ0��cv 1��cc 2��cp
  float efficiency;       //��Ч�� ���Ⱥ�ɢ
  uint8_t power_dir;      //��������0������1����ŵ�
  uint16_t max_power_lim; //����ϵͳ�����
  float power_pass;       //  ����ϵͳ��ȥ���ʰ�����Ӳ�������pid������ڴ���
  uint8_t referee_power;
  uint8_t referee_buff;
  float powerbuffer ;     //��������
    
  PID_Regulator_t BuckBoostLoopCcompare_PID;//���������
  PID_Regulator_t BuckBoostLoopVcompare_PID;//�����ѹ��
  PID_Regulator_t Robot_Buffer_PID;//����������
  PID_Regulator_t Robot_Power_PID;//���ʻ�
    
  
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
  
  float charge_power;//���ʻ�����OUT
  float charge_current;// = charge_power/inV
  float cloop;
  float vloop;
  float ratio_pass;//��������ѹ����

  uint8_t POWER_LIMIT_MODE;//�������ݹ�������ģʽ 0ʹ�õ��� 1��ʹ�õ���
    
} BUCKBOOST_STRUCT;




extern BUCKBOOST_STRUCT buckboost_control;

void BuckBoostInit(void);
void BuckBoost_FIFO_Feedback(void);
void BuckBoost_Set_Control(void);
void BuckBoost_PID_Calculate(void);
void BuckBoost_OUT(void);
void BuckBoost_OUTControl(float ratio);
#endif

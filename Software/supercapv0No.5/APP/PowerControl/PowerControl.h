#ifndef _POWERCONTROL_H
#define _POWERCONTROL_H

#include "main.h"
#include "ADCRead.h"


typedef struct
{
  float U_now;
  float I_now;
  float P_now;
} CONTROL_STRUCT;

volatile typedef struct
{
  uint8_t data_update;          //1更新
  uint8_t loop_mode;            //环路模式0：cv 1：cc 2：cp
  float efficiency;             //总效率 ，热耗散
  uint8_t power_dir;            //能量方向0正向充电1反向放电
  uint16_t max_power_lim;       //裁判系统最大功率
  float power_pass;             //  裁判系统花去功率板与板子测量误差经过pid结果用于传递
  float powerbuffer ;           //缓冲能量
} BUCKBOOST_STRUCT;





#endif

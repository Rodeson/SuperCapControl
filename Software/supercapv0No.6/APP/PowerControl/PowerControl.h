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
  uint8_t data_update;          //1����
  uint8_t loop_mode;            //��·ģʽ0��cv 1��cc 2��cp
  float efficiency;             //��Ч�� ���Ⱥ�ɢ
  uint8_t power_dir;            //��������0������1����ŵ�
  uint16_t max_power_lim;       //����ϵͳ�����
  float power_pass;             //  ����ϵͳ��ȥ���ʰ�����Ӳ�������pid������ڴ���
  float powerbuffer ;           //��������
} BUCKBOOST_STRUCT;





#endif

#ifndef _ADCREAD_H
#define _ADCREAD_H

#include "main.h"
#include "adc.h"
#include "filter.h"


typedef struct
{
  uint8_t elec_type;         //0��ѹ1����
  
  //������������
  double real_valu1;         //��ʵֵ1  �ⲿ�Ǳ���
  double get_volt1;          //����ֵ1  ��Ƭ���˲�����
  double real_valu2;         //��ʵֵ2
  double get_volt2;          //����ֵ2
  double real_valu3;         //��ʵֵ3
  double get_volt3;          //����ֵ3
  
  //����һ��������Ϸ��� y=ax+b
  float a;              //y=ax+b
  float b;              //y=ax+b
    
  uint16_t valu_result;   //�ȶ���ԭʼ����

} ELEC_INFO_STRUCT;

extern uint32_t ADC1_Read_DMA_INFO[2];
extern uint32_t ADC2_Read_DMA_INFO[3];
extern float ELEC_INFO_ADC_DATA[5];

extern ELEC_INFO_STRUCT cap_c;        //���ݵ�������
extern ELEC_INFO_STRUCT cap_v;        //���ݵ�ѹ����
extern ELEC_INFO_STRUCT in_c;         //�����������
extern ELEC_INFO_STRUCT in_v;         //�����ѹ����
extern ELEC_INFO_STRUCT out_c;        //���̵�������


void ELEC_INFO_INIT(void);
void ELEC_INFO_ADC_Read_Init(void);
void ELEC_INFO_ADC_Read(void);
void ADCDATA_To_RealDATA(void);

#endif

#ifndef _ADCREAD_H
#define _ADCREAD_H

#include "main.h"
#include "adc.h"
#include "filter.h"


typedef struct
{
  uint8_t elec_type;         //0电压1电流
  
  //测量三组数据
  double real_valu1;         //真实值1  外部仪表获得
  double get_volt1;          //测量值1  单片机滤波后获得
  double real_valu2;         //真实值2
  double get_volt2;          //测量值2
  double real_valu3;         //真实值3
  double get_volt3;          //测量值3
  
  //构建一阶线性拟合方程 y=ax+b
  float a;              //y=ax+b
  float b;              //y=ax+b
    
  uint16_t valu_result;   //稳定的原始数据

} ELEC_INFO_STRUCT;

extern uint32_t ADC1_Read_DMA_INFO[2];
extern uint32_t ADC2_Read_DMA_INFO[3];
extern float ELEC_INFO_ADC_DATA[5];

extern ELEC_INFO_STRUCT cap_c;        //电容电流参数
extern ELEC_INFO_STRUCT cap_v;        //电容电压参数
extern ELEC_INFO_STRUCT in_c;         //输入电流参数
extern ELEC_INFO_STRUCT in_v;         //输入电压参数
extern ELEC_INFO_STRUCT out_c;        //底盘电流参数


void ELEC_INFO_INIT(void);
void ELEC_INFO_ADC_Read_Init(void);
void ELEC_INFO_ADC_Read(void);
void ADCDATA_To_RealDATA(void);

#endif

#include "ADCRead.h"

ELEC_INFO_STRUCT cap_c;        //电容电流参数
ELEC_INFO_STRUCT cap_v;        //电容电压参数
ELEC_INFO_STRUCT in_c;         //输入电流参数
ELEC_INFO_STRUCT in_v;         //输入电压参数
ELEC_INFO_STRUCT out_c;        //底盘电流参数

uint32_t ADC1_Read_DMA_INFO[2];
uint32_t ADC2_Read_DMA_INFO[3];

float ELEC_INFO_ADC_DATA[5];

//将EXCEL拟合的线性参数放入结构体内
void ELEC_INFO_INIT(void)
{
    //CAP_C R^2 = 0.9942 
    cap_c.a          =  2.5215;
    cap_c.b          = -4.1546;
    cap_c.real_valu1 = 0;
    cap_c.get_volt1  = 0;
    cap_c.real_valu2 = 0;
    cap_c.get_volt2  = 0;
    cap_c.real_valu3 = 0;
    cap_c.get_volt3  = 0;  
    cap_c.elec_type  = 1;
    //CAP_V R^2 = 1
    cap_v.a          = 20.696;
    cap_v.b          = -0.0737;
    cap_v.real_valu1 = 0;
    cap_v.get_volt1  = 0;
    cap_v.real_valu2 = 0;
    cap_v.get_volt2  = 0; 
    cap_v.real_valu3 = 0;
    cap_v.get_volt3  = 0;
    cap_v.elec_type  = 0;
    //IN_C  R^2 = 1
    in_c.a           =  2.4968;
    in_c.b           = -4.1225;
    in_c.real_valu1  = 0;
    in_c.get_volt1   = 0;
    in_c.real_valu2  = 0;
    in_c.get_volt2   = 0;
    in_c.real_valu3  = 0;
    in_c.get_volt3   = 0;
    in_c.elec_type   = 1;    
    //IN_V  R^2 = 0.9999
    in_v.a           = 20.289;
    in_v.b           = -0.2389;
    in_v.real_valu1  = 0;
    in_v.get_volt1   = 0;
    in_v.real_valu2  = 0;
    in_v.get_volt2   = 0;
    in_v.real_valu3  = 0;
    in_v.get_volt3   = 0;
    in_v.elec_type   = 0;
    //OUT_C R^2 = 0.9999
    out_c.a          =  3.863;
    out_c.b          = -6.4786;
    out_c.real_valu1 = 0;
    out_c.get_volt1  = 0;
    out_c.real_valu2 = 0;
    out_c.get_volt2  = 0;
    out_c.real_valu3 = 0;
    out_c.get_volt3  = 0;
    out_c.elec_type  = 1;
}

void ELEC_INFO_ADC_Read_Init(void)
{
      HAL_ADC_Start_DMA(&hadc1,ADC1_Read_DMA_INFO,2);
      HAL_Delay(5);
      HAL_ADC_Start_DMA(&hadc2,ADC2_Read_DMA_INFO,3);
      HAL_Delay(5);
}

void ELEC_INFO_ADC_Read(void)
{
      int i = 0;
      HAL_ADC_Start_DMA(&hadc1,ADC1_Read_DMA_INFO,2);
      HAL_ADC_Start_DMA(&hadc2,ADC2_Read_DMA_INFO,3); 
    
      ELEC_INFO_ADC_DATA[0] = (float)ADC1_Read_DMA_INFO[0];
      ELEC_INFO_ADC_DATA[1] = (float)ADC2_Read_DMA_INFO[0];
      ELEC_INFO_ADC_DATA[2] = (float)ADC2_Read_DMA_INFO[1];
      ELEC_INFO_ADC_DATA[3] = (float)ADC2_Read_DMA_INFO[2];
      ELEC_INFO_ADC_DATA[4] = (float)ADC1_Read_DMA_INFO[1];
    
      
      for(i=0; i<5; i++)
      {
          ELEC_INFO_ADC_DATA[i] = ELEC_INFO_ADC_DATA[i] * 3.3f /4096;
      }
      
          
      in_v.get_volt1  = ELEC_INFO_ADC_DATA[0];
      in_c.get_volt1  = ELEC_INFO_ADC_DATA[1];
      out_c.get_volt1 = ELEC_INFO_ADC_DATA[2];
      cap_c.get_volt1 = ELEC_INFO_ADC_DATA[3];
      cap_v.get_volt1 = ELEC_INFO_ADC_DATA[4];
      
}

//将ADC数据转变为实际数据
//根据拟合函数计算
void ADCDATA_To_RealDATA(void)
{
    in_v.real_valu1  = in_v.a  * in_v.get_volt1  + in_v.b;
    in_c.real_valu1  = in_c.a  * in_c.get_volt1  + in_c.b;
    out_c.real_valu1 = out_c.a * out_c.get_volt1 + out_c.b;
    cap_c.real_valu1 = cap_c.a * cap_c.get_volt1 + cap_c.b;
    cap_v.real_valu1 = cap_v.a * cap_v.get_volt1 + cap_v.b;
}


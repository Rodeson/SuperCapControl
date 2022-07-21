#ifndef __filter_H
#define	__filter_H
//#include "stm32g4xx_hal.h"
#include "stdint.h"
//#include "core_cm4.h"
//1.�޷��˲�
//u16 amplitudeLimiterFilter(void);

//2.��λֵ�˲�
uint16_t middleValueFilter(void);

//3.����ƽ���˲�
//uint16_t averageFilter(void);

//4.����ƽ���˲������ֳƻ���ƽ���˲�����
//uint16_t moveAverageFilter(void);

//5.��λֵƽ���˲������������ƽ���˲�����
//uint16_t middleAverageFilter(void);

//6.������λֵ�˲���
//uint16_t recursionMiddleFilter(void);

//7.�޷�ƽ���˲���
//uint16_t filter(void );

//8.һ���ͺ��˲���
//uint16_t firstOrderFilter(u16 newValue, u16 oldValue);

//9.��Ȩ����ƽ���˲�������û�е��ƣ�
//uint16_t weightAverageFilter(void)
	
//10.�����˲���
//uint16_t glitchFilter(u16 oldValue);

//11.���������˲���
//uint16_t Three_Standard_Deviation_filter(void);

//12.ȥ��ֵ����ƽ���˲���
//uint16_t abandon_extremum_averageFilter(void);

//���ADֵ
uint16_t getValue(void);
#define low_filter_default {0, 0, 0, 0, 0, 0}
typedef struct
{
	float  Input;
	float  Output;
	float  Fc;
	float  Fs;
	float  Ka;
	float  Kb;
	
}LOWPASS_FILTER_STRUCT;
#define PI    3.14159265F // ��
#define PI2   6.28318530F // 2��
extern LOWPASS_FILTER_STRUCT lpf[5];
extern void bubble_sort( uint16_t *a ,unsigned char Num);//ð�ݷ�����
extern uint32_t middle_filter(uint16_t *a ,unsigned char Num,char extremum);
extern void low_filter_calc(LOWPASS_FILTER_STRUCT *p);
extern void low_filter_init(LOWPASS_FILTER_STRUCT *p,float sample_f,float cutoff_f);
#endif


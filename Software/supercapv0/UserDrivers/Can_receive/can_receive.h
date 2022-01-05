#ifndef CAN_RECEIVE_H
#define CAN_RECEIVE_H

#include "can.h"
#include "BuckboostControl.h"

#define FULLECD      8192
#define ECDDVALE_MAX    FULLECD/2
#define ROLLINGDIRECTION     CLOCKWISE

enum eDIRECTION {ANTICLOCKWISE = +1,CLOCKWISE = -1};
/* CAN send and receive ID */
typedef enum
{
    USER_ID = 0X210,
} can_msg_id_e;

static uint8_t aData[8];
extern CAN_RxHeaderTypeDef  Rx1Message;

typedef struct
{
    uint16_t inputpower; //输入功率
    uint16_t capvolt;    //电容电压
    uint16_t powermax;   //最大电流
    uint16_t setpower;   //设定功率

    //0x00 初始化
    //0x01 电池模式 电池给电容充电 给电机输出
    //0x02 电池延迟模式
    //0x03 电容模式 电池电容一起给电机输出
    //0x04 电容延迟
    //0x05 超级电容断电模式
    uint16_t outputmode; //输出模式

}supercap_measure_t;

void CAN1_Init(void);
void CAN1_Getdata(CAN_RxHeaderTypeDef *pHeader,uint8_t aData[]);
void get_supercap_measure(BUCKBOOST_STRUCT *cap, uint8_t aData[]);
//返回超级电容变量地址，通过指针方式获取原始数据
extern const supercap_measure_t *get_SuperCap_Measure_Point(void);
extern void supercap_sendmessage(uint16_t Vcap);

#endif

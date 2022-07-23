#include "can_receive.h"
#include "string.h"
//声明超级电容变量
static supercap_measure_t supercap;

CAN_RxHeaderTypeDef  Rx1Message;
CAN_TxHeaderTypeDef  Tx1Message;

uint32_t pTxMailbox;

void CAN1_Init()						
{
	CAN_FilterTypeDef canfilter;

	canfilter.FilterMode = CAN_FILTERMODE_IDMASK;
	canfilter.FilterScale = CAN_FILTERSCALE_32BIT;
	
	//filtrate any ID you want here
	canfilter.FilterIdHigh = 0x0000;
	canfilter.FilterIdLow = 0x0000;
	canfilter.FilterMaskIdHigh = 0x0000;
	canfilter.FilterMaskIdLow = 0x0000;
  
	canfilter.FilterFIFOAssignment = CAN_FilterFIFO0;
	canfilter.FilterActivation = ENABLE;
	canfilter.SlaveStartFilterBank = 14;
	//use different filter for can1&can2
	canfilter.FilterBank=0;

	HAL_CAN_ConfigFilter(&hcan1,&canfilter);
	
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);

	HAL_CAN_Start(&hcan1);
}

const supercap_measure_t *get_SuperCap_Measure_Point(void)
{
    return &supercap;
}
//统一处理can中断函数，并记录发送数据的时间，作为离线判断的依据
void CAN1_Getdata(CAN_RxHeaderTypeDef *pHeader,uint8_t aData[])
{
	switch (pHeader->StdId)
	{
		case 0x0210:
		{
			//得到超级电容数据
			get_supercap_measure(&buckboost_control,aData);
		}
		default:
		{
			
		}break;
	}
}

void get_supercap_measure(BUCKBOOST_STRUCT *cap, uint8_t aData[])
{
    cap -> referee_power  = aData[0];
    cap -> referee_buff  = aData[1];
}

void supercap_sendmessage(uint16_t Vcap)
{
		static uint8_t supercap_tx_data[2];
	
		Tx1Message.StdId=0x211;
		Tx1Message.IDE =CAN_ID_STD;
		Tx1Message.RTR=CAN_RTR_DATA;
		Tx1Message.DLC =0x08;
		memcpy (supercap_tx_data, &Vcap, sizeof(Vcap));
		HAL_CAN_AddTxMessage(&hcan1, &Tx1Message, supercap_tx_data ,&pTxMailbox);
}

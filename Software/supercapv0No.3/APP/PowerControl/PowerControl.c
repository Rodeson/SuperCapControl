#include "PowerControl.h"

CONTROL_STRUCT battery_in;
CONTROL_STRUCT chassis_out;
CONTROL_STRUCT dcdc_in;
CONTROL_STRUCT dcdc_out;


//超级电容模块输入电流计算
//等效裁判系统监控底盘输出口输出功率
//Power_in = V_BAT * I_BAT
//如果反馈功率为负数 则出现电流反灌电池(危险)
float SuperCAPModel_INPower_Calculate(void)
{
    
}


#include "BuckboostControl.h"

BUCKBOOST_STRUCT buckboost_control;

CONTROL_STRUCT battery_in;
CONTROL_STRUCT chassis_out;
CONTROL_STRUCT dcdc_in;
CONTROL_STRUCT dcdc_out;

//BuckBoost电路PWM初始化
void BuckBoostInit(void)
{
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_PWM_Start(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH);
    HAL_TIM_PWM_Start(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH);
    HAL_TIM_PWM_Start(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH);
    HAL_TIM_PWM_Start(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH);
    
    //初始4个MOS全关
    __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,0);
    __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,1000);
    __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,1000);
    __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,0);
    //BuckBoostPID控制参数初始化
    //1.缓冲能量环
    PID_Init(&buckboost_control.Robot_Buffer_PID,
                           Robot_Buffer_PID_MODE,
                        Robot_Buffer_PID_MAX_OUT,
                       Robot_Buffer_PID_MAX_IOUT,
                             Robot_Buffer_PID_KP,
                             Robot_Buffer_PID_KI,
                             Robot_Buffer_PID_KD);    
    //2.功率环
    PID_Init(&buckboost_control.Robot_Power_PID,
                           Robot_Power_PID_MODE,
                        Robot_Power_PID_MAX_OUT,
                       Robot_Power_PID_MAX_IOUT,
                             Robot_Power_PID_KP,
                             Robot_Power_PID_KI,
                             Robot_Power_PID_KD);
    //3.电流环
    PID_Init(&buckboost_control.BuckBoostLoopCcompare_PID,
                                  BUCKBOOSTLOOPC_PID_MODE,
                               BUCKBOOSTLOOPC_PID_MAX_OUT,
                              BUCKBOOSTLOOPC_PID_MAX_IOUT,
                                    BUCKBOOSTLOOPC_PID_KP,
                                    BUCKBOOSTLOOPC_PID_KI,
                                    BUCKBOOSTLOOPC_PID_KD);
    //4.电压环
    PID_Init(&buckboost_control.BuckBoostLoopVcompare_PID,
                                  BUCKBOOSTLOOPV_PID_MODE,
                               BUCKBOOSTLOOPV_PID_MAX_OUT,
                              BUCKBOOSTLOOPV_PID_MAX_IOUT,
                                    BUCKBOOSTLOOPV_PID_KP,
                                    BUCKBOOSTLOOPV_PID_KI,
                                    BUCKBOOSTLOOPV_PID_KD);
   //滤波初始化
   for (uint8_t i = 0; i < 5; i++)
   {
       low_filter_init(&lpf[i], 10e3, 1e3);
   }
}

volatile float buck_duty = 0;
volatile float boost_duty = 0;
volatile uint32_t BB_CompareValue = 0;
volatile uint32_t BB_CompareValue_CAL= 0;
void BuckBoost_OUTControl(float ratio) // vout/vin
{    
volatile float CompareValue;    
//升降压判断        
    if(ratio < 0 && buckboost_control.power_dir == 1)
    {
        CompareValue = 0.96 * ratio * 1000;
        boost_duty = 0.06;
        buck_duty = 0.96 * ratio;
    }
    else if(ratio > 1.0f && buckboost_control.BUCKBOOST_CAP_V > buckboost_control.BUCKBOOST_IN_V)//需要升压
    {
        buckboost_control.buckboostmode = 2;
        buck_duty  = 0.05;
        boost_duty = 1 - buck_duty/ratio;
    }
    else if(ratio < 1.0f && ratio > 0)//需要降压
    {
        buckboost_control.buckboostmode = 1;
        boost_duty = 0.06;
        buck_duty  = ratio * (1 - boost_duty);
    }
    else//error ratio < 0
    {
        buckboost_control.buckboostmode = 0;
    }
    
    if(buckboost_control.BUCKBOOST_CAP_V > CAP_V_MAX && buckboost_control.power_dir == 0)
    {
        buckboost_control.buckboostmode = 0;
        buckboost_control.BuckBoostLoopCcompare_PID.out = 0;
        buckboost_control.BuckBoostLoopCcompare_PID.out = 0;
    }
    
    
    
//输出
    //放电模式
    if(buckboost_control.power_dir == 1 && buckboost_control.BUCKBOOST_CAP_V > CAP_V_MIN)
    {
        BB_CompareValue = 1000 * 0.94*buckboost_control.BUCKBOOST_CAP_V/buckboost_control.BUCKBOOST_IN_V;
        BB_CompareValue = 2 * BB_CompareValue + CompareValue;
        BB_CompareValue = BB_CompareValue / 2;
        
        if(BB_CompareValue > 950)
        {
            BB_CompareValue = 950;
        }
        else if(BB_CompareValue < 60)
        {
            BB_CompareValue = 60;
        }
      
        __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,BB_CompareValue);
        __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,BB_CompareValue);
        __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,60);
        __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,60);        
    }
    else if(buckboost_control.power_dir == 1 && buckboost_control.BUCKBOOST_CAP_V < CAP_V_MIN)
    {
        buckboost_control.BuckBoostLoopCcompare_PID.out = 0;
        __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,0);
        __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,0);        
    }
    //充电模式
    else if(buckboost_control.buckboostmode == 0)
    {
        __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,0);
        __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,0);          
    }
    else if(buckboost_control.buckboostmode == 1)
    {
        
        BB_CompareValue = BB_CompareValue + buck_duty * 1000;
        BB_CompareValue = BB_CompareValue /2;
        BB_CompareValue_CAL = 1000 * 0.94*buckboost_control.BUCKBOOST_CAP_V/buckboost_control.BUCKBOOST_IN_V;
        
        if(buckboost_control.power_dir == 0)
        {
            if(BB_CompareValue < BB_CompareValue_CAL)  BB_CompareValue = BB_CompareValue_CAL;
        }
        
        if(BB_CompareValue > 950)
        {
            BB_CompareValue = 950;
        }
        
        __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,BB_CompareValue);
        __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,BB_CompareValue);
        __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,60);
        __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,60);        
    }
    else if(buckboost_control.buckboostmode == 2)
    {
        BB_CompareValue = BB_CompareValue + boost_duty * 1000;
        BB_CompareValue = BB_CompareValue / 2;
        
//        __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,50);
//        __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,50);
//        __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,BB_CompareValue);
//        __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,BB_CompareValue);        
        __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,0);
        __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,0); 
    }
    else//error
    {
        __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,0);
        __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,0); 
    }
}

void BuckBoost_OUT(void)
{

#if USE_SUPERCAP == 1
    BuckBoost_OUTControl(buckboost_control.ratio_pass);
#elif USE_SUPERCAP == 0
        __HAL_TIM_SetCompare(&Q1_PWMControl_TIM,Q1_PWMControl_TIM_CH,0);
        __HAL_TIM_SetCompare(&Q2_PWMControl_TIM,Q2_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q3_PWMControl_TIM,Q3_PWMControl_TIM_CH,1000);
        __HAL_TIM_SetCompare(&Q4_PWMControl_TIM,Q4_PWMControl_TIM_CH,0);  
#endif    
    
    
    //板载RGB灯等效控制
    if(buckboost_control.power_dir == 0)
    {
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,500);
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,0);
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_4,0);        
    }
    else if(buckboost_control.power_dir == 1)
    {
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,0);
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,0);
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_4,500);        
    }
    else//error
    {
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,0);        
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_3,500);
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_4,0);        
    }  
}

//获取当前系统电压电流相关参数
//并计算当前功率
//现在这种写法不太稳定 后面在这里更新ADC数据并计算
void BuckBoost_FIFO_Feedback(void)
{

    buckboost_control.max_power_lim = buckboost_control.referee_power;
    buckboost_control.powerbuffer = buckboost_control.referee_buff * 2;
    
//低通滤波处理        
      lpf[0].Input = cap_c.real_valu1;
      lpf[1].Input = cap_v.real_valu1;
      lpf[2].Input = in_c.real_valu1;
      lpf[3].Input = in_v.real_valu1;
      lpf[4].Input = out_c.real_valu1;
    
      for (char i = 0; i < 5; i++)
      {
        low_filter_calc(&lpf[i]);
      }
      buckboost_control.BUCKBOOST_CAP_C = lpf[0].Output;
      buckboost_control.BUCKBOOST_CAP_V = lpf[1].Output;
      buckboost_control.BUCKBOOST_IN_C = lpf[2].Output;
      buckboost_control.BUCKBOOST_IN_V = lpf[3].Output;
      buckboost_control.BUCKBOOST_OUT_C = lpf[4].Output;
      
        
    //调试过程暂定1A需求 测试电容充电
    //buckboost_control.BUCKBOOST_NEED_C = 1;
    //调试过程暂定2A需求 测试电容放电
    //buckboost_control.BUCKBOOST_NEED_C = 2;

    battery_in.I_now = buckboost_control.BUCKBOOST_IN_C;
    battery_in.U_now = buckboost_control.BUCKBOOST_IN_V;
    battery_in.P_now = battery_in.I_now * battery_in.U_now;

    chassis_out.I_now = buckboost_control.BUCKBOOST_OUT_C;
    chassis_out.U_now = buckboost_control.BUCKBOOST_IN_V;
    chassis_out.P_now = chassis_out.I_now * chassis_out.U_now;

    dcdc_in.I_now = buckboost_control.BUCKBOOST_IN_C - buckboost_control.BUCKBOOST_OUT_C;
    dcdc_in.U_now = buckboost_control.BUCKBOOST_IN_V;
    dcdc_in.P_now = dcdc_in.I_now * dcdc_in.U_now;

    dcdc_out.I_now = buckboost_control.BUCKBOOST_CAP_C;
    dcdc_out.U_now = buckboost_control.BUCKBOOST_CAP_V;
    dcdc_out.P_now = buckboost_control.BUCKBOOST_CAP_C * buckboost_control.BUCKBOOST_CAP_V;

    buckboost_control.efficiency = (dcdc_out.P_now) / dcdc_in.P_now * 100;
    buckboost_control.data_update = 1;
}



//充电放电模式设置
void BuckBoost_Set_Control(void)
{
    //设置功率限制模式
    buckboost_control.POWER_LIMIT_MODE = 0;
    //设置缓冲能量 60J (仅在调试过程使用定值)
    //buckboost_control.powerbuffer = 60;
    //调试过程设置最大功率为55W
    //buckboost_control.max_power_lim = 30;
    
    //计算缓冲能量环
    buckboost_control.max_power_lim = buckboost_control.max_power_lim - PID_Calculate(&buckboost_control.Robot_Buffer_PID,buckboost_control.powerbuffer,30.0f);
    if(buckboost_control.Robot_Buffer_PID.out<-3)
    {
        buckboost_control.max_power_lim = buckboost_control.referee_power + 3;
    }
    
    if(buckboost_control.max_power_lim < 0)
    {
        buckboost_control.max_power_lim = 30;
    }
    
    //计算功率环
    buckboost_control.max_power_lim = 45;
    
    buckboost_control.charge_power = PID_Calculate(&buckboost_control.Robot_Power_PID,battery_in.P_now,buckboost_control.max_power_lim);
    
    //buckboost_control.charge_power = 12;
    if(buckboost_control.charge_power > 60)
    {
        buckboost_control.charge_power = 60;
    }
    else if(buckboost_control.charge_power <-120)
    {
        buckboost_control.charge_power = -120;
    }
    
    buckboost_control.charge_current = buckboost_control.charge_power / buckboost_control.BUCKBOOST_IN_V;
    
    //buckboost_control.charge_power > 0 超级电容充电 
    if(buckboost_control.charge_power > 0)
    {
        buckboost_control.power_dir = 0;
    }
    else if(buckboost_control.charge_power < 0)
    {
        buckboost_control.power_dir = 1;
    }
}

void BuckBoost_PID_Calculate(void)
{
    if(buckboost_control.power_dir == 0)
    {
        buckboost_control.cloop = PID_Calculate(&buckboost_control.BuckBoostLoopCcompare_PID,dcdc_in.I_now,buckboost_control.charge_current);
        buckboost_control.vloop = PID_Calculate(&buckboost_control.BuckBoostLoopVcompare_PID,buckboost_control.BUCKBOOST_CAP_V,CAP_V_MAX);

        if(buckboost_control.vloop <= buckboost_control.cloop)
        {
            buckboost_control.ratio_pass = buckboost_control.vloop / buckboost_control.BUCKBOOST_IN_V * 1.0f;
            buckboost_control.loop_mode = 0;
        }
        else
        {
            buckboost_control.ratio_pass = buckboost_control.cloop / buckboost_control.BUCKBOOST_IN_V * 1.0f;
            buckboost_control.loop_mode = 1;
        }
    }
    else if(buckboost_control.power_dir == 1)
    {
        buckboost_control.cloop = PID_Calculate(&buckboost_control.BuckBoostLoopCcompare_PID,dcdc_in.I_now,buckboost_control.charge_current);
        buckboost_control.ratio_pass = buckboost_control.cloop / buckboost_control.BUCKBOOST_CAP_V * 1.0f;
        buckboost_control.loop_mode = 1;
    }
    
    if(buckboost_control.ratio_pass > buckboost_control.BUCKBOOST_CAP_V / buckboost_control.BUCKBOOST_IN_V + 0.2f)
    {
        buckboost_control.ratio_pass = buckboost_control.BUCKBOOST_CAP_V / buckboost_control.BUCKBOOST_IN_V + 0.2f;
    }
}








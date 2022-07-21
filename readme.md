超级电容

@Rodeson

Email：lds947003754@outlook.com



## 1.超级电容硬件说明

1. 使用4Buckboost电路，实现升降压控制。
2. 将控制板和功率板进行拆分设计，两个板子对应接口固定，PWM控制线通过FPC排线连接，电源接口使用XT30接口连接。
3. 更改设计后，V2.0板子实际面积更小，在舵轮上安装方便。
4. 在实际使用过程中，为方便了解超级电容工作状态，视觉上增加RGB灯显示板子工作情况，听觉上增加蜂鸣器，能够在工作异常时第一时间了解情况。
5. 传感器选型上使用ina240a1和opa2350ua，使用adc读取模拟信号，上一代超级电容使用ina226容易出现i2c通信掉线情况。
6. 增加板载温度传感器MCP9700AT-E_TT，监测超级电容工作温度。
7. PCB设计时，减小PCB上大电流走线距离，板载大电流开窗布线。
8. 增加可交互的五维按键和OLED显示屏，方便现场调参。



## 2.硬件设计

2.1 采样电路

2.1.1接口采样电路

![](Images/XT30采样电路.png)

2.1.2电流传感器

![](Images/电流计电路设计.png)

2.1.3电压传感器

![](Images/电压测量运放电路设计.png)

2.2 BuckBoost电路

![](Images/4BuckBoost驱动电路.png)

2.3供电路径

![](Images/超级电容供电关系.png)

## 3.超级电容控制思路

![超级电容控制逻辑](Images/超级电容控制逻辑.jpg)

## 4.超级电容buckboost模块控制逻辑

### 4.1采样数据计算

![采样数据计算](D:\Desktop\采样数据计算.jpg)

```c
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
```

### 4.2串级PID思路

#### 4.2.1缓冲能量环

![缓冲能量环](Images/缓冲能量环.jpg)

![输出设置](Images/输出设置.jpg)

![电压电流环](Images/电压电流环.jpg)

![功率环](Images/功率环.jpg)

#### 4.2.2功率环

![功率环](Images/功率环.jpg)

#### 4.2.3电压电流环

![电压电流环](Images/电压电流环.jpg)

### 4.3输出设置

![输出设置](Images/输出设置.jpg)
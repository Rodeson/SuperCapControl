#include "filter.h"
//#include "core_cm4.h"
#include "main.h"
LOWPASS_FILTER_STRUCT lpf[5];

/*1.限幅滤波(目测能用)
方法：根据经验判断，确定两次采样允许的最大偏差值（设为LIMIT）每次检测到新值时判断：
        如果本次值与上次值之差 <= LIMIT,则本次值有效如果本次值与上次值之差 > LIMIT,则本次值无效,放弃本次值,用上次值代替本次值
优点：能有效克服因偶然因素引起的脉冲干扰
缺点：无法抑制那种周期性的干扰，且平滑度差
*/
//#define LIMIT 10
//u16 oldValue;
//u16 First_flag=0;
//u16 amplitudeLimiterFilter()
//{
//    u16 newValue = getValue();
//		if(First_flag==0){First_flag=1;oldValue=newValue;return newValue;}
//		if(First_flag==1)
//		{
//					if(((newValue - oldValue) > LIMIT )|| ((oldValue - newValue) < LIMIT))
//					{
//							return oldValue;
//					}
//					else
//					{
//							oldValue=newValue;
//							return newValue;
//					}
//		}
//}

/*-----------------------------------------------------------------------
2.中位值滤波(能用！！！)
方法：连续采样N次（N取奇数）把N次采样值按大小排列取中间值为本次有效值
优点：能有效克服因偶然因素引起的波动干扰；对温度、液位等变化缓慢的被测参数有良好的滤波效果
缺点：对流量，速度等快速变化的参数不宜*/
#define N 99
uint16_t middleValueFilter()
{
  uint16_t value_buf[N];
  uint16_t i, j, k, temp;
  for (i = 0; i < N; ++i)
  {
    value_buf[i] = getValue();
  }
  for (j = 0; j < N - 1; ++j)
  {
    for (k = 0; k < N - j - 1; ++k)
    {
      //从小到大排序，冒泡法排序
      if (value_buf[k] > value_buf[k + 1])
      {
        temp = value_buf[k];
        value_buf[k] = value_buf[k + 1];
        value_buf[k + 1] = temp;
      }
    }
  }

  return value_buf[(N - 1) / 2];
}

/*-----------------------------------------------------------------------
3.算术平均滤波(能用！！！)
方法：连续取N个采样值进行算术平均运算;
        N值较大时：信号平滑度较高，但灵敏度较低
        N值较小时：信号平滑度较低，但灵敏度较高
        N值的选取：一般流量，N=12；压力：N=4
优点：试用于对一般具有随机干扰的信号进行滤波。
        这种信号的特点是有一个平均值，信号在某一数值范围附近上下波动。
缺点：测量速度较慢或要求数据计算较快的实时控制不适用*/

//#define N 10
//u16 averageFilter()
//{
//    u16 sum = 0;
//    u8 i;
//    for(i = 0; i < N; ++i)
//    {
//        sum += getValue();
//    }
//    return sum/N;
//}

///*-----------------------------------------------------------------------
////4.递推平均滤波法（又称滑动平均滤波法）(能用！！！)
////方法：把连续取N个采样值看成一个队列，队列的长度固定为N
////         每次采样到一个新数据放入队尾,并扔掉原来队首的一次数据(先进先出原则)
////         把队列中的N个数据进行算术平均运算,就可获得新的滤波结果
////         N值的选取：流量，N=12；压力：N=4；液面，N=4~12；温度，N=1~4
////优点：对周期性干扰有良好的抑制作用，平滑度高；试用于高频振荡的系统
////缺点：灵敏度低；对偶然出现的脉冲性干扰的抑制作用较差，不适于脉冲干扰较严重的场合
////        比较浪费RAM（改进方法，减去的不是队首的值，而是上一次得到的平均值） */

//#define N 10
//u16 value_buf[N];
//u16 sum=0;
//u16 curNum=0;
//u16 moveAverageFilter()
//{
//    if(curNum < N)
//    {
//        value_buf[curNum] = getValue();
//        sum += value_buf[curNum];
//			  curNum++;
//        return sum/curNum;
//    }
//    else
//    {
//        sum -= sum/N;
//        sum += getValue();
//        return sum/N;
//    }
//}

/*-----------------------------------------------------------------------
5.中位值平均滤波（防脉冲干扰平均滤波法）(能用！！！)
方法：相当于“中位值滤波法”+“算术平均滤波法”
        连续采样N个数据，去掉一个最大值和一个最小值然后计算N-2个数据的算术平均值
        N值的选取：3~14
优点：融合了两种滤波的优点。对于偶然出现的脉冲性干扰，可消除有其引起的
         采样值偏差。对周期干扰有良好的抑制作用，平滑度高，适于高频振荡的系统。
缺点：测量速度慢*/
//#define N 10
//u16 middleAverageFilter()
//{
//    u16 i,j,k;
//    u16 temp,sum = 0;
//    u16 value_buf[N];
//    for(i = 0; i < N; ++i)
//    {
//        value_buf[i] = getValue();
//    }
//    /*从小到大冒泡排序*/
//    for(j = 0; j < N-1; ++j)
//    {
//        for(k = 0; k < N-j-1; ++k)
//        {
//            if(value_buf[k] > value_buf[k+1])
//            {
//                temp = value_buf[k];
//                value_buf[k] = value_buf[k+1];
//                value_buf[k+1] = temp;
//
//            }
//        }
//    }
//    for(i = 1; i < N-1; ++i)
//    {
//        sum += value_buf[i];
//    }
//    return sum/(N-2);
//}

/*-----------------------------------------------------------------------
6.递推中位值滤波法
 优点：对于偶然出现的脉冲性干扰，可消除由其引起的采样值偏差。
 对周期性干扰有良好的抑制作用，平滑度高；
 试用于高频振荡的系统。
 缺点：测量速度慢

取最近的10个值，去掉最大最小值求平均
队列queue中，第0个值换成新值，其余值依次往后移一个位置*/
//#define N 10
//u16 value_buf[N];
//u16 sum=0;
//u16 curNum=0;
//u16 max;
//u16 min;
//u16 recursionMiddleFilter()
//{
//    u16  i,j,k,temp,flag=0,temp_1,temp_2,flag_2=0,flag_3=0;
//	  if(curNum < N)
//    {
//        value_buf[curNum] = getValue();
//			  curNum++;
//    }
//    else if (curNum == N)
//    {
//         /*从小到大冒泡排序*/
//					for(j = 0; j < N-1; ++j)
//				{
//						for(k = 0; k < N-j-1; ++k)
//						{
//								if(value_buf[k] > value_buf[k+1])
//								{
//										temp = value_buf[k];
//										value_buf[k] = value_buf[k+1];
//										value_buf[k+1] = temp;
//								}
//						}
//				}
//				  for(i=1;i<N-1;i++)
//				{
//						sum+=value_buf[i];
//				}
//				max=value_buf[N-2];
//				min=value_buf[1];
//				curNum++;
//				return sum/(N-2);
//    }
//		else
//		{
//				  temp_1=getValue();
//			    if((temp_1<min)&&(temp_1>max))
//				{
//					flag++;flag_3=1;
//					if(temp_1<min) flag_2=1;
//					if(temp_1>max) flag_2=2;
//				}
//					else
//				{
//					for(i=1;value_buf[i]<temp_1;i++)
//					{
//						value_buf[i-1]=value_buf[i];
//					}
//					value_buf[i]=temp_1;
//				}
//				  temp_2=getValue();
//			    if((temp_2<min)&&(temp_2>max))
//				{
//					flag++;flag_3=2;
//					if(temp_2<min) flag_2=1;
//					if(temp_2>max) flag_2=2;
//				}
//				  else
//				{
//					for(i=N-2;value_buf[i]>temp_2;i--)
//					{
//						value_buf[i+1]=value_buf[i];
//					}
//					value_buf[i]=temp_2;
//				}
//				  if(flag==2)
//				{
//					return sum/(N-2);
//				}
//				  if(flag==0)
//				{
//					sum=sum+temp_1 +temp_2-max-min;
//					max=value_buf[N-2];
//				  min=value_buf[1];
//					return sum/(N-2);
//				}
//				  if(flag==1)
//				{
//					if(flag_3==1)
//					{
//						if(flag_2==1){sum=sum-max+temp_2;max=value_buf[N-2];}
//						if(flag_2==2){sum=sum-min+temp_2;min=value_buf[1];}
//					}
//
//					if(flag_3==2)
//					{
//						if(flag_2==1){sum=sum-max+temp_1;max=value_buf[N-2];}
//						if(flag_2==2){sum=sum-min+temp_1;min=value_buf[1];}
//					}
//					return sum/(N-2);
//				}
//
//    return sum/(N-2);
//}

/*-----------------------------------------------------------------------    
7.限幅平均滤波法(能用！！！)
方法：相当于“限幅滤波法”+“递推平均滤波法”
        每次采样到的新数据先进行限幅处理再送入队列进行递推平均滤波处理
优点：对于偶然出现的脉冲性干扰，可消除有其引起的采样值偏差。    
缺点：比较浪费RAM  */
//#define A 50
//#define N 12
//u16 data[N];
//u16 First_flag=0;
//u16 filter()
//{
//  u16 i;
//  u16 temp,sum,flag=0;
//  data[0]=getValue();
//	for(i=1;i<N;i++)
//	{
//		temp=getValue();
//		if((temp-data[i-1])>A||((data[i-1]-temp)>A))
//		{
//		  i--;flag++;
//		}
//		else
//		{
//			data[i]=temp;
//		}
//	}
//  for(i=0;i<N;i++)
//  {
//    sum+=data[i];
//  }
//  return  sum/N;
//}

///*8.一阶滞后滤波法
////方法：取a=0~1,本次滤波结果=（1-a）*本次采样值+a*上次滤波结果
////优点：对周期性干扰具有良好的抑制作用适用于波动频率较高的场合
////缺点：相位滞后，灵敏度低滞后程度取决于a值大小不能消除滤波频率高于采样频率的1/2的干扰信号*/
//float a;
//u16 firstOrderFilter(u16 newValue, u16 oldValue)
//{
//    return  a * newValue + (1-a) * oldValue;
//}

////使用
//value = getValue();
//while(1)
//{
//    value = firstOrderFilter(getValue(),value);
//}

///*-----------------------------------------------------------------------
////9.加权递推平均滤波法（并没有递推）
////方法：是对递推平均滤波法的改进，即不同时刻的数据加以不同的权；
////        通常是，越接近现时刻的数值，权取得越大；
////        给予新采样值的权系数越大，则灵敏度越高，但信号平滑度越低。
////优点：适用于有较大纯滞后时间常数的对象和采样周期较短的系统
////缺点：对于纯滞后时间常数较小，采样周期较长
////        变化缓慢的信号不能迅速反应系统当前所受干扰的严重程度，滤波效果差*/

//#define N 10
//u8 weight[N] = {1,2,3,4,5,6,7,8,9,10};
//u8 weigth_sum = 1+2+1+2+3+4+5+6+7+8+9+10;
//u16 weightAverageFilter()
//{
//    u16 value_buf[N];
//    u16 i, sum = 0;
//    for(i = 0; i < N; ++i)
//    {
//        value_buf[i] = getValue();
//        delay();
//    }
//    for(i = 0; i < N; ++i)
//    {
//        sum += value_buf[i] * weight[i];
//    }
//    return sum/weight_sum;
//}

///*-----------------------------------------------------------------------
////10.消抖滤波法
////方法：比如开关，刚按下去的时候会产生抖动，但是经过一段时间后回到稳态；
////        如果N次后，还不是稳态，就取当前值作为新状态值
////        设置一个滤波计数器，将每次采样值与当前有效值比较：
////        如果采样值＝当前有效值，则计数器清零
////        如果采样值<>当前有效值，则计数器+1，并判断计数器是否>=上限N(溢出)
////        如果计数器溢出,则将本次值替换当前有效值,并清计数器
////优点：对于变化缓慢的被测参数有较好的滤波效果,可避免在临界值附近控制器的反复开/关跳动或显示器上数值抖动
////缺点：对于快速变化的参数不宜
////        如果在计数器溢出的那一次采样到的值恰好是干扰值,则会将干扰值当作有效值*/

//u16 glitchFilter(u16 oldValue)
//{
//    u16 newValue = getValue();
//    u16 count = 0;
//    while(oldValue != newValue)
//    {
//        count ++;
//        if(count >= N)
//        {
//            return newValue;
//        }
//        delay();
//        newValue = getValue();
//    }
//    return oldValue;
//}

/***********************3西格玛滤波法(能用！！！)****************************/
//	#define N 30
//	#define P_sigma  0.0626499   //1.253/(genhao_n)(genhao_n-1)
//	u16 data[N];
//	u16 error[N];
//	u16 Three_Standard_Deviation_filter()
//	{
//			u16 i,j=0;
//			float Standard_Deviation;
//			u32 temp,sum,average;
//			for(i=0;i<N;i++)
//		{
//			data[i]=getValue();
//		}
////		for(i=0;i<N;i++)
////		{
////			printf("data[%d]=%d\r\n",i,data[i]);
////		}
//			sum=0;
//			for(i=0;i<N;i++)
//		{
//			sum+=data[i];
//		}
//			average = sum/N;printf("average=%d\r\n",average);
//			for(i=0;i<N;i++)
//		{
//			if(data[i]>average)
//			{error[i]=data[i]-average;}
//			else
//			{error[i]=average-data[i];}
//		}
////		for(i=0;i<N;i++)
////		{
////			printf("error[%d]=%d\r\n",i,error[i]);
////		}
//			sum=0;
//			for(i=0;i<N;i++)
//		{
//			sum+=error[i];
//		}
//			Standard_Deviation=P_sigma*sum; //printf("Standard_Deviation=%f\r\n",Standard_Deviation);
//			for(i=0;i<N;i++)
//		{
//			if((data[i]<average-(u16)Standard_Deviation)||(data[i]>average+(u16)Standard_Deviation))
//			{
//				data[i]=0;j++;
//			}
//		}
////		for(i=0;i<N;i++)
////		{
////			printf("data[%d]=%d\r\n",i,data[i]);
////		}
//			sum=0;
//			for(i=0;i<N;i++)
//		{
//			sum+=data[i];
//		}
//		printf("sum/(N-j)=%d\r\n",sum/(N-j));
//		printf("sum/(N-j)/4096*3.3=%f\r\n",(float)sum/(N-j)/4096*3.3);
//			return  sum/(N-j);
//	}

/***********************去极限算数平均值滤波(能用！！！)****************************/

//#define N 50
//#define Enum 5
//u16 abandon_extremum_averageFilter()
//{
//    u16 temp,value_buf[N];
//	  u32 sum = 0;
//    u8  i,j,k;
//    for(i = 0; i < N; ++i)
//    {
//        value_buf[i]= getValue();
//    }
//
//		/*从小到大冒泡排序*/
//    for(j = 0; j < N-1; ++j)
//    {
//        for(k = 0; k < N-j-1; ++k)
//        {
//            if(value_buf[k] > value_buf[k+1])
//            {
//                temp = value_buf[k];
//                value_buf[k] = value_buf[k+1];
//                value_buf[k+1] = temp;
//
//            }
//        }
//    }
//		for(i = Enum; i < N-Enum; ++i)
//    {
//        sum+=value_buf[i];
//    }
//    return sum/(N-2*Enum);
//}
/*---------------------------getValue();--------------------------------------------*/
uint16_t getValue()
{
  return 0;
}
/*------一阶低通滤波-----------------------------------------------------*/
void low_filter_init(LOWPASS_FILTER_STRUCT *p, float sample_f, float cutoff_f)
{
  float Tc; //时间常数τ
  p->Fc = cutoff_f;
  p->Fs = sample_f;
  if (p->Fc <= 0.0f || p->Fs <= 0.0f)
  {
    p->Ka = 1.0f;
    p->Kb = 0.0f;
    return;
  }

  Tc = 1.0f / (PI2 * p->Fc);

  p->Ka = 1.0f / (1.0f + Tc * p->Fs);
  p->Kb = 1.0f - p->Ka;
  p->Input = 0.0f;
  p->Output = 0.0f;
}

void low_filter_calc(LOWPASS_FILTER_STRUCT *p)
{
  if (p->Output == p->Input)
    return;

  p->Output = p->Ka * p->Input + p->Kb * p->Output;
}
//冒泡排序
void bubble_sort(uint16_t *a, unsigned char Num) //冒泡法排序
{
  uint16_t *p = a;
  uint16_t i, j, tmp;
  for (i = 0; i < Num - 1; i++)
  {
    for (j = 0; j + i < Num - 1; j++)
    {
      if (a[j] > a[j + 1])
      {
        tmp = p[j];
        p[j] = p[j + 1];
        p[j + 1] = tmp;
      }
    }
  }
}
/*递归快速排序*/
int partition(int arr[], int low, int high)
{
  int key;
  key = arr[low];
  while (low < high)
  {
    while (low < high && arr[high] >= key)
      high--;
    if (low < high)
      arr[low++] = arr[high];
    while (low < high && arr[low] <= key)
      low++;
    if (low < high)
      arr[high--] = arr[low];
  }
  arr[low] = key;
  return low;
}
void quick_sort(int arr[], int start, int end)
{
  int pos;
  if (start < end)
  {
    pos = partition(arr, start, end);
    quick_sort(arr, start, pos - 1);
    quick_sort(arr, pos + 1, end);
  }
  return;
}
//中位平均滤波//
uint32_t middle_filter(uint16_t *a, unsigned char Num, char extremum)
{
  uint8_t index = 0x00;
  uint32_t avg_sample = 0x00;
  bubble_sort(a, Num);
  // quick_sort(a,0,Num-1)  ;
  for (index = extremum; index < Num - extremum; index++)
  {
    avg_sample += a[index];
  }
  /* 计算N-X 个ADC 采样的均值 */
  avg_sample /= Num - 2 * extremum;
  /* 返回均值 */
  return avg_sample;
}

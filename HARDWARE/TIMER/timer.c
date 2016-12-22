#include "timer.h"
#include "led.h"
#include "usart.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//PWM  驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/03
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数

//定时器2通道1输入捕获配置

TIM_ICInitTypeDef  TIM2_ICInitStructure;

void TIM2_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;  //PA0 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 输入  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PA0 下拉
	
	//初始化定时器2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM2输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频  分频n次即输入n个周期采集一次
  	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	




	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;  //PA0 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 输入  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PA0 下拉

	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频  分频n次即输入n个周期采集一次
  	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
    TIM_Cmd(TIM2,ENABLE ); 	//使能定时器2
 
}


u8  TIM2CH1_CAPTURE_STA=0;	//输入捕获状态		    				
u16	TIM2CH1_CAPTURE_VAL;	//输入捕获值
int counter=0,CaptureNum=0,fre=0,Tim=0,FQ=0,TalFr; 
extern int flag;

int counter2=0,CaptureNum2=0,fre2=0,Tim2=0,FQ2=0,TalFr2; 
extern int flag2;
//定时器5中断服务程序	 
void TIM2_IRQHandler(void)
{ 
  if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{	    
		   TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
		    if(CaptureNum==0)
	    {
			  counter=TIM_GetCapture1(TIM2);
			   CaptureNum=1;
		} else 
		    if(CaptureNum==1)
			 {
				if(TIM2,TIM_GetITStatus(TIM2,TIM_IT_Update)!=SET)
				{Tim=TIM_GetCapture1(TIM2);
				 Tim=Tim-counter;
				
				} else
				{TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
				 Tim=0xffff+TIM_GetCapture1(TIM2)-counter+1;
				
				}
			  CaptureNum=0;


			}


	  if(Tim!=0)
		 {
		  fre=1000000/Tim;
		  flag++;
		  FQ=FQ+fre;
		 }


      if(flag==200)
		{flag=0;
	     TalFr=(int)FQ/200;
		 FQ=0;
		}

	}

/////////////////////////////////////////////////////////////////////
	if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
{	    
		   TIM_ClearITPendingBit(TIM2,TIM_IT_CC2);
		    if(CaptureNum2==0)
		{
			  counter2=TIM_GetCapture2(TIM2);
			   CaptureNum2=1;
		} else if(CaptureNum2==1)
			{
				if(TIM2,TIM_GetITStatus(TIM2,TIM_IT_Update)!=SET)
				{Tim2=TIM_GetCapture2(TIM2);
				 Tim2=Tim2-counter2;
				
				} else
				{TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
				 Tim2=0xffff+TIM_GetCapture2(TIM2)-counter2+1;
				
				}
			  CaptureNum2=0;


			}
	
	 if(Tim2!=0)
		 {
		  fre2=1000000/Tim2;
		  flag2++;
		  FQ2=FQ2+fre2;
		 }

	
	if(flag2==200)
	{    flag2=0;
	     TalFr2=(int)FQ2/200;
		 FQ2=0;
	}

}
    
    
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
 
}

#include "timer.h"
#include "led.h"
#include "usart.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//PWM  ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��

//��ʱ��2ͨ��1���벶������

TIM_ICInitTypeDef  TIM2_ICInitStructure;

void TIM2_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ��TIM2ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOAʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;  //PA0 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 ����  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PA0 ����
	
	//��ʼ����ʱ��2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//Ԥ��Ƶ��   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
  
	//��ʼ��TIM2���벶�����
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ  ��Ƶn�μ�����n�����ڲɼ�һ��
  	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2,ENABLE);//��������ж� ,����CC1IE�����ж�	




	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;  //PA0 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0 ����  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PA0 ����

	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ  ��Ƶn�μ�����n�����ڲɼ�һ��
  	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
  	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
    TIM_Cmd(TIM2,ENABLE ); 	//ʹ�ܶ�ʱ��2
 
}


u8  TIM2CH1_CAPTURE_STA=0;	//���벶��״̬		    				
u16	TIM2CH1_CAPTURE_VAL;	//���벶��ֵ
int counter=0,CaptureNum=0,fre=0,Tim=0,FQ=0,TalFr; 
extern int flag;

int counter2=0,CaptureNum2=0,fre2=0,Tim2=0,FQ2=0,TalFr2; 
extern int flag2;
//��ʱ��5�жϷ������	 
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
    
    
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
 
}

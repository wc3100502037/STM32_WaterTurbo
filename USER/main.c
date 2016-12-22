#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "spi.h"
#include "24l01.h"   
#include "adc.h"
#include"timer.h"
#include"pwm.h"
//ALIENTEK Mini STM32开发板范例代码24
//无线通信实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  
     extern  u8  TIM2CH1_CAPTURE_STA;	//输入捕获状态		    				
     extern  u16	TIM2CH1_CAPTURE_VAL;	//输入捕获值
	 int flag=0,flag2=0,Temp1,Temp2,Temp3,Temp4,Temp5;
	 float Adc1,Adc2;
	 extern int fre,TalFr,fre2,TalFr2;
   	 u8 tmp_buf[10]; 
	 u8 blue_buf[22];
	 u8* tempbuff=tmp_buf;
	 u16 CurrentValue=400;
	 float Torque;
	 int TorqueTmp,Speed;
	 u8 FLAG=1;
///////////////////////////////////////////////////////////////////////////////////////////////// 
 void convertToEight(u8 *num,u16 target)
  {u8 temp2;
  *num=((u8)target)&0x00ff;	 //低位转入num
  temp2=(target>>8)&0x00ff;
  *(++num)=((u8)temp2)&0x00ff;	//高位转入num++
  }

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 u16 convertToSix(u8 *num)
   {u16 temp1=0,temp2=0;
    temp1=(*num)&0x00ff;
 
    temp2=(*(++num))&0x00ff;
	temp2=temp2<<8;
    temp1=temp1+temp2;
     return temp1;

   }
 ////////////////////////////////////////////////////////////////////////////////////////////////////
   int main(void)
 { 
	u8 key,mode;
	u16 t=0;			 

	delay_init();	    	 //延时函数初始化	
	NVIC_Configuration();	 
	uart_init(9600);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
	LCD_Init();			   	//初始化LCD 	
 	KEY_Init();				//按键初始化
 	NRF24L01_Init();    	//初始化NRF24L01  
	Adc_Init();
	TIM2_Cap_Init(0xffff,72-1);
	TIM1_PWM_Init(3456,0);
	tmp_buf[4]=11;
 	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,50,200,16,16,"Mini STM32");	
	blue_buf[0]=0;
	blue_buf[3]=1;
	blue_buf[4]=2;
	blue_buf[7]=3;
	blue_buf[8]=4;
	blue_buf[11]=5;
	blue_buf[12]=6;
	blue_buf[15]=7;
	blue_buf[16]=8;
	blue_buf[19]=9;
	blue_buf[20]=33;
	blue_buf[21]=34;
 	while(NRF24L01_Check())	//检查NRF24L01是否在位.	
	{
		LCD_ShowString(60,80,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(60,80,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(60,80,200,16,16,"NRF24L01 OK");
 	while(1)//在该部分确定进入哪个模式!
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			mode=1;   
			break;
		}else if(key==KEY1_PRES)
		{
			mode=0;
			break;
		}
		t++;
		if(t==100)LCD_ShowString(10,100,230,16,16,"KEY0:RX_Mode  KEY1:TX_Mode"); //闪烁显示提示信息
 		if(t==200)
		{	
			LCD_Fill(10,100,230,150+16,WHITE);
			t=0; 
		}
		delay_ms(5);	  
	}   
 	LCD_Fill(10,150,240,166,WHITE);//清空上面的显示		  
 	POINT_COLOR=BLUE;//设置字体为蓝色	   
	if(mode==0)//RX模式
	{
		LCD_ShowString(60,120,150,16,16,"NRF24L01 RX_Mode");	
		LCD_ShowString(60,140,150,16,16,"Received DATA:");	
		NRF24L01_RX_Mode();		  
		while(1)
		{	 tempbuff=blue_buf; 		    		    				 
			if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
			{	blue_buf[1]=tmp_buf[0];
			    blue_buf[2]=tmp_buf[1];

				blue_buf[5]=tmp_buf[2];
				blue_buf[6]=tmp_buf[3];

				blue_buf[9]=tmp_buf[4];
				blue_buf[10]=tmp_buf[5];

				blue_buf[13]=tmp_buf[6];
				blue_buf[14]=tmp_buf[7];

				blue_buf[17]=tmp_buf[8];
				blue_buf[18]=tmp_buf[9];
			    	 
				LCD_ShowString(40,180,160,16,16,"Torque:  .   ");	
				LCD_ShowString(40,200,160,16,16,"Speed:");	
				LCD_ShowString(40,220,160,16,16,"Temp3:");	
				LCD_ShowString(40,240,160,16,16,"Temp4:");	
				LCD_ShowString(40,260,160,16,16,"Temp5:");	
		
                LCD_ShowxNum(92,180,convertToSix(&tmp_buf[0]),8,16,0);
				LCD_ShowxNum(100,200,convertToSix(&tmp_buf[2]),8,16,0);
				LCD_ShowxNum(100,220,convertToSix(&tmp_buf[4]),8,16,0);
				LCD_ShowxNum(100,240,convertToSix(&tmp_buf[6]),8,16,0);
			    LCD_ShowxNum(100,260,convertToSix(&tmp_buf[8]),8,16,0);
				
				
				  while((tempbuff-blue_buf)!=21) 
	           {
	                USART_SendData(USART1,*tempbuff);  
	              while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		            tempbuff++;
               }

			}else delay_us(100);	   
			t++;
			if(t==10000)//大约1s钟改变一次状态
			{
				t=0;
				LED0=!LED0;
			} 				    
		};	
	}else//TX模式
	{	
	    LCD_ShowString(60,120,200,32,16,"Sended DATA:");							    
		LCD_ShowString(60,140,239,16,16,"NRF24L01 TX_Mode");
	
		NRF24L01_TX_Mode();
	//	mode=' ';//从空格键开始  
		while(1)
		{
		 if(FLAG==1)
		 {
		 LCD_ShowString(40,180,160,16,16,"Torque:  .   ");	
	     }else{
		 LCD_ShowString(40,180,160,16,16,"NToque:  .   ");	
		 }

		 LCD_ShowString(40,200,160,16,16,"Speed:");		 
         LCD_ShowString(156,180,40,32,16,"N/M");
		 LCD_ShowString(156,200,40,32,16,"r/min");
		 Temp1=TalFr;

		 Temp2=TalFr2;		   				 
		 Temp3=Get_Adc_Average(ADC_Channel_10,10);
		 Temp4=Get_Adc_Average(ADC_Channel_12,10);
		 Temp5=Get_Adc_Average(ADC_Channel_13,10);
		 TIM_SetCompare1(TIM1,CurrentValue);


          if((Temp1>10000)&&(Temp1<15050))
		  {	Torque=(float)(4.00*((Temp1-10000.0)/1000.0));	//运行浮点数时，运算式里必须有浮点数1000.00
		  	   FLAG=1;
		  }else if((Temp1<10000)&&(Temp1>4950))
		  {
		    Torque=4.00*((10000.0-Temp1)/1000.0);
			FLAG=0;
		
		  }

             Speed=Temp2;
			if(NRF24L01_TxPacket(tmp_buf)==TX_OK)
		{	    convertToEight(&tmp_buf[0],Temp1);
				convertToEight(&tmp_buf[2],Temp2);
				convertToEight(&tmp_buf[4],Temp3);
				convertToEight(&tmp_buf[6],Temp4);
				convertToEight(&tmp_buf[8],Temp5);
				
				
			   
				LCD_ShowString(40,220,160,16,16,"Temp3:");	
				LCD_ShowString(40,240,160,16,16,"Temp4:");	
				LCD_ShowString(40,260,160,16,16,"Temp5:");	
           	   if(FLAG==1){
		        TorqueTmp=Torque;
				LCD_ShowxNum(96,180,TorqueTmp,2,16,0);
				TorqueTmp=(Torque-TorqueTmp)*1000;				
			    LCD_ShowxNum(120,180,TorqueTmp,3,16,0x80);
				}else{
				TorqueTmp=Torque;
				LCD_ShowxNum(96,180,TorqueTmp,2,16,0);
				TorqueTmp=(Torque-TorqueTmp)*1000;				
			    LCD_ShowxNum(120,180,TorqueTmp,3,16,0x80);
				 }

				LCD_ShowxNum(100,200,Speed,6,16,0);
				LCD_ShowxNum(100,220,Temp3,8,16,0);
				LCD_ShowxNum(100,240,Temp4,8,16,0);
			    LCD_ShowxNum(100,260,Temp5,8,16,0);
			}else	   	
			{										   	
 				LCD_ShowString(60,120,239,32,16,"Send Failed "); 
				LCD_Fill(0,138,240,218,WHITE);//清空上面的显示			   
			};
			LED0=!LED0;
			delay_ms(200);				    
		};
	} 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

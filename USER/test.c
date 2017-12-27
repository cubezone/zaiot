//#include <stm32f10x_lib.h>
#include <stm32f10x_map.h>
#include <stm32f10x_nvic.h>      

#include "sys.h"
#include "delay.h"	   
#include "pwm.h"
#include "ds18b20.h"
#include "usart.h"

int i;
int m = 0;
int ret;

// PB0 infar input.
// PB1  BEEP
// PB2  value
// PB3  DS18B20

// PB8  LED

float temperature_test(void);
void IoT_Post(float tmp);
void IoT_Post_d(float tmp);
float g_otmp;
float g_ntmp;

int main(void)
{
	
	Stm32_Clock_Init(9);//系统时钟设置
	delay_init(72);		//延时初始化
	
	RCC->APB2ENR |= 0x00000001; //afio
  AFIO->MAPR   = (0x00FFFFFF & AFIO->MAPR)|0x02000000;  //disable JTAG  release PB3,PB4 to GPIO
	RCC->APB2ENR |= 0X0000001c;//IO PORT A,B,C
	
	GPIOB->CRL = 0X33337338;    //3推挽输出, 7开漏输出,8 上/下拉输入,
	GPIOB->CRH = 0X33333333;    //3输出

	GPIOB->ODR = 1<<0 ;
	
	uart_init(72,115200); //PA9,PA10
	
	delay_ms(1000);
	printf("AT+RST\n");
	
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
	
	for ( i = 0 ; i < 5 ; i++)
	{
		//printf("AT+CWJAP=\"TP-LINK_3000\",\"1521521521\"\r\n");
		printf("AT+CWJAP=\"TAC-CUC\",\"qwertasdfg\"\r\n");
		delay_ms(1000);
		delay_ms(1000);
	}
	
	while(1) 
	{ 
		delay_ms(1000);
		//LED
		PBout(8) = 0;delay_ms(200);
		PBout(8) = 1;delay_ms(200);	
		PBout(12) = 0;delay_ms(200);
		PBout(12) = 1;delay_ms(200);	
		//printf("AAAAAAA");
		g_ntmp = temperature_test();
		IoT_Post(g_ntmp);
		if ((g_ntmp - g_otmp) > 0.2
			  || (g_ntmp - g_otmp) < -0.2
			 )
		{
			IoT_Post_d(g_ntmp);
			g_otmp = g_ntmp;
		}				
		
		//IoT_Post(	22.33);
	}
	
	// led
	for (i = 0 ; i < 3; i ++)
	{
		PBout(8) = 0;delay_ms(200);
		PBout(8) = 1;delay_ms(200);	
	}	
	// BEEP pwm
	{				
		pwm(2,2,100);
		delay_ms(50);
		pwm(2,2,100);
		delay_ms(50);
		pwm(2,2,100);
		delay_ms(50);
		pwm(1,1,200);
		delay_ms(50);		
	}
	
	while(1)
	{
		temperature_test();
		delay_ms(500);
	}
	
	while(1)
	{
	  ret = infar_test();		
		if (ret >0){
			//LED
			for (i = 0 ; i < 3; i ++)
			{
				PBout(8) = 0;delay_ms(20);
				PBout(8) = 1;delay_ms(20);	
			}			
			if (ret==0x1244FF01)
				value(5,33-5,100);
			if (ret==0x1244FF02)
				value(10,33-10,100);
			if (ret==0x1244FF03)
				value(15,33-15,100);
			if (ret==0x1244FF04)
				value(20,33-20,100);
			if (ret==0x1244FF05)
				value(25,33-25,100);
		}
		else{
			for (i = 0 ; i < 3; i ++)
			{
				PBout(8) = 0;delay_ms(30);
				PBout(8) = 1;delay_ms(10);	
			}
		}
	}
}

void IoT_Post(float tmp)
{
	printf("AT\r\n");
	delay_ms(500);
	//printf("AT+CWJAP=\"TP-LINK_3000\",\"1521521521\"\r\n");
	printf("AT+CIPMUX=0\r\n");
	delay_ms(500);
	printf("AT+CIPMODE=1\r\n");
	delay_ms(500);
	printf("AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n");
	delay_ms(500);
	printf("AT+CIPSEND\r\n");
	delay_ms(500);	
	printf("POST /devices/11392600/datapoints?type=3 HTTP/1.1\n");
	printf("api-key:SCjkvjG8SLEgsAQoMkZqOl8VpsU=\n");
	printf("Host:api.heclouds.com\n");
	printf("Content-Length:38\n");
	printf("\n");
	//printf("{\"Temperature\":29.14,\"Humidity\":74.61}\n",tmp);
	printf("{\"Temperature\":%2.2f,\"Humidity\":74.61}",tmp);
	delay_ms(1000);
	printf("+++");
	delay_ms(500);
}

void IoT_Post_d(float tmp)
{
	printf("AT\r\n");
	delay_ms(500);
	//printf("AT+CWJAP=\"TP-LINK_3000\",\"1521521521\"\r\n");
	printf("AT+CIPMUX=0\r\n");
	delay_ms(500);
	printf("AT+CIPMODE=1\r\n");
	delay_ms(500);
	printf("AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n");
	delay_ms(500);
	printf("AT+CIPSEND\r\n");
	delay_ms(500);	
	printf("POST /devices/11392600/datapoints?type=3 HTTP/1.1\n");
	printf("api-key:SCjkvjG8SLEgsAQoMkZqOl8VpsU=\n");
	printf("Host:api.heclouds.com\n");
	printf("Content-Length:40\n");
	printf("\n");
	//printf("{\"Temperature\":29.14,\"Humidity\":74.61}\n",tmp);
	printf("{\"Temperature_d\":%2.2f,\"Humidity\":74.61}",tmp);
	delay_ms(1000);
	printf("+++");
	delay_ms(500);
}

float temperature_test(void)
{
	float t;
	ds18b20_init();
	t = ds18b20_read();
	return t;
}

int pwmtest()
{		
	while(1)
		for( m = 1 ; m < 50; m ++)
			pwm(m,50-m,200);
		
	while(1)
		for( m = 1 ; m < 5; m ++)
			pwm(m,m,200);
		
	while (1)
	{				
		pwm(2,2,100);
		delay_ms(50);
		pwm(2,2,100);
		delay_ms(50);
		pwm(2,2,100);
		delay_ms(50);
		pwm(1,1,200);
		delay_ms(50);		
	}
}	

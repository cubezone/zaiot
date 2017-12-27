#include "pwm.h"

static int GUA_Infrared_Receiver_GetHighLevelTime(void)  
{  
    int nGUA_Num = 0;  
      
    //??????????  
    while(PBin(0) == 1)  
    {  
        //??????  
        if(nGUA_Num >= 250)   
        {  
            return nGUA_Num;  
        }  
      
        //???20us???  
        nGUA_Num++;  
          
        //??  
        delay_us(20);             
    }  
      
    return nGUA_Num;  
}  

static int GUA_Infrared_Receiver_GetLowLevelTime(void)  
{  
    int nGUA_Num = 0;  
      
    //??????????  
    while(PBin(0) == 0)  
    {  
        //??????  
        if(nGUA_Num >= 500)   
        {  
            return nGUA_Num;  
        }  
      
        //???20us???  
        nGUA_Num++;  
          
        //??  
        delay_us(20);             
    }  
      
    return nGUA_Num;  
}  

int gGUA_InfraredReceiver_Data;

int GUA_Infrared_Receiver_Process(void)  
{  
    int nGUA_Time_Num = 0;  
    int nGUA_Data = 0;  
    int nGUA_Byte_Num = 0;  
    int nGUA_Bit_Num = 0;      
      
    //?????9ms????,??????>10ms?<8ms  
    nGUA_Time_Num = GUA_Infrared_Receiver_GetLowLevelTime();  
    if((nGUA_Time_Num >= 500) || (nGUA_Time_Num <= 400))  
    {  
        return -1;  
    }     
      
    //?????4.5ms????,??????>5ms?<4ms  
    nGUA_Time_Num = GUA_Infrared_Receiver_GetHighLevelTime();  
    if((nGUA_Time_Num >= 250) || (nGUA_Time_Num <= 200))  
    {  
        return -2;  
    }     
              
    //???4????  
    for(nGUA_Byte_Num = 0; nGUA_Byte_Num < 4; nGUA_Byte_Num++)  
    {  
        //?????8???  
        for(nGUA_Bit_Num = 0; nGUA_Bit_Num < 8; nGUA_Bit_Num++)  
        {  
            //???bit??0.56ms????,??????>1.2ms?<0.40ms  
            nGUA_Time_Num = GUA_Infrared_Receiver_GetLowLevelTime();  
            if((nGUA_Time_Num >= 60) || (nGUA_Time_Num <= 20))  
            {  
                return -3;  
            }     
  
            //???bit???????:?????,1.68ms(1.2ms~2.0ms),?????,0.56ms(0.2ms~1ms),????????  
            nGUA_Time_Num = GUA_Infrared_Receiver_GetHighLevelTime();  
            if((nGUA_Time_Num >=60) && (nGUA_Time_Num < 100))  
            {  
                nGUA_Data = 1;  
            }             
            else if((nGUA_Time_Num >=10) && (nGUA_Time_Num < 50))  
            {  
                nGUA_Data = 0;  
            }  
            else
            {  
                return -4;  
            }  
  
            //????  
            gGUA_InfraredReceiver_Data <<= 1;   
            gGUA_InfraredReceiver_Data |= nGUA_Data;                          
        }  
    }   
 
    return 0;      
}  

void pwm (int a, int b, int len)
{
	int i = 0;
	len = len/(a+b);
	for (i = 0 ; i < len ; i++)
	{
		PBout(1) = 0;
		delay_ms(a);
		PBout(1) = 1;
		delay_ms(b);
	}
}


void value (int a, int b, int len)
{
	int i = 0;
	len = len/(a+b)*1000;
	for (i = 0 ; i < len ; i++)
	{
		PBout(2) = 0;
		delay_us(a);
		PBout(2) = 1;
		delay_us(b);
	}
}

int  infar_test(void)
{
	int ret =0;
	 while (PBin(0) == 1)
		 delay_us(1);
	 ret = GUA_Infrared_Receiver_Process();	 
	 return ret;
}


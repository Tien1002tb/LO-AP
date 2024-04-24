
#include "step.h"

//IN4: PB15  d
//IN3: PB14  c
//IN2: PB13  b
//IN1: PB12  a
void SetMotor(unsigned char InputData);
void delayMs(uint32_t ms);

u8 forward[4] = {0x03,0x06,0x0c,0x09}; 
u8 reverse[4]= {0x03,0x09,0x0c,0x06}; 
//GPIO_InitTypeDef GPIO_InitStructure;

//void Step_Motor_GPIO_Init(void)
//{  
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//}


void SetMotor(unsigned char InputData)
{
	if(InputData == 0x03)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		GPIO_SetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
	}
	else if(InputData == 0x06)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		GPIO_SetBits(GPIOB,GPIO_Pin_13);
		GPIO_SetBits(GPIOB,GPIO_Pin_14);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
	}
	else if(InputData == 0x09)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		GPIO_SetBits(GPIOB,GPIO_Pin_15);
	}
	else if(InputData == 0x0c)
	{	
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		GPIO_SetBits(GPIOB,GPIO_Pin_14);
		GPIO_SetBits(GPIOB,GPIO_Pin_15);
	}
	else if(InputData == 0x00)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
	}
}

void motor_circle(int n, int direction, int delay)
{
    int i, j;
    for(i = 0; i < n * 8; i++)
    {
		for(j = 0; j < 4; j++)
		{
			if(1 == direction)
			{
				SetMotor(0x00);
				SetMotor(forward[j]);
			}
			else
			{
				SetMotor(0x00);
				SetMotor(reverse[j]);
			}
			
			delayMs(delay > 2 ? delay : 2);
		}
    }
}
void delayMs(uint32_t ms) {
    uint32_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 7200; j++); // Delay loop
    }
}

#include "dht11.h"

uint8_t DHT11_Init(void)
{
	RCC->APB2ENR |= (1 << 2);
	DHT11_Pin_Out();
	GPIOA->ODR |= (1 << DHT11_Pin);
	DHT11_Rst();
	return DHT11_Check();
}

void DHT11_Pin_Out(void)
{
	GPIOA->CRL &= 0XFFFFFFF0;
	GPIOA->CRL |= 3 << 0;
}

void DHT11_Pin_In(void)
{
	GPIOA->CRL &= 0XFFFFFFF0;
	GPIOA->CRL |= 8 << 0;
}

void DHT11_Rst(void)
{
	DHT11_Pin_Out();
	GPIOA->ODR &= ~(1 << DHT11_Pin);
  DelayMs(20);
	GPIOA->ODR |= (1 << DHT11_Pin);
	DelayUs(30);
}

uint8_t DHT11_Check(void)
{
	uint8_t time = 0;
	DHT11_Pin_In();
	while((GPIOA->IDR & (1 << DHT11_Pin)) && time < 100)
	{
		time++;
		DelayUs(1);
	}
	if(time >= 100) return 1;
	else time = 0;
	while(!(GPIOA->IDR & (1 << DHT11_Pin)) && time < 100)
	{
		time++;
		DelayUs(1);
	}
	if(time >= 100) return 1;
	else return 0;
}

uint8_t DHT11_Read_Bit(void)
{
	uint8_t time = 0;
	while((GPIOA->IDR & (1 << DHT11_Pin)) && time < 100)
	{
		time++;
		DelayUs(1);
	}
	time = 0;
	while(!(GPIOA->IDR & (1 << DHT11_Pin)) && time < 100)
	{
		time++;
		DelayUs(1);
	}
	DelayUs(40);
	if((GPIOA->IDR & (1 << DHT11_Pin))) return 1;
	else return 0;
}

uint8_t DHT11_Read_Byte(void)
{
	uint8_t i = 0;
	uint8_t data = 0;
	for(i = 0; i < 8; i++)
	{
		data <<= 1;
		data |= DHT11_Read_Bit();
	}
	return data;
}

uint8_t DHT11_Read_Data(uint8_t *RHI, uint8_t *RHD, uint8_t *TCI, uint8_t *TCD)
{
	uint8_t i = 0;
	uint8_t buf[5];
	
	DHT11_Rst();
	if(DHT11_Check() == 0)
	{
		for(i = 0; i < 5; i++)
		{
			buf[i] = DHT11_Read_Byte();
		}
		if(buf[0] + buf[1] + buf[2] + buf[3] == buf[4])
		{
			*RHI = buf[0];
			*RHD = buf[1];
			*TCI = buf[2];
			*TCD = buf[3];
		}
	}
	else return 1;
	return 0;
}

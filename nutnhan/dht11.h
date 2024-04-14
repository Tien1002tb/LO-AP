#include "systick_time.h"
#include "stm32f10x.h"                  // Device header

#define DHT11_Pin 0 //PA0

uint8_t DHT11_Init(void);
void DHT11_Pin_Out(void);
void DHT11_Pin_In(void);
void DHT11_Rst(void);
uint8_t DHT11_Check(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Data(uint8_t *RHI, uint8_t *RHD, uint8_t *TCI, uint8_t *TCD);



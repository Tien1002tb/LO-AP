#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "step.h"
#include "systick_time.h"
#include "lcd_1602_drive.h"
#include "dht11.h"
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stdio.h"

#define GPIO_PORT GPIOA
#define DEN  GPIO_Pin_2
#define QUAT GPIO_Pin_1
#define CONG GPIO_Pin_3
#define TRU GPIO_Pin_4
#define OK GPIO_Pin_5
GPIO_InitTypeDef GPIO_Structure; 
int congtru_tong =0 ;
uint8_t RHI = 0,RHD = 0 , TCI = 0, TCD = 0;
float RHI_set = 50.00, TCI_set = 50.00;
int count = 0;
char Temp1[20], Temp2[20],Temp3[20];
char Humi1[20], Humi2[20],Humi3[20];
float temp , humi;

char 	vrc_Getc , vri_stt = 0;
char vrc_res[100];
int vri_count = 0;

char  data[100];
int  lux = 1, fan_A = 1, fan_B = 1, motor = 1;

void delayMs(uint32_t ms);
void uart_Init(void);
void uart_SendChar(char _chr);

void gpio_Init(void);
void mannhietdo(void);
void mansetting1(void);
void mansetting2(void);
void setting(void);
void uart_SendStr(char *str);
void USART1_IRQHandler(void);
void uart_Init(void);

void truyen_nhan(void *p);
void mainmenu(void *p);
//void daotrung(void *p );

int main(){
	xTaskCreate(mainmenu, (const char*)"USER",128 , NULL, 1, NULL);	
	xTaskCreate(truyen_nhan , (const char*)"USER",128 , NULL, 1, NULL);	
	vTaskStartScheduler(); 
	while(1){		
	}
}

void gpio_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_Structure.GPIO_Pin = DEN | QUAT; // cau hinh chan gpio den
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
	
	GPIO_Structure.GPIO_Pin = CONG | TRU | OK ; // nut
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
}

void mainmenu(void *p){
	systick_init();
	DHT11_Init();
	gpio_Init();
	lcd_i2c_init(1);
	uint8_t button_state_ok = 1 ;
	uint8_t last_button_state_1 ;
	while(1){	
		last_button_state_1 = button_state_ok;
	  button_state_ok = GPIO_ReadInputDataBit(GPIO_PORT, OK);				
        if (button_state_ok == 0 && last_button_state_1 == 1) {
						congtru_tong ++;			
            if (congtru_tong == 3) {
                congtru_tong = 0;
            }					
							lcd_i2c_cmd(1, 0x01); // Clear Display
							DelayMs(100); // Delay for debounce									
					}   
				
				if(congtru_tong == 0){
							lcd_i2c_msg(1 ,1, 0, " MAN HINH CHINH");
							lcd_i2c_msg(1 ,2, 3, "PRESS OK >>");				
					}
						else if(congtru_tong == 1){
							mannhietdo();		
					}
						else if(congtru_tong == 2){
							setting();
					}
				}	
		}

void mannhietdo(void){
		DHT11_Read_Data(&RHI, &RHD, &TCI, &TCD);
		temp = TCI + (float)TCD / 100;
		humi = RHI + (float)RHD / 100;
		sprintf(Temp1 ,"T:%.2f *C", temp); 
		lcd_i2c_msg(1 ,1, 0, Temp1);
		sprintf(Humi1 ,"H:%.2f %%  OK >>" ,humi);
		lcd_i2c_msg(1 ,2, 0, Humi1);
		DelayMs(50);		
	}
void mansetting1(void){
		sprintf(Temp2 ,"SET >T:%.2f  ", TCI_set);
		lcd_i2c_msg(1 ,1, 0, Temp2);
		lcd_i2c_msg(1 ,1, 13, "*C");
		sprintf(Humi1 ,"H:%.2f %% " ,RHI_set);
		lcd_i2c_msg(1 ,2, 5, Humi1);
		DelayMs(50);
}
void setting(void){
		mansetting1();   
		if (congtru_tong == 2 && GPIO_ReadInputDataBit(GPIO_PORT, CONG) == 0){ // nut tang nhiet do 
				TCI_set = TCI_set + 0.1;
					if (TCI_set >= 90){
							TCI_set = 0;
					}		
			}
		else if (congtru_tong == 2 && GPIO_ReadInputDataBit(GPIO_PORT, TRU) == 0){ // nut giam nhiet do 
				TCI_set = TCI_set - 0.5;
					if (TCI_set == 0){
						TCI_set = 90;
					}
			}
		if (temp < TCI_set){
			GPIO_SetBits(GPIO_PORT, DEN);
			GPIO_ResetBits(GPIO_PORT, QUAT);
		}
	else if (temp > TCI_set){
			GPIO_ResetBits (GPIO_PORT, DEN);
			GPIO_SetBits (GPIO_PORT, QUAT);
		}
}
//UART truyen nhan data

void truyen_nhan(void *p){
	uart_Init();
	sprintf(data, "%.2f-%.2f-%d-%d-%d-%d\n", temp, humi, lux, fan_A, fan_B, motor);
	uart_SendStr(data);
	delayMs(1000);	
}

void uart_SendChar(char _chr){
	USART_SendData(USART1,_chr);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET);
}

void uart_SendStr(char *str){
	while(*str != NULL){
		uart_SendChar(*str++);		
	}
}

uint16_t UARTx_Getc(USART_TypeDef* USARTx){
	return USART_ReceiveData(USARTx);
}

void USART1_IRQHandler(void) {
 if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        vrc_Getc = UARTx_Getc(USART1);	
				if(vrc_Getc == 'T'){
					vri_stt = 1;

				}
				if(vrc_Getc == 'M'){
					vri_stt = 2;
				}
				else{
					vrc_res[vri_count] = vrc_Getc;
					vri_count++;
					
				}
				if(vri_stt == 1){
					uart_SendStr(vrc_res);
					vrc_res[vri_count] = NULL;
					vri_count = 0;
					vri_stt = 0;
				}
				if(vri_stt == 2){
					uart_SendStr(vrc_res);
					vrc_res[vri_count] = NULL;
					vri_count = 0;
					vri_stt = 0;
				}
				
		}
 }

void uart_Init(void){
	USART_InitTypeDef usart_typedef;
	// enable clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	// congifgure pin Tx - A9;
	GPIO_Structure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
	// configure pin Rx - A10;
	GPIO_Structure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);
	// usart configure
	usart_typedef.USART_BaudRate = 9600;
	usart_typedef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_typedef.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	usart_typedef.USART_Parity = USART_Parity_No;
	usart_typedef.USART_StopBits = USART_StopBits_1;
	usart_typedef.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &usart_typedef);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);
	USART_Cmd(USART1, ENABLE);
}


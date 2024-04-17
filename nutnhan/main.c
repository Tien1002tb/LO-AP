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

void gpio_Init(void);
void mainmenu(void);
void mannhietdo(void);
void mansetting1(void);
void mansetting2(void);
void setting(void);
void active (void);

int main(){
	systick_init();// initialize the delay function (Must initialize)
	lcd_i2c_init(1);
	DHT11_Init();
	gpio_Init();
	
	while(1){		
		mainmenu();
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

void mainmenu(void){
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
							lcd_i2c_msg(1 ,1, 0, "MAN HINH CHINH");
							lcd_i2c_msg(1 ,2, 0, ">TIEP");				
					}
					else if(congtru_tong == 1){
							mannhietdo();		
					}
					else if(congtru_tong == 2){
							setting();
					}
				}		
		}

//void mannhietdo(void){
//	DHT11_Read_Data(&RHI, &RHD, &TCI, &TCD);
//	sprintf(Temp1 ,"T:%02u *C",TCI);
//	lcd_i2c_msg(1 ,1, 0, Temp1);
//	sprintf(Humi1 ,"H:%02u %% >" ,RHI);
//	lcd_i2c_msg(1 ,2, 0, Humi1);
//	DelayMs(50);
//	}
//void mansetting1(void){
//	sprintf(Temp2 ,"SET >T:%02u  ", TCI_set);
//	lcd_i2c_msg(1 ,1, 0, Temp2);
//	lcd_i2c_msg(1 ,1, 10, "*C");
//	sprintf(Humi1 ,"H:%02u %% " ,RHI);
//	lcd_i2c_msg(1 ,2, 5, Humi1);
//	DelayMs(50);
//}


//void setting(void){
//		mansetting1();   
//		if (congtru_tong == 2 && GPIO_ReadInputDataBit(GPIO_PORT, CONG) == 0){ // nut tang nhiet do 
//				TCI_set = TCI_set + 1;
//					if (TCI_set >= 90){
//							TCI_set = 0;
//					}		
//			}
//		else if (congtru_tong == 2 && GPIO_ReadInputDataBit(GPIO_PORT, TRU) == 0){ // nut giam nhiet do 
//				TCI_set = TCI_set - 1;
//					if (TCI_set == 0){
//						TCI_set = 90;
//					}
//			}
//		if (TCI < TCI_set){
//			GPIO_SetBits(GPIO_PORT, DEN);
//			GPIO_ResetBits(GPIO_PORT, QUAT);
//		}
//	else if (TCI > TCI_set){
//			GPIO_ResetBits (GPIO_PORT, DEN);
//			GPIO_SetBits (GPIO_PORT, QUAT);
//		}
//}


//void mannhietdo(void){
//	DHT11_Read_Data(&RHI, &RHD, &TCI, &TCD);
//	sprintf(Temp1 ,"T:%02u *C",TCI);
//	lcd_i2c_msg(1 ,1, 0, Temp1);
//	sprintf(Humi1 ,"H:%02u %% >" ,RHI);
//	lcd_i2c_msg(1 ,2, 0, Humi1);
//	DelayMs(50);
//	}
//void mansetting1(void){
//	sprintf(Temp2 ,"SET >T:%02u  ", TCI_set);
//	lcd_i2c_msg(1 ,1, 0, Temp2);
//	lcd_i2c_msg(1 ,1, 10, "*C");
//	sprintf(Humi1 ,"H:%02u %% " ,RHI);
//	lcd_i2c_msg(1 ,2, 5, Humi1);
//	DelayMs(50);
//}

void mannhietdo(void){
	DHT11_Read_Data(&RHI, &RHD, &TCI, &TCD);
 temp = TCI + (float)TCD / 100;
	humi = RHI + (float)RHD / 100;
	sprintf(Temp1 ,"T:%.2f *C", temp); 
	lcd_i2c_msg(1 ,1, 0, Temp1);
	sprintf(Humi1 ,"H:%.2f %% >" ,humi);
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

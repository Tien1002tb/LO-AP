#include "systick_time.h"
#include "lcd_1602_drive.h"
#include "dht11.h"
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stdio.h"

#define GPIO_PORT GPIOA
#define DEN  GPIO_Pin_0
#define QUAT GPIO_Pin_1
#define CONG GPIO_Pin_3
#define TRU GPIO_Pin_4
#define OK GPIO_Pin_5
#define BACK GPIO_Pin_6
GPIO_InitTypeDef GPIO_Structure; 
int congtru_tong = 0;
int tong;
uint8_t RHI = 0,RHD = 0 , TCI = 0, TCD = 0;
u8 RHI_set = 0, TCI_set = 0;
char Temp1[20], Temp2[20];
char Humi1[20], Humi2[20];
void state(void);
void gpio_Init(void);
void mainmenu(void);
void mannhietdo(void);
void mansetting1(void);
void mansetting2(void);
void Setting(void);

int main(){
	systick_init();// initialize the delay function (Must initialize)
	lcd_i2c_init(1);
	DHT11_Init();
	gpio_Init();
	while(1){
		mainmenu();
		Setting();
	}
}
void gpio_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_Structure.GPIO_Pin = DEN | QUAT; // cau hinh chan gpio den
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
	
	GPIO_Structure.GPIO_Pin = CONG | TRU | OK | BACK; // nut
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Structure);	
}
void mainmenu(void){
	while(1){
	 u8 last_button_state;
	 uint8_t button_state_ok = GPIO_ReadInputDataBit(GPIO_PORT, OK);
   uint8_t button_state_back = GPIO_ReadInputDataBit(GPIO_PORT, BACK);

        if (button_state_ok == 0 && last_button_state == 1) {
            congtru_tong = congtru_tong + 1;
					
            if (congtru_tong >= 3) {
                congtru_tong = 3;
            }
           lcd_i2c_cmd(1, 0x01); // Clear Display
            DelayMs(100); // Delay for debounce
						state();
        }

        if (button_state_back == 0 && last_button_state == 1) {
            congtru_tong = congtru_tong - 1;
            if (congtru_tong <= 0) {
                congtru_tong = 0;
            } 
						lcd_i2c_cmd(1, 0x01); // Clear Display
            DelayMs(100); // Delay for debounce
						state();
        }
        last_button_state = button_state_ok | button_state_back;
			}
		}

void state(void){
	switch(congtru_tong){
		case 0:
//			lcd_i2c_cmd(1, 0x01); // Clear Display
			lcd_i2c_msg(1 ,1, 0, "MAN HINH CHINH");
			lcd_i2c_msg(1 ,2, 0, ">TIEP");	
			break;
		case 1:
//			lcd_i2c_cmd(1, 0x01); // Clear Display
			mannhietdo();
		break;
		case 2:
//			lcd_i2c_cmd(1, 0x01); // Clear Display
			mansetting1();
		break;
		case 3:	
//			lcd_i2c_cmd(1, 0x01); // Clear Display			
			mansetting2();
		break;
		default:
			break;
		}
	}
void mannhietdo(void){
	sprintf(Temp1 ,"T:%u *C",TCI);
	lcd_i2c_msg(1 ,1, 0, Temp1);
	sprintf(Humi1 ,"H:%u %% >" ,RHI);
	lcd_i2c_msg(1 ,2, 0, Humi1);
	DelayMs(50);
	}
void mansetting1(void){

	sprintf(Temp1 ,"SET >T:%u *C", TCI_set);
	lcd_i2c_msg(1 ,1, 0, Temp2);
	sprintf(Humi1 ,"H:%u %%",RHI_set);
	lcd_i2c_msg(1 ,2, 5, Humi2);
	DelayMs(50);
}

void mansetting2(void){
	sprintf(Temp1 ,"SET T:%u *C", TCI_set);
	lcd_i2c_msg(1 ,1, 0, Temp2);
	sprintf(Humi1 ,">H:%u %%",RHI_set);
	lcd_i2c_msg(1 ,2, 5, Humi2);
	DelayMs(50);
	
}
void Setting(void){

	while(1){
	if (congtru_tong == 2){
		if (GPIO_ReadInputDataBit(GPIO_PORT, CONG) == 0){ // nut tang nhiet do 
				TCI_set = TCI_set + 1;
					if (TCI_set == 90){
							TCI_set = 90;
					}
			}
		if (GPIO_ReadInputDataBit(GPIO_PORT, TRU) == 0){ // nut giam nhiet do 
				TCI_set = TCI_set - 1;
					if (TCI_set == 0){
						TCI_set = 0;
					}
			}
		}
	if(congtru_tong == 3){
		if (GPIO_ReadInputDataBit(GPIO_PORT, CONG) == 0){ // nut tang do am
				RHI_set = RHI_set + 1;
					if (RHI_set == 100){
						RHI_set = 100;
					}
			}
		if (GPIO_ReadInputDataBit(GPIO_PORT, TRU) == 0){ // nut giam do am
				RHI_set = RHI_set - 1;
					if (RHI_set == 0){
						RHI_set = 0;
					}
			}
	}
	}
}

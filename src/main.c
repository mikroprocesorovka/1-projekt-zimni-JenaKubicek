#include "stm8s.h"
#include "milis.h"
#include "stm8_hd44780.h"
#include "spse_stm8.h"
#include "delay.h"
#include <stdio.h>
#include "uart1.h"
#define _ISOC99_SOURCE
#define _GNU_SOURCE
#define BTN_PORT GPIOE
#define BTN_PIN  GPIO_PIN_4
#define BTN_PUSH (GPIO_ReadInputPin(BTN_PORT, BTN_PIN)==RESET)

void ADC_init(void){
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL2,DISABLE); //PB2
ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4); // nastavíme clock pro ADC (16MHz / 4 = 4MHz)
ADC2_AlignConfig(ADC2_ALIGN_RIGHT); // volíme zarovnání výsledku vpravo
ADC2_Select_Channel(ADC2_CHANNEL_2); // nasatvíme multiplexer na některý ze vstupních kanálů
ADC2_Cmd(ENABLE); // rozběhneme AD převodník
ADC2_Startup_Wait(); // počkáme než se AD převodník rozběhne (~7us)
}
void TIM2_setup(void){
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_1, 640 - 1);//25kHz    
    TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 400, 
                TIM2_OCPOLARITY_HIGH); // PD4
    TIM2_Cmd(ENABLE);
    TIM2_OC2PreloadConfig(ENABLE);

}
void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    init_milis();
    lcd_init();
    ADC_init();
    TIM2_setup();
    init_uart1();
}


int main(void){
    init();
    uint32_t timeA = 0;
    uint16_t timeB = 0;
    uint16_t adc_value;
    char text[32];
    char text2[32];
    uint16_t voltage;
    int16_t teplota_1_cast = 0;
    int16_t teplota_2_cast = 0;
    uint8_t rychlost;

    while (1) {
        if (milis() - timeA > 333){
            timeA=milis();
            adc_value = ADC_get(ADC2_CHANNEL_2); // do adc_value ulož výsledek převodu vstupu ADC_IN2 (PB2)
            voltage = ((uint32_t)adc_value*5000 + 512)/1024;
            teplota_1_cast=voltage/10;//celočíselná část
            teplota_2_cast=voltage%10;//desetinná část
            lcd_gotoxy(0, 0);
            sprintf(text,"Teplota = %2u %1u C",teplota_1_cast,teplota_2_cast);
            lcd_puts(text);
            lcd_gotoxy(1,0);
            sprintf(text2,"Rychlost = %3u %",rychlost);
            lcd_puts(text2);
        }
        if(milis() - timeB > 1500){
            if (teplota_1_cast < 25){
                TIM2_SetCompare1(0);
                rychlost = 0;
            }
            if(teplota_1_cast > 25){
                TIM2_SetCompare1(128);
                rychlost = 20;
            }
            if(teplota_1_cast > 30){
                TIM2_SetCompare1(256);
                rychlost = 40;
            }
            if(teplota_1_cast > 35){
                TIM2_SetCompare1(320);
                rychlost = 50;
            }
            if(teplota_1_cast > 40){
                TIM2_SetCompare1(384);
                rychlost = 60;
            }
            if(teplota_1_cast > 45){
                TIM2_SetCompare1(448);
                rychlost = 70;
            }
            if(teplota_1_cast > 50){
                TIM2_SetCompare1(512);
                rychlost = 80;
            }
            if(teplota_1_cast > 55){
                TIM2_SetCompare1(576);
                rychlost = 90;
            }
            if(teplota_1_cast > 60){
                TIM2_SetCompare1(639);
                rychlost = 100;
            }
            timeB=milis();
        }
    }
}
/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"

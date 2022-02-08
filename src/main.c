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
#define TACH_PORT GPIOD
#define TACH_PIN GPIO_PIN_6
#define TACH_GET GPIO_ReadInputPin(TACH_PORT, TACH_PIN)!=RESET

void ADC_init(void){
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL2,DISABLE); //PB6
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
                TIM2_OCPOLARITY_HIGH);
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
    GPIO_Init(TACH_PORT, TACH_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
}


int main(void)
{
    init();
    uint32_t timeA = 0;
    uint16_t timeB = 0;
    uint16_t timeC = 0;
    uint16_t timeD = 0;
    uint16_t adc_value;
    char text[32];
    char text2[];
    uint16_t voltage;
    uint16_t RPM;
    int16_t teplota_1_cast = 0;
    int16_t teplota_2_cast = 0;
    static int minule = 0;
    static int impulz1 = 0;
    static int impulz2 = 0;
    static int otocka = 0;


    while (1) {
        if (milis() - timeA > 333){
            timeA=milis();
            adc_value = ADC_get(ADC2_CHANNEL_2); // do adc_value ulož výsledek převodu vstupu ADC_IN6 (PB6)
            voltage = ((uint32_t)adc_value*5000 + 512)/1024;
            teplota_1_cast=voltage/10;//celočíselná část
            teplota_2_cast=voltage%10;//desetinná část
            lcd_gotoxy(0, 0);
            sprintf(text,"Teplota = %2u %1u C",teplota_1_cast,teplota_2_cast);
            lcd_puts(text);
            printf("Teplota = %2u,%1u C\n\r",teplota_1_cast,teplota_2_cast);
        }
       /* if(milis() - timeB > 1500){
            if (teplota_1_cast < 25){
                TIM2_SetCompare1(0);
            }
            if(teplota_1_cast > 25){
                TIM2_SetCompare1(100);
            }
            if(teplota_1_cast > 30){
                TIM2_SetCompare1(200);
            }
            if(teplota_1_cast > 35){
                TIM2_SetCompare1(300);
            }
            if(teplota_1_cast > 40){
                TIM2_SetCompare1(400);
            }
            if(teplota_1_cast > 45){
                TIM2_SetCompare1(500);
            }
            if(teplota_1_cast > 50){
                TIM2_SetCompare1(639);
            }
            timeB=milis();
        }*/
        if (milis() - timeC > 2){
            if (TACH_GET && minule == 0 && impulz2 == 0){
                minule = 1;
                impulz1 = 1;
                impulz2 = 0;
            }
            if (GPIO_ReadInputPin(TACH_PORT, TACH_PIN)==RESET){
                minule = 0;
            }
            if (TACH_GET && minule == 0 && impulz1 == 1){
                impulz2 = 1;
            }
            if (impulz1 && impulz2){
                otocka+=1;
                impulz1 = 0;
                impulz2 = 0;
            }
            timeC = milis();
        }
        if (milis() - timeD > 10000){
            RPM = otocka*6;
            lcd_gotoxy(1,0);
            sprintf(text2,"RPM = %5u",RPM);
            lcd_puts(text2);
            printf("RPM = %5u",RPM);
            timeD=milis();
            otocka=0;
        }

    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"

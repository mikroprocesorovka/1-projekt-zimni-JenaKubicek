#include "stm8s.h"
#include "milis.h"
#include "stm8_hd44780.h"
#include "spse_stm8.h"
#include "delay.h"
#include <stdio.h>
#define _ISOC99_SOURCE
#define _GNU_SOURCE

void ADC_init(void){
// na pinech/vstupech ADC_IN2 (PB2) a ADC_IN3 (PB3) vypneme vstupní buffer
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL2,DISABLE);
// nastavíme clock pro ADC (16MHz / 4 = 4MHz)
ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);
// volíme zarovnání výsledku (typicky vpravo, jen vyjmečně je výhodné vlevo)
ADC2_AlignConfig(ADC2_ALIGN_RIGHT);
// nasatvíme multiplexer na některý ze vstupních kanálů
ADC2_Select_Channel(ADC2_CHANNEL_2);
// rozběhneme AD převodník
ADC2_Cmd(ENABLE);
// počkáme než se AD převodník rozběhne (~7us)
ADC2_Startup_Wait();
}
void TIM2_setup(void){
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_1, 640 - 1);//25kHz    
    TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 1, 
                TIM2_OCPOLARITY_LOW);
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
}


int main(void)
{
    init();
    uint32_t timeA = 0;
    uint16_t adc_value;
    char text[32];
    int16_t teplota;
    int16_t teplota_1_cast;
    int16_t teplota_2_cast;

    while (1) {
        if (milis()-timeA>100){
            timeA=milis();
            adc_value = ADC_get(ADC2_CHANNEL_2); // do adc_value ulož výsledek převodu vstupu ADC_IN2 (PB2)
            teplota = ((uint32_t)adc_value*5000 + 512)/1024;
            teplota_1_cast=teplota/10;//celočíselná část
            teplota_2_cast=teplota%10;//desetinná část

        }
        lcd_gotoxy(0, 0);
        sprintf(text,"Teplota = %2u %1u C",teplota_1_cast,teplota_2_cast);
        lcd_puts(text);
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"

#include <msp430.h>

void ConfigPinos(){                              //Configuração dos pinos I/O
    P1DIR |= BIT0 + BIT1;
    P1OUT &= ~BIT0;
    P1SEL1 |= BIT1;
}

void ConfigTimerA(){                            //Configuração do Timer A
    TA0CCR0 = 1000-1;                           // Periodo do PWM
    TA0CCTL1 = OUTMOD_7;                        // Modo de saída reset/set
    TA0CCR1 = 990;                               // PWM duty cycle
    TA0CTL = TASSEL_2 | MC__UP|ID_3|TACLR;      // SMCLK, up mode, clear TAR
}

void ConfigADC10(){
    SYSCFG2 |= ADCPCTL0;                       //Configura A0, A6 e A7
    ADCCTL0 |= ADCSHT_4 | ADCON;               // ADCON, S&H=64 ADC clks
    ADCCTL1 |= ADCSHP|ADCSSEL_1;               // ADCCLK = ACLK; sampling timer
    ADCCTL2 |= ADCRES;                         // 10-bit conversion results
    ADCMCTL0 |= ADCINCH_0;                     // A0 ADC input select; Vref=AVCC
    ADCCTL0 |= ADCENC;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                  // Desliga Watch Dog-Time
    PM5CTL0 &= ~LOCKLPM5;                      // Desliga modo de alta impedancia dos pinos

//---------------------Configuração de pinos e perifericos------------------------------------
    ConfigPinos();
    ConfigTimerA();
    ConfigADC10();

    unsigned volatile int ADC_Result;

    while(1){
        ADCCTL0 |= ADCSC;                           // Sampling and conversion start
           while(ADCCTL1 & ADCBUSY);
        ADC_Result= ADCMEM0;

        if(ADC_Result>700){
            if(TA0CCR1<=10){
                TA0CCR1=0;
            }else{
                TA0CCR1--;
            }
        }
        if(ADC_Result<600){
            if(TA0CCR1>=990){
                TA0CCR1=990;
            }else{
                TA0CCR1++;
            }
        }
        }

}

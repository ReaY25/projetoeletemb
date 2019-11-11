#include <msp430.h>

void ConfigPinos(){                              //Configuração dos pinos I/O
    P1DIR |= BIT0 + BIT1 + BIT6 + BIT7;
    P1OUT &= ~(BIT0 + BIT6 + BIT7);
    P1SEL1 |= BIT1;
}

void ConfigTimerA(){                            //Configuração do Timer A
    TA0CCR0 = 1000-1;                           // Periodo do PWM
    TA0CCTL1 = OUTMOD_7;                        // Modo de saída reset/set
    TA0CCR1 = 10;                               // PWM duty cycle
    TA0CTL = TASSEL_2 | MC__UP|ID_3|TACLR;      // SMCLK, up mode, clear TAR
}

void ConfigADC10(){
    SYSCFG2 |= ADCPCTL0 + ADCPCTL6 + ADCPCTL7;  //Configura A0, A6 e A7
    ADCCTL0 |= ADCSHT_4 | ADCON;               // ADCON, S&H=64 ADC clks
    ADCCTL1 |= ADCSHP|ADCSSEL_1;               // ADCCLK = ACLK; sampling timer
    ADCCTL2 |= ADCRES;                         // 10-bit conversion results
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Desliga Watch Dog-Time
    PM5CTL0 &= ~LOCKLPM5;                    // Desliga modo de alta impedancia dos pinos

//---------------------Configuração de pinos e perifericos------------------------------------
    ConfigPinos();
    ConfigTimerA();
    ConfigADC10();
    unsigned volatile int ADC_Result[3];
    while(1)
       {
        ADCMCTL0 |= ADCINCH_0;                     // A0 ADC input select; Vref=AVCC
        ADCMCTL0 &= ~(ADCINCH_6|ADCINCH_7);
        ADCCTL0 |= ADCENC;
        ADCCTL0 |= ADCSC;                           // Sampling and conversion start
           while(ADCCTL1 & ADCBUSY);
        ADC_Result[0]= ADCMEM0;
        ADCCTL0 &=~ ADCENC;
        ADCCTL0 &=~ ADCSC;

        ADCMCTL0 |= ADCINCH_6;                     // A0 ADC input select; Vref=AVCC
        ADCMCTL0 &= ~(ADCINCH_0|ADCINCH_7);
        ADCCTL0 |= ADCENC;
        ADCCTL0 |= ADCSC;                           // Sampling and conversion start
            while(ADCCTL1 & ADCBUSY);
        ADC_Result[1]= ADCMEM0;
        ADCCTL0 &=~ ADCENC;
        ADCCTL0 &=~ ADCSC;

        ADCMCTL0 |= ADCINCH_7;                     // A0 ADC input select; Vref=AVCC
        ADCMCTL0 &= ~(ADCINCH_0|ADCINCH_6);
        ADCCTL0 |= ADCENC;
        ADCCTL0 |= ADCSC;                           // Sampling and conversion start
            while(ADCCTL1 & ADCBUSY);
        ADC_Result[0]= ADCMEM0;
        ADCCTL0 &=~ ADCENC;
        ADCCTL0 &=~ ADCSC;
       }

}

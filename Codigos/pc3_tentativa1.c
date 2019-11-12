#include <msp430.h>

volatile int ADC_Result,ADC_Ref,duty_cycle;

void ConfigPinos(){                              //Configuração dos pinos I/O
    P1DIR |= BIT0 + BIT1;
    P1OUT &= ~BIT0;
    P1SEL1 |= BIT1;


    P2DIR &= ~BIT3;
    P2REN |= BIT3;
    P2OUT |= BIT3;
    P2IE = BIT3;
    P2IES = BIT3;
    P2IFG = 0x00;
    _enable_interrupts();
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
//------Achando valor de referencia----------

    ADCCTL0 |= ADCSC;
    ADC_Ref= ADCMEM0;

    while(1){
        ADCCTL0 |= ADCSC;                           // Sampling and conversion start
        while(ADCCTL1 & ADCBUSY);
        ADC_Result= ADCMEM0;

        if((ADC_Ref-ADC_Result)>=100){
            if(TA0CCR1<=10){
                TA0CCR1=0;
            }else{
                TA0CCR1--;
            }
        }
        if((ADC_Ref-ADC_Result)<=50){
            if(TA0CCR1>=990){
                TA0CCR1=990;
            }else{
                TA0CCR1++;
            }
        }
    duty_cycle=TA0CCR1;
    }

}
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void){
    ADCCTL0 |= ADCSC;
    ADC_Ref= ADCMEM0;
    P2IFG = 0x00;
}

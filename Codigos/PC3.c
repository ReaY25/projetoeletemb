#include <msp430.h>

float ADC_Result[3],ADC_Ref[3],duty_cycle;
float media_result, media_ref;

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
    TA0CCR1 = 0;                               // PWM duty cycle
    TA0CTL = TASSEL_2 | MC__UP|ID_3|TACLR;      // SMCLK, up mode, clear TAR
}

void ConfigADC10(){
    SYSCFG2 |= ADCPCTL0 + ADCPCTL6 + ADCPCTL7;  //Configura A0, A6 e A7
    ADCCTL0 |= ADCSHT_4 | ADCON;               // ADCON, S&H=64 ADC clks
    ADCCTL1 |= ADCSHP|ADCSSEL_1;               // ADCCLK = ACLK; sampling timer
    ADCCTL2 |= ADCRES;                         // 10-bit conversion results
}

float media_ADC(float vetor[]){
    int i;
    float media;
    for(i=0;i<=2;i++){
        media+=vetor[i];
    }
    return media/3;
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
    ADCMCTL0 |= ADCINCH_0;                     // A0 ADC input select; Vref=AVCC
    ADCMCTL0 &= ~(ADCINCH_6|ADCINCH_7);
    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;                           // Sampling and conversion start
    while(ADCCTL1 & ADCBUSY);
    ADC_Ref[0]= ADCMEM0;
    ADCCTL0 &=~ ADCENC;
    ADCCTL0 &=~ ADCSC;

    ADCMCTL0 |= ADCINCH_6;                     // A0 ADC input select; Vref=AVCC
    ADCMCTL0 &= ~(ADCINCH_0|ADCINCH_7);
    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;
     while(ADCCTL1 & ADCBUSY);
    ADC_Ref[1]= ADCMEM0;
    ADCCTL0 &=~ ADCENC;
    ADCCTL0 &=~ ADCSC;

    ADCMCTL0 |= ADCINCH_7;
    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;
     while(ADCCTL1 & ADCBUSY);
    ADC_Ref[2]= ADCMEM0;
    ADCCTL0 &=~ ADCENC;
    ADCCTL0 &=~ ADCSC;

    media_ref= media_ADC(ADC_Ref);


    while(1){
        ADCMCTL0 |= ADCINCH_0;
        ADCMCTL0 &= ~(ADCINCH_6|ADCINCH_7);
        ADCCTL0 |= ADCENC;
        ADCCTL0 |= ADCSC;
           while(ADCCTL1 & ADCBUSY);
        ADC_Result[0]= ADCMEM0;
        ADCCTL0 &=~ ADCENC;
        ADCCTL0 &=~ ADCSC;

        ADCMCTL0 |= ADCINCH_6;
        ADCCTL0 |= ADCENC;
        ADCCTL0 |= ADCSC;
            while(ADCCTL1 & ADCBUSY);
        ADC_Result[1]= ADCMEM0;
        ADCCTL0 &=~ ADCENC;
        ADCCTL0 &=~ ADCSC;

        ADCMCTL0 |= ADCINCH_7;
        ADCCTL0 |= ADCENC;
        ADCCTL0 |= ADCSC;
            while(ADCCTL1 & ADCBUSY);
        ADC_Result[2]= ADCMEM0;
        ADCCTL0 &=~ ADCENC;
        ADCCTL0 &=~ ADCSC;

        media_result= media_ADC(ADC_Result);

        if((media_ref-media_result)>=150 ){
            if(TA0CCR1<=10){
                TA0CCR1=0;
            }else{
                TA0CCR1-=3;
            }
        } else if((media_ref-media_result)<=50){
            if(TA0CCR1>=990){
                TA0CCR1=990;
            }else{
                TA0CCR1+=3;
            }
        }

    duty_cycle=TA0CCR1;
    }

}
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void){
    ADCMCTL0 |= ADCINCH_0;                     // A0 ADC input select; Vref=AVCC
    ADCMCTL0 &= ~(ADCINCH_6|ADCINCH_7);
    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;                           // Sampling and conversion start
     while(ADCCTL1 & ADCBUSY);
    ADC_Ref[0]= ADCMEM0;
    ADCCTL0 &=~ ADCENC;
    ADCCTL0 &=~ ADCSC;

    ADCMCTL0 |= ADCINCH_6;                     // A0 ADC input select; Vref=AVCC
    ADCMCTL0 &= ~(ADCINCH_0|ADCINCH_7);
    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;
     while(ADCCTL1 & ADCBUSY);
    ADC_Ref[1]= ADCMEM0;
    ADCCTL0 &=~ ADCENC;
    ADCCTL0 &=~ ADCSC;

    ADCMCTL0 |= ADCINCH_7;
    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;
     while(ADCCTL1 & ADCBUSY);
    ADC_Ref[2]= ADCMEM0;
    ADCCTL0 &=~ ADCENC;
    ADCCTL0 &=~ ADCSC;

    media_ref= media_ADC(ADC_Ref);

    P2IFG = 0x00;
}

#include <msp430.h>

int ADC_Result[4],ADC_Ref[4],duty_cycle;
int media_result, media_ref;
extern int media_ADC(int A[]);
void ConfigPinos(){                              //Configuração dos pinos I/O
    P1DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT6;
    P1OUT &= ~BIT0;
    P1SEL1 |= BIT1;


    P2DIR &= ~(BIT3|BIT7);
    P2REN |= (BIT3|BIT7);
    P2OUT |= (BIT3|BIT7);
    P2IE = (BIT3|BIT7);
    P2IES = (BIT3|BIT7);
    P2IFG = 0x00;
    _enable_interrupts();
}

void ConfigTimerA(){                            //Configuração do Timer A
    TA0CTL = TASSEL_2 | MC__UP|TACLR;      // SMCLK, up mode, clear TAR
    TA0CCR0 = 500-1;                           // Periodo do PWM
    TA0CCTL1 = OUTMOD_7;                        // Modo de saída reset/set
    TA0CCR1 = 0;                               // PWM duty cycle

}

void ConfigADC10(){
    ADCCTL0 &= ~ADCENC;
    ADCCTL0 &= ~ADCSC;
    SYSCFG2 |= ADCPCTL0;  //Configura A0, A6 e A7
    SYSCFG2 &= ~ADCPCTL6;
    ADCCTL0 |= ADCSHT_4 | ADCON;               // ADCON, S&H=64 ADC clks
    ADCCTL1 |= ADCSHP|ADCSSEL_1;               // ADCCLK = ACLK; sampling timer
    ADCCTL2 |= ADCRES;                         // 10-bit conversion results
    ADCMCTL0 |= ADCINCH_0;
    ADCMCTL0 &= ~ADCINCH_6;
    ADCCTL0 |= ADCENC;
}

int media_ADC_C(int vetor[]){
    int i;
    int media;
    for(i=0;i<=3;i++){
        media+=vetor[i];
    }
    return media/4;
}
int Ler_Potenciometro(){
    int volatile Valor_Potenciometro;

    ADCCTL0 &= ~ADCENC;
    ADCCTL0 &= ~ADCSC;
    SYSCFG2 |= ADCPCTL6;  //Configura A0, A6 e A7
    SYSCFG2 &=~ ADCPCTL0;
    ADCCTL0 |= ADCSHT_4 | ADCON;               // ADCON, S&H=64 ADC clks
    ADCCTL1 |= ADCSHP|ADCSSEL_1;               // ADCCLK = ACLK; sampling timer
    ADCCTL2 |= ADCRES;                         // 10-bit conversion results
    ADCMCTL0 |= ADCINCH_6;
    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;

        while(ADCCTL1 & ADCBUSY);
    Valor_Potenciometro=ADCMEM0;
    return Valor_Potenciometro;

}
void Ler_sensor(int ADC[]){
            ConfigADC10();
            P1OUT &=~(BIT2+BIT3);
            ADCCTL0 |= ADCSC;
                while(ADCCTL1 & ADCBUSY);
            ADC[0]= ADCMEM0;

            P1OUT|=BIT2;
            P1OUT &=~(BIT3);
            ADCCTL0 |= ADCSC;
                while(ADCCTL1 & ADCBUSY);
            ADC[1]= ADCMEM0;

            P1OUT|=BIT3;
            P1OUT &=~(BIT2);
            ADCCTL0 |= ADCSC;
                while(ADCCTL1 & ADCBUSY);
            ADC[2]= ADCMEM0;

            P1OUT|=(BIT2 + BIT3);
            ADCCTL0 |= ADCSC;
                while(ADCCTL1 & ADCBUSY);
            ADC[3]= ADCMEM0;


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
    Ler_sensor(ADC_Ref);
    media_ref=media_ADC(ADC_Ref) ;


    while(1){
        Ler_sensor(ADC_Result);
        media_result=media_ADC(ADC_Result) ;

         if((media_ref-media_result)>=150 ){
            if(TA0CCR1<=20){
                TA0CCR1=0;
            }else{
                TA0CCR1-=1;
            }
        } else if((media_ref-media_result)<=50){
            if(TA0CCR1>=499){
                TA0CCR1=499;
            }else{
                TA0CCR1+=1;
            }
        }

    duty_cycle=TA0CCR1;
    }

}
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void){

    if(P2IFG & BIT3){
        Ler_sensor(ADC_Ref);
        media_ref= media_ADC_C(ADC_Ref);
    }
    if(P2IFG & BIT7){
        while(1){
            if(Ler_Potenciometro()<=20)TA0CCR1=0;

            TA0CCR1 = Ler_Potenciometro();

            if((P2IN & BIT3)==0){
              media_ref=Ler_Potenciometro();
              break;
          }
        }
    }
    P2IFG = 0x00;
}

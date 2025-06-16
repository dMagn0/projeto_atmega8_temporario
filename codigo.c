/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Advanced
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http:
Chip type : ATmega8
Program type : Application
AVR Core Clock frequency: 14.745600 MHz
Memory model : Small
External RAM size : 0
Data Stack size : 256
*****************************************************/
#include <mega8.h>

#include <stdio.h>
#include <delay.h>
#include "lib/rc522.h"

#define STAND_BY = 0 
#define TRANSICAO_03 = 1 
#define TRANSICAO_30 = 2 
#define RECARGA = 3 
#define TRANSICAO_60 = 4 
#define TRANSICAO_06 = 5 
#define CONFIRMACAO = 6 

typedef struct BancoDeDados{
    uint8_t card_id[5];
    uint16_t card_type;
    uint16_t saldo;
};//8 bit * 5 + 16 bit * 1 + 16 nit *1 = 9 bytes 

char ESTADO_DA_MAQUINA = 0; // 0 = stand-by, 1 = 0 para 3, 2 = 3 para 1, 3 = recarga, 4 = 6 para 1, 5 = 1 para 6, 6 = aguardo confirmacao 

BancoDeDados bancoDeDados[10];

void main(void)
{
    uint8_t card_id[5];
    uint16_t card_type;
    char botao = 0; // not PINC.5
    char leitura = 0; // 1 = pressionado
    

    Func0=In

    PORTB = 0x00;
    DDRB = 0x2C;

    PORTC= 0x00;
    DDRC= 0b00011001;

    PORTD=0x00;
    DDRD=0x00;

    TCCR0=0x00;
    TCNT0=0x00;

    TCCR1A=0x00;
    TCCR1B=0x00;
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;

    ASSR=0x00;
    TCCR2=0x00;
    TCNT2=0x00;
    OCR2=0x00;

    MCUCR=0x00;

    TIMSK=0x00;

    UCSRA=0x00;
    UCSRB=0x18;
    UCSRC=0x86;
    UBRRH=0x00;
    UBRRL=0x5F;

    ACSR=0x80;
    SFIOR=0x00;

    ADCSRA=0x00;

    SPCR=0x5F;
    SPSR=0x00;

    TWCR=0x00;

    delay_ms(1000);
    rc522Init();
    if(rc522IsAttached()){
        printf("RC522 Connected\n");
    }
    else{
        printf("RC522 Not connected\n");
    }
    while (1){

        botao = ~PINC.5;
        leitura = rc522ReadCard(card_id, &card_type);

        if(rc522ReadCard(card_id, &card_type))
        {
            printf("Card [0x%x, 0x%x, 0x%x, 0x%x, 0x%x]\n", card_id[0],card_id[1], card_id[2], card_id[3], card_id[4]);
            /*
                Importante fazer o print pelo menos uma fez pra saber o 
                endereço da tag, caso desejem fazer algoexclusivo para ela.
            */
        


            delay_ms(1000);
        }
        switch (ESTADO_DA_MAQUINA){
            case STAND_BY:
            break;
            case TRANSICAO_03:
            break;
            case TRANSICAO_30:
            break;
            case RECARGA:
            break;
            case TRANSICAO_60:
            break;
            case TRANSICAO_06:
            break;
            case CONFIRMACAO:
            break;
        }
    }
}

ESTADO_DA_MAQUINA = 0; # 0 = stand-by , 1 = cadastro e deposito, 2 = aguardo de confirmacao  
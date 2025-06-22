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
#include <string.h>  
#include "lib/rc522.h"
#include "lib/rc522.c"

#define STAND_BY 0 
#define TRANSICAO_03 1 
#define CONCLUSAO_2B 2 
#define RECARGA 3 
#define CONCLUSAO_3B 4 
#define TRANSICAO_06 5 
#define CONFIRMACAO 6 
#define CARTAO_INVALIDO 7
#define SALDO_INSUFICIENTE 8

#define TAMANHO_DO_BANCO 10

#define LED_FUNCIONAMENTO PORTC.4
#define LED_CONFIRMACAO PORTC.3

typedef struct {
    uint8_t card_id[5];
    uint16_t card_type;
    unsigned char saldo;
} BancoDeDados; //8 bit * 5 + 16 bit * 1 + 8 bit *1 = 7 bytes ~~ 10 byte

unsigned char estadoDaMaquina = STAND_BY; // 0 = stand-by, 1 = 0 para 3, 2 = 3 para 1, 3 = recarga, 4 = 6 para 1, 5 = 1 para 6, 6 = aguardo confirmacao 

BancoDeDados bancoDeDados[TAMANHO_DO_BANCO];
unsigned char numero_cadastrado = 0;

unsigned char consulta_conta_cadastrada(uint8_t *card_id, uint16_t card_type, unsigned char* posicao){
    char i = 0;

    for(i=0;i<numero_cadastrado;i++){
        
        if(card_type == bancoDeDados[i].card_type){

            if(memcmp(card_id, bancoDeDados[i].card_id, 5) == 0){
                *posicao = i;
                return 1;
            }

        }
    
    }
    *posicao = 11;
    return 0;

}

unsigned char confirma_saldo(unsigned char posicao){
    if(bancoDeDados[posicao].saldo == 0){
        return 0;
    }
    return 1;
}

void subtrai_saldo(unsigned char posicao){
    bancoDeDados[posicao].saldo--;
}

void main(void)
{
    uint8_t card_id[5];
    uint16_t card_type;
    unsigned char botao = 0; // not PINC.5
    unsigned char leitura = 0; // 1 = pressionado

    unsigned char processa_cartao = 1;
    
    unsigned char posicao_da_conta = 0; // posicao da conta cadastrada

    unsigned char timer_aux = 255;

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

        estadoDaMaquina = STAND_BY;
        timer_aux = 0;
        LED_CONFIRMACAO = 1;
        LED_FUNCIONAMENTO = 1;
        processa_cartao = 1;

        // botao = ~PINC.5;
        // leitura = rc522ReadCard(card_id, &card_type);

        if(rc522ReadCard(card_id, &card_type))
        {
            
            /*
                printf("Card [0x%x, 0x%x, 0x%x, 0x%x, 0x%x]\n", card_id[0],card_id[1], card_id[2], card_id[3], card_id[4]);
                Importante fazer o print pelo menos uma fez pra saber o 
                endereço da tag, caso desejem fazer algoexclusivo para ela.
            */
        

            while(processa_cartao){
                botao = ~PINC.5;
                switch (estadoDaMaquina){
                    case STAND_BY:

                        if(botao){
                            LED_FUNCIONAMENTO = 0;
                            if(timer_aux>=100){
                                estadoDaMaquina = TRANSICAO_03;
                                timer_aux = 255;
                                break;
                            }
                            break;
                        }
                        if(timer_aux>= 10){
                            estadoDaMaquina = CARTAO_INVALIDO;
                            timer_aux = 255;
                            break;
                        }

                        LED_FUNCIONAMENTO = 1;
                        timer_aux = 255;

                        if( consulta_conta_cadastrada( card_id, card_type, &posicao_da_conta) ){
                            if( confirma_saldo(posicao_da_conta)){
                                    estadoDaMaquina = TRANSICAO_06;
                                    break;
                            }

                            estadoDaMaquina = SALDO_INSUFICIENTE;
                            break;
                        }

                        estadoDaMaquina = CARTAO_INVALIDO;
                    break;    
                    case TRANSICAO_03:
                        if(botao == 0){
                            estadoDaMaquina = RECARGA;
                            LED_FUNCIONAMENTO = 1;
                            timer_aux=255;
                            break;
                        }

                        if(timer_aux >= 10){
                            timer_aux = 0;
                            LED_FUNCIONAMENTO = ~LED_FUNCIONAMENTO;
                        }
                    
                    break;
                    case CONCLUSAO_2B:
                        LED_CONFIRMACAO = 0;
                        delay_ms(200);
                        LED_CONFIRMACAO = 1;
                        delay_ms(100);
                        LED_CONFIRMACAO = 0;
                        delay_ms(200);
                        LED_CONFIRMACAO = 1;
                        processa_cartao = 0;
                    break;
                    case RECARGA:

                        if( posicao_da_conta == 11 ){
                            if(numero_cadastrado == 10){
                                estadoDaMaquina = CARTAO_INVALIDO;
                                break;
                            }

                            memcpy(bancoDeDados[numero_cadastrado].card_id, card_id, 5);
                            bancoDeDados[numero_cadastrado].card_type =card_type; 
                            bancoDeDados[numero_cadastrado].saldo = 5;
                            
                            numero_cadastrado ++;
                            estadoDaMaquina = CONCLUSAO_3B;
                            break;
                        }

                        bancoDeDados[posicao_da_conta].saldo +=5;
                        estadoDaMaquina = CONCLUSAO_2B;

                    break;
                    case CONCLUSAO_3B:
                        LED_CONFIRMACAO = 0;
                        delay_ms(200);
                        LED_CONFIRMACAO = 1;
                        delay_ms(100);
                        LED_CONFIRMACAO = 0;
                        delay_ms(200);
                        LED_CONFIRMACAO = 1;
                        delay_ms(100);
                        LED_CONFIRMACAO = 0;
                        delay_ms(200);
                        LED_CONFIRMACAO = 1;

                        processa_cartao = 0;
                    break;
                    case TRANSICAO_06:
                        LED_CONFIRMACAO = 0;
                        delay_ms(200);
                        LED_CONFIRMACAO = 1;
                        
                        estadoDaMaquina = CONFIRMACAO;
                    break;
                    case CONFIRMACAO:

                        if(botao){
                            subtrai_saldo(posicao_da_conta);
                            estadoDaMaquina = CONCLUSAO_3B;
                            break;
                        }

                        if(timer_aux >= 100){
                            estadoDaMaquina = SALDO_INSUFICIENTE;
                        }


                    break;
                    case CARTAO_INVALIDO:
                        LED_FUNCIONAMENTO = 0;
                        delay_ms(200);
                        LED_FUNCIONAMENTO = 1;
                        delay_ms(100);
                        LED_FUNCIONAMENTO = 0;
                        delay_ms(200);
                        LED_FUNCIONAMENTO = 1;
                        delay_ms(100);
                        LED_FUNCIONAMENTO = 0;
                        delay_ms(200);
                        LED_FUNCIONAMENTO = 1;

                        processa_cartao = 0;
                    break;
                    case SALDO_INSUFICIENTE:
                        LED_FUNCIONAMENTO = 0;
                        delay_ms(200);
                        LED_FUNCIONAMENTO = 1;
                        delay_ms(100);
                        LED_FUNCIONAMENTO = 0;
                        delay_ms(200);
                        LED_FUNCIONAMENTO = 1;

                        processa_cartao = 0;
                    break;
                }
                timer_aux++;
                delay_ms(50);
            }
            delay_ms(1000);
        }

    }
}

/*
 * porteropaso.c
 *
 * Created: 18/1/2021 12:50:01
 * Author : Gustavo
 */ 

#define  F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
uint8_t valor1;
uint8_t valor2=2;
uint8_t valida(uint8_t t){
	return t;
}
int main(void)
{
    /* Replace with your application code */
	DDRD=0b00011111;
	DDRB &= ~(1<<DDB1);
	DDRB &= ~(1<<DDB2);
	PORTD &= ~(1<<PD2);
	PORTD &= ~(1<<PD3);
	PORTD &= ~(1<<PD4);
	PORTD &= ~(1<<PD5);
	
    while (1) 
    {
		
		if((PINB & (1<<PB1)) && (valor2==2)){
			valor2=valida(1);
			valor1=valida(2);
			//izquierda
			PORTD |= (1<<PD2);
			PORTD |=(1<<PD3);
			PORTD &=~(1<<PD4);
			PORTD &=~(1<<PD1);
			_delay_ms(20);
			//arriba
			PORTD |= (1<<PD2);
			PORTD &=~(1<<PD3);
			PORTD &=~(1<<PD4);
			PORTD |= (1<<PD1);
			_delay_ms(20);
			//derecha
			PORTD &= ~(1<<PD2);
			PORTD &=~(1<<PD3);
			PORTD |= (1<<PD4);
			PORTD |= (1<<PD1);
			_delay_ms(20);
			//abajo
			PORTD &= ~(1<<PD2);
			PORTD |= (1<<PD3);
			PORTD |= (1<<PD4);
			PORTD &= ~(1<<PD1);
		}
		if((PINB & (1<<PB2)) && (valor2==1)){
			valor2=valida(2);
			
			//derecha
			PORTD &= ~(1<<PD2);
			PORTD &=~(1<<PD3);
			PORTD |= (1<<PD4);
			PORTD |= (1<<PD1);
			_delay_ms(20);
			//arriba
			PORTD |= (1<<PD2);
			PORTD &=~(1<<PD3);
			PORTD &=~(1<<PD4);
			PORTD |= (1<<PD1);
			//izquierda
			_delay_ms(20);
			PORTD |= (1<<PD2);
			PORTD |=(1<<PD3);
			PORTD &=~(1<<PD4);
			PORTD &=~(1<<PD1);
			_delay_ms(20);
			//abajo
			PORTD &= ~(1<<PD2);
			PORTD |= (1<<PD3);
			PORTD |= (1<<PD4);
			PORTD &= ~(1<<PD1);
			
		}
		
    }
}


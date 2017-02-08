#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t desborde;

void timer0_init() {
	//TCCR0B = (1 << CS02) | (1 << CS00);
	TCCR0B = 0x05;

	TCNT0 = 0;

	//TIMSK0 = (1 << TOIE0);
	TIMSK0 = 0x01;

	sei();

	desborde = 0;
}

ISR(TIMER0_OVF_vect) {
	desborde++;
}

void reinicio() {
	desborde = 0;
	TCNT0 = 0;
}

/*void mostrar_display_anodo(volatile uint8_t * puerto, uint8_t decimal) {
	switch(decimal) {
		case 0:
			*puerto = 0xc0;
		break;
		case 1:
			*puerto = 0xf9;
		break;
		case 2:
			*puerto = 0xa4;
		break;
		case 3:
			*puerto = 0xb0;
		break;
		case 4:
			*puerto = 0x99;
		break;
		case 5:
			*puerto = 0x92;
		break;
		case 6:
			*puerto = 0x83;
		break;
		case 7:
			*puerto = 0xf8;
		break;
		case 8:
			*puerto = 0x80;
		break;
		case 9:
			*puerto = 0x98;
		break;
		default:
			*puerto = 0xc0;
	}
}*/

uint8_t mostrar_display_catodo(volatile uint8_t * puerto, uint8_t decimal) {
	switch(decimal) {
		case 0:
			return (*puerto = 0x3f);
		break;
		case 1:
			return (*puerto = 0x06);
		break;
		case 2:
			return (*puerto = 0x5b);
		break;
		case 3:
			return (*puerto = 0x4f);
		break;
		case 4:
			return (*puerto = 0x66);
		break;
		case 5:
			return (*puerto = 0x6d);
		break;
		case 6:
			return (*puerto = 0x7c);
		break;
		case 7:
			return (*puerto = 0x07);
		break;
		case 8:
			return (*puerto = 0x7f);
		break;
		case 9:
			return (*puerto = 0x67);
		break;
		default:
			return (*puerto = 0x3f);
	}
}

void double_dabble(uint8_t contador) {
	uint8_t aux = 0x00, i;

	//while(contador != 0) {
	for(i = 0; i < 8; i++) {
		if((aux & 0x0f) > 4)
			aux += 3;

		if(((aux & 0xf0) >> 4) > 4)
			aux += (3 << 4);

		aux = aux << 1;
		aux += (contador & 0x80) >> 7;

		contador = contador << 1;
	}

	uint8_t dataD = mostrar_display_catodo(&PORTD,(uint8_t)(aux & 0x0f));
	uint8_t dataB =  mostrar_display_catodo(&PORTB,(uint8_t)((aux & 0xf0) >> 4));
	PORTD = (dataB & 0x40) << 1;
}

int main() {
	DDRC = 0x00;
	DDRB = 0xff;
	DDRD = 0xff;

	PORTC = 0xff;

	timer0_init();

	uint8_t binario, contador;

	binario = PINC;

	while(1) {
		if(binario != PINC) {
			binario = PINC;
			contador = binario;
			reinicio();
		}

		if(desborde >= 61)
			if(TCNT0 >= 9) {
				contador++;

				if(contador > 99)
					contador = 0;

				reinicio();
			}

		double_dabble(contador);
	}

	return 0;
}
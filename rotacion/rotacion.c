#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t desborde;

void timer0_init() {
	TCCR0B = 0x05;

	TCNT0 = 0x00;

	TIMSK0 = 0x01;

	sei();

	desborde = 0;
}

ISR(TIMER0_OVF_vect) {
	desborde++;
}

void reinicio() {
	TCNT0 = 0;
	desborde = 0;
}

uint8_t rotacion(uint8_t data) {
	uint8_t carry = (data & 0x80)? 0x01 : 0x00;

	return (data << 1) + carry;
}

int main() {
	DDRC = 0x00;
	DDRB = 0x00;
	DDRD = 0xff;

	PORTC = 0xff;
	PORTB = 0xff;

	timer0_init();

	uint8_t data, aux;

	data = 0;

	while(1) {
		uint8_t binario = (PINB & 0x3f) | ((PINC  & 0x03) << 6);

		if(data != binario) {
			data = binario;
			aux = data;
			reinicio();
		}

		if(desborde >= 61)
			if(TCNT0 >= 9) {
				aux = rotacion(aux);
				reinicio();
			}

		PORTD = aux;
	}

	return 0;
}
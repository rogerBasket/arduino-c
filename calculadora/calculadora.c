#include <avr/io.h>

int main() {
	DDRB = 0xff;
	DDRD = 0x00;
	DDRC = 0x00;

	PORTD = 0xff;
	PORTC = 0xff;

	while(1) {
		short opt = PINC;
		
		short datos = PIND;

		short a = datos & 0x0f;
		short b = (datos & 0xf0) >> 4; 

		switch(opt) {
			case 0:
				PORTB = a + b;
			break;
			case 1:
				PORTB = a - b;
			break;
			case 2:
				PORTB = ((a & 0x0c) >> 2) + (a & 0x03);
			break;
			case 3:
				PORTB = (a & 0x03) * (b & 0x03);
			break;
			default:
				PORTB = a + a;
		}
	}

	return 0;
}
#include <avr/io.h>

int main() {
	DDRB = 0x00;
	DDRD = 0xff;

	while(1) {
		PORTD = PINB;
	}
}
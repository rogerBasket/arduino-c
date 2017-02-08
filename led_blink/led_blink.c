#include <avr/io.h>
#include <util/delay.h>

int main() {
	short leds = 1;

	DDRB = 0xff;

	while(1) {
		for(leds = 1; leds <= (1 << 5); leds <<= 1) {
			PORTB = leds;
			_delay_ms(200);
		}
		for(leds = (1 << 4); leds >= 2; leds >>= 1) {
			PORTB = leds;
			_delay_ms(200);
		}
	}

	return 0;
}
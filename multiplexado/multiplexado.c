#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t desborde;
volatile short digito;
volatile uint8_t conversion;

void adc_init() {
	/*
		seleccion del voltaje de referencia
	*/
	ADMUX = (1 << REFS0);     

	/*
		seleccion de la entrada analogica 0 del micro
	*/
	ADMUX |= 0x00;

	/*
		habilita el adc del micro
	*/
	ADCSRA |= (1 << ADEN);

	/*
		habilita el modo carrera libre del microcontrolador
	*/
	ADCSRA |= (1 << ADATE);

	/*
		activa la interrupcion del adc cuando una conversion ha sido completada
	*/
	ADCSRA |= (1 << ADIE);

	/*
		divide la frecuencia del reloj entre un factor de division,
		ADPS2 .. 0, establecidos en 1 selecciona un factor de 128
	*/
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	/*
		habilita las interrupciones globales
	*/
	sei();

	digito = 0;
	conversion = 0;

	/*
		inicia la primera conversion del adc
	*/
	ADCSRA |= (1 << ADSC);
}

void timer0_init() {
	/*
		selecciona un prescalar para dividir la frecuencia del reloj del micro,
		0x05 establece un factor de 1024
	*/
	TCCR0B = 0x05;

	/*
		inicia el contador del timer0 a 0
	*/
	TCNT0 = 0x00;

	/*
		habilita la interrupcion overflow del timer0
	*/
	TIMSK0 = 0x01;

	/*
		habilita ñas interrupciones globales
	*/
	sei();

	desborde = 0;
}

/*
	rutina que se llama cuando se produce una interrupcion overflow del timer0
*/
ISR(TIMER0_OVF_vect) {
	desborde++;
}

/*
	rutina que se llama cuando se produce una interrupcion del adc
*/
ISR(ADC_vect) {
	conversion = 1;

	digito = (ADCL) | (ADCH << 8);
}

void reinicio() {
	TCNT0 = 0;
	desborde = 0;
}

uint8_t mostrar_display_anodo(volatile uint8_t * puerto, uint8_t decimal) {
	switch(decimal) {
		case 0:
			return (*puerto = 0xc0);
		break;
		case 1:
			return (*puerto = 0xf9);
		break;
		case 2:
			return (*puerto = 0xa4);
		break;
		case 3:
			return (*puerto = 0xb0);
		break;
		case 4:
			return (*puerto = 0x99);
		break;
		case 5:
			return (*puerto = 0x92);
		break;
		case 6:
			return (*puerto = 0x83);
		break;
		case 7:
			return (*puerto = 0xf8);
		break;
		case 8:
			return (*puerto = 0x80);
		break;
		case 9:
			return (*puerto = 0x98);
		break;
		default:
			return (*puerto = 0xc0);
	}
}

void seleccionar_display(volatile uint8_t * puerto, uint8_t display) {
	switch(display) {
		case 0:
			*puerto = 0x01;
		break;
		case 1:
			*puerto = 0x02;
		break;
		case 2:
			*puerto = 0x04;
		break;
		default:
			*puerto = 0x01;
	}
}

void double_dabble(volatile uint8_t * selector, volatile uint8_t * puerto, short binario) {
	short i, aux = 0;
	uint8_t display;

	if(binario > 999)
		binario = 999;

	for(i = 0; i < 12; i++) {
		if((aux & 0x0f) > 4)
			aux += 3;

		if(((aux & 0xf0) >> 4) > 4)
			aux += (3 << 4);

		aux = aux << 1;
		aux += (binario & 0x0800) >> 11;

		binario = binario << 1;
	}

	uint8_t valores[3] = {((aux & 0x0f00) >> 8),((aux & 0x00f0) >> 4),((aux & 0x000f))};

	for(display = 0; display < 3;) {
		seleccionar_display(selector,display);
		mostrar_display_anodo(puerto,valores[display]);
		if(desborde % 4 == 0) {
			display++;
		}
	}
	//mostrar_display_anodo(salida,aux);
}

void multiplexado(volatile uint8_t * selector, volatile uint8_t * puerto) {
	uint8_t display;

	//mostrar_display_catodo(puerto,dato);

	for(display = 0; display < 3;) {
			seleccionar_display(selector,display);
			mostrar_display_anodo(puerto,display+1);
			if(TCNT0 > 250) {
				display++;
			}
	}
	
	/*seleccionar_display(selector,display);
	mostrar_display_anodo(puerto,dato);

	seleccionar_display(selector,1);
	mostrar_display_anodo(puerto,2);

	seleccionar_display(selector,2);
	mostrar_display_anodo(puerto,3);*/
}

int main() {
	DDRD = 0xff;
	DDRB = 0xff;

	timer0_init();

	uint8_t i = 0;

	while(i > 3) {
		multiplexado(&PORTB,&PORTD);

		if(desborde >= 61)
			if(TCNT0 >= 9) {
				i++;
				//i++;
				reinicio();
			}
	}

	adc_init();

	while(1) {
		if(conversion == 1) {
			double_dabble(&PORTB,&PORTD,digito);

			if(desborde >= 1)
				reinicio();

			conversion = 0;
		}
	}
	
	return 0;
}
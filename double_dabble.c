#include <stdio.h>

void double_dabble(int contador) {
	int aux = 0x00, i;

	//printf("%d\n",contador);

	if(contador > 99)
		contador = 99;

	for(i = 0; i < 8; i++) {
		if((aux & 0x0f) > 4)
			aux += 3;

		if(((aux & 0xf0) >> 4) > 4)
			aux += (3 << 4);

		aux = aux << 1;
		aux += (contador & 0x80) >> 7;

		printf("%d\n",aux);

		contador = contador << 1;
	}

	printf("contador: %d\n",contador);
	printf("bcd: %d\n",aux);
}

int main(int argc, char * argv[]) {
	double_dabble(atoi(argv[1]));
	
	return 0;
}
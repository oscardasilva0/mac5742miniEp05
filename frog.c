#include <stdio.h>
#include <stdlib.h>
/**
 * -1 macho
 * 1 femea
 * */
int* gereVetor(int tamanho, int metade) {
	int *sapos = (int*) malloc(tamanho * sizeof(int));

	for (int i = 0; i < tamanho; i++) {
		sapos[i] = 1;
		if (i < metade) {
			sapos[i] = -1;
		}
	}
	sapos[metade] = 0;
	return sapos;
}

void mostraSapos(int *sapos,int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		printf("|%d| ", sapos[i]);
	}

}

int verificaParada(int *sapos, int tamanho) {
	int metade = (tamanho - 1) / 2;
	int retorno = 0;
	for (int i = 0; i < metade; i++) {
		if (sapos[i] != -1) {
			retorno = 1;

		}

	}

	return retorno;

}

int main() {
	int casais = 5;
	int tamanho = (casais * 2) + 1;
	int *sapos = gereVetor(tamanho, casais);
	mostraSapos(sapos, 11);
	int condicaparada = 0;
	while (condicaparada == 0) {
		condicaparada = verificaParada(sapos, tamanho);
		printf("\nw%dw ", condicaparada);

	}

	// Liberando a memória alocada
	free(sapos);

	return 0;
}

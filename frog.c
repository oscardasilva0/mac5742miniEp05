#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
volatile unsigned int count = 0;

void *increment(void *);

volatile unsigned int reinicios = 0;
//volatile unsigned int pulos = 0;

static char femea = 'F';
static char macho = 'M';
static char pedra = 'O';

static int tamanho = 7;
static int metade = 3;

typedef struct {
	char ID[2];
	int Possicao;
	char Sexo; // F=Femino S=Masculino  O= Pedra
} Sapo;

Sapo *saposLagoa;

int possoPular(Sapo *sapo) {
	int possicaoPulo = -1;
	int possicaoSapo = sapo->Possicao;

	if (sapo->Sexo == femea) {
		if (possicaoSapo - 1 >= 0 && saposLagoa[possicaoSapo - 1].Sexo == pedra) {
			possicaoPulo = possicaoSapo - 1;
		} else if (possicaoSapo - 2 >= 0 && saposLagoa[possicaoSapo - 2].Sexo == pedra) {
			possicaoPulo = possicaoSapo - 2;
		}
	} else if (sapo->Sexo == macho) {
		if (possicaoSapo + 1 < tamanho && saposLagoa[possicaoSapo + 1].Sexo == pedra) {
			possicaoPulo = possicaoSapo + 1;
		} else if (possicaoSapo + 2 < tamanho && saposLagoa[possicaoSapo + 2].Sexo == pedra) {
			possicaoPulo = possicaoSapo + 2;
		}
	}


	return possicaoPulo;
}

void *pular(void *arg) {
	int localPulo = -1;
	Sapo *sapo = (Sapo *) arg;
	if (sapo->Sexo == femea || sapo->Sexo == macho) {
		for (int pulos = 0; pulos < 1000; pulos++) {
			localPulo = possoPular(sapo);
			if (localPulo > -1) {
				pthread_mutex_lock(&mutex);
				localPulo = possoPular(sapo);
				if (saposLagoa[localPulo].Sexo == pedra) {
					Sapo sapoPedra = saposLagoa[localPulo];

					//mudar local do sapo
					int possicaoSapo = sapo->Possicao;
					sapo->Possicao = localPulo;
					saposLagoa[localPulo] = *sapo;

					//mudar pedra
					sapoPedra.Possicao = possicaoSapo;
					saposLagoa[possicaoSapo] = sapoPedra;
				}
				pthread_mutex_unlock(&mutex);
			}
		}
		printf("|%c%c| ", sapo->ID[0], sapo->ID[1]);
		printf("|%c|\n ", sapo->Sexo == pedra);
	}
	return ((void *) 0);
}


Sapo *gereVetor() {
	Sapo *sapos = (Sapo *) malloc(tamanho * sizeof(Sapo));

	for (int i = 0; i < tamanho; i++) {
		sapos[i].Possicao = i;
		sapos[i].Sexo = macho;
		if (i > metade) {
			sapos[i].Sexo = femea;
		}
		sapos[i].ID[0] = sapos[i].Sexo;
		sapos[i].ID[1] = i + '0';
	}
	sapos[metade].Sexo = pedra;
	sapos[metade].ID[0] = sapos[metade].Sexo;
	sapos[metade].ID[1] = 'O';
	return sapos;
}

bool existemPulos() {
	int retorno = false;
	for (int i = 0; i < tamanho; i++) {
		if (possoPular(&saposLagoa[i]) != -1) {
			retorno = true;
			break;
		}
	}
	return retorno;
}

//void mostraSapos(int *sapos, int tamanho) {
void mostraSapos(Sapo *sapos) {
	for (int i = 0; i < tamanho; i++) {
		printf("|%c%c| ", sapos[i].ID[0], sapos[i].ID[1]);
	}
	printf("\n");
}

int main() {
	pthread_t threads[tamanho];

	while (reinicios < 1) {
		saposLagoa = gereVetor();
		mostraSapos(saposLagoa);

		for (int i = 0; i < tamanho; i++) {
			pthread_create(&threads[i], NULL, pular, &saposLagoa[i]);
		}

		for (int i = 0; i < tamanho; i++) {
			pthread_join(threads[i], NULL);
		}

		mostraSapos(saposLagoa);
		reinicios++;
	}
	return (0);
}

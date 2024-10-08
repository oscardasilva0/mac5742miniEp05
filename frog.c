
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;

volatile unsigned int count = 0;

void *increment(void *);

volatile unsigned int totalPulos = 0;
bool possoPular = true;

static char femea = 'F';
static char macho = 'M';
static char pedra = 'O';

// static int tamanho = 7;
// static int metade = 3;
static int tamanho = 7;
static int metade = 3;
/*      y
 *m m m 0 f f f
    m m 0 f f
m m m m 0 f f f f*/
typedef struct {
	char ID[2];
	int Possicao;
	char Sexo; // F=Femino S=Masculino  O= Pedra
} Sapo;

Sapo *saposLagoa;

int pular(Sapo *sapo) {
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

void *verificaPulo(void *arg) {
	int localPulo = -1;
	Sapo *sapo = (Sapo *) arg;

	pthread_barrier_wait(&barrier);

	if (sapo->Sexo == femea || sapo->Sexo == macho) {
		do {
			pthread_mutex_lock(&mutex);
			localPulo = pular(sapo);
			if (localPulo > -1 && saposLagoa[localPulo].Sexo == pedra) {
				Sapo sapoPedra = saposLagoa[localPulo];

				//mudar local do sapo
				int possicaoSapo = sapo->Possicao;
				sapo->Possicao = localPulo;
				saposLagoa[localPulo] = *sapo;

				//mudar pedra
				sapoPedra.Possicao = possicaoSapo;
				saposLagoa[possicaoSapo] = sapoPedra;
				totalPulos = 0;
			}
			pthread_mutex_unlock(&mutex);
			totalPulos++;
		} while (possoPular) ;
	}
	pthread_exit(NULL);
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

void mostraSapos(Sapo *sapos) {
	for (int i = 0; i < tamanho; i++) {
		printf("|%c%c| ", sapos[i].ID[0], sapos[i].ID[1]);
	}
	printf("\n");
}

bool ordemCorreta() {
	if (saposLagoa[metade].Sexo != pedra) {
		return false;
	}
	for (int i = 0; i < metade; i++) {
		if (saposLagoa[i].Sexo == macho) {
			return false;
			break;
		}
	}
	return true;
}

bool existemPulos() {
	pthread_mutex_lock(&mutex);
	bool retorno = false;
	for (int i = 0; i < tamanho; i++) {
		if (pular(&saposLagoa[i]) != -1) {
			retorno = true;
		}
	}
	pthread_mutex_unlock(&mutex);
	return retorno;
}

void *existemPulosThread() {
	int maximoPulos = 100000 * tamanho;
	while (existemPulos() && totalPulos < (unsigned int) maximoPulos) {
		possoPular = true;
	}
	possoPular = false;
	pthread_exit(NULL);
	return ((void *) 0);
}

int main() {
	pthread_t threads[tamanho];
	pthread_t threadsVerificaPulo;
	pthread_barrier_init(&barrier, NULL, tamanho);
	int totalCorretos = 0;
	for (int reinicios = 0; reinicios < 1000; reinicios++) {
		totalPulos = 0;
		saposLagoa = gereVetor();
		//mostraSapos(saposLagoa);

		pthread_create(&threadsVerificaPulo, NULL, existemPulosThread, NULL);

		for (int i = 0; i < tamanho; i++) {
			if (pthread_create(&threads[i], NULL, verificaPulo, &saposLagoa[i]) != 0) {
				perror("Erro ao criar thread do sapo");
				exit(1); // Encerra o programa em caso de erro
			}
		}

		for (int i = 0; i < tamanho; i++) {
			pthread_join(threads[i], NULL);
		}
		pthread_join(threadsVerificaPulo, NULL);
		mostraSapos(saposLagoa);
		if (ordemCorreta()) {
			totalCorretos++;
		}
		free(saposLagoa);
	}
	pthread_barrier_destroy(&barrier);
	printf("Total de corretos: %d\n", totalCorretos);
	return (0);
}

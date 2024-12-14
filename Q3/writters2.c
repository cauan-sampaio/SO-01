#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 100
#define NUM_WRITERS 1

pthread_mutex_t mutex;
sem_t db;
int leitor_lendo = 0;
int escritores_esperando = 0;

void ler_base_de_dados(void);
void usar_banco(void);
void pensando_nos_dados(void);
void escrever_no_banco(void);

void *leitor(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        // Bloqueia leitores se escritores estão esperando
        while (escritores_esperando > 0) {
            pthread_mutex_unlock(&mutex);
            usleep(100);  // Pequena espera para evitar ocupação de CPU
            pthread_mutex_lock(&mutex);
        }

        leitor_lendo++;
        if (leitor_lendo == 0) {
            sem_wait(&db); // Primeiro leitor bloqueia a região crítica
        }
        pthread_mutex_unlock(&mutex);

        ler_base_de_dados();

        pthread_mutex_lock(&mutex);
        leitor_lendo--;
        if (leitor_lendo == 1) {
            sem_post(&db); // Último leitor libera a região crítica
        }
        pthread_mutex_unlock(&mutex);

        usar_banco();
    }
}

void *escritor(void *arg) {
    while (1) {
        pensando_nos_dados();

        pthread_mutex_lock(&mutex);
        escritores_esperando++;
        printf("Escritores esperando: %d\n", escritores_esperando);
        pthread_mutex_unlock(&mutex);

        sem_wait(&db); // Escritor bloqueia a região crítica
        escrever_no_banco();
        sem_post(&db); // Escritor libera a região crítica

        pthread_mutex_lock(&mutex);
        escritores_esperando--;
        pthread_mutex_unlock(&mutex);
    }
}

void ler_base_de_dados() {
    int tempo_leitura = rand() % 3 + 1;
    printf("Leitor lendo banco de dados. Total de leitores: %d\n", leitor_lendo);
    sleep(tempo_leitura);
}

void usar_banco() {
    int tempo_uso = rand() % 5 + 1;
    printf("Leitor utilizando dados do banco.\n");
    sleep(tempo_uso);
}

void pensando_nos_dados() {
    int esperando = rand() % 4 + 1;
    // printf("Escritores esperando: %d\n", escritores_esperando);
    sleep(esperando);
}

void escrever_no_banco() {
    int tempo_escrever = rand() % 3 + 1;
    printf("Escritor escrevendo no banco de dados.\n");
    sleep(tempo_escrever);
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    int i;

    pthread_mutex_init(&mutex, NULL);
    sem_init(&db, 0, 1);

    // Criando threads de leitores
    for (i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, leitor, NULL);
    }

    // Criando threads de escritores
    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, escritor, NULL);
    }

    // Aguardando threads
    for (i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&db);

    return 0;
}

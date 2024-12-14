#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 10
#define NUM_WRITERS 5

pthread_mutex_t mutex;
pthread_mutex_t db;
int leitor_lendo = 0;

void ler_base_de_dados(void);
void usar_banco(void);
void pensando_nos_dados(void);
void escrever_no_banco(void);

void leitor() {
    while (1)
    {
        pthread_mutex_lock(&mutex);
        leitor_lendo += 1;
        if (leitor_lendo == 1) 
            pthread_mutex_lock(&db);
        pthread_mutex_unlock(&mutex);

        ler_base_de_dados();

        pthread_mutex_lock(&mutex);
        leitor_lendo -= 1;
        if (leitor_lendo == 0)
            pthread_mutex_unlock(&db);
        pthread_mutex_unlock(&mutex);

        usar_banco();
    }
}

void escritor() {
    while (1)
    {
        pensando_nos_dados();
        pthread_mutex_lock(&db);
        escrever_no_banco();
        pthread_mutex_unlock(&db);
    }
}

void ler_base_de_dados(){
    int tempo_leitura = rand() % 5;
    printf("Usuario lendo banco de dados. Total de %d leitores lendo agora\n", leitor_lendo);
    sleep(tempo_leitura);
}

void usar_banco(){
    int tempo_uso = rand() % 15;
    printf("Usuario utilizando conhecimento do banco de dados.\n");
    sleep(tempo_uso);
}

void pensando_nos_dados(){
    int tempo_pensar = rand() % 10;
    printf("Escritor pensando no que escrever\n");
    sleep(tempo_pensar);
}

void escrever_no_banco(){
    int tempo_escrever = rand() % 10;
    printf("Escritor escrevendo no banco de dados\n");
    sleep(tempo_escrever);
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    int i;

    pthread_mutex_init(&db, NULL);
    pthread_mutex_init(&mutex, NULL);

    // Criando threads de leitores
    for (i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, (void *) leitor, NULL);
    }

    // Criando threads de escritores
    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, (void *) escritor, NULL);
    }

    // Aguarda as threads terminarem
    for (i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_mutex_destroy(&db);
    pthread_mutex_destroy(&mutex);

    return 0;
}

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

sem_t db;  // Controle de acesso ao banco de dados
sem_t mutex, writer_queue;  // Proteção de contadores e fila de escritores
int rc = 0;  // Contador de leitores
int wc = 0;  // Contador de escritores esperando

void* reader(void* arg) {
    int id = *(int*)arg;

    sem_wait(&writer_queue);  // Prioridade para escritores
    sem_wait(&mutex);
    rc++;
    if (rc == 1) sem_wait(&db);  // Primeiro leitor bloqueia escritores
    sem_post(&mutex);
    sem_post(&writer_queue);

    printf("Reader %d is reading the database...\n", id);
    sleep(1);  // Simula leitura
    printf("Reader %d finished using the data.\n", id);

    sem_wait(&mutex);
    rc--;
    if (rc == 0) sem_post(&db);  // Último leitor libera escritores
    sem_post(&mutex);

    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;

    printf("Writer %d is thinking up data...\n", id);
    sleep(1);  // Simula preparação de dados

    sem_wait(&mutex);
    wc++;
    if (wc == 1) sem_wait(&writer_queue);  // Primeiro escritor bloqueia novos leitores
    sem_post(&mutex);

    sem_wait(&db);  // Acessa a base de dados
    printf("Writer %d is writing to the database...\n", id);
    sleep(2);  // Simula escrita
    printf("Writer %d finished writing.\n", id);
    sem_post(&db);

    sem_wait(&mutex);
    wc--;
    if (wc == 0) sem_post(&writer_queue);  // Último escritor libera leitores
    sem_post(&mutex);

    return NULL;
}

int main() {
    pthread_t readers[5], writers[2];
    int ids[5] = {0, 1, 2, 3, 4};

    sem_init(&db, 0, 1);
    sem_init(&mutex, 0, 1);
    sem_init(&writer_queue, 0, 1);

    for (int i = 0; i < 5; i++) pthread_create(&readers[i], NULL, reader, &ids[i]);
    for (int i = 0; i < 2; i++) pthread_create(&writers[i], NULL, writer, &ids[i]);

    for (int i = 0; i < 5; i++) pthread_join(readers[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(writers[i], NULL);

    sem_destroy(&db);
    sem_destroy(&mutex);
    sem_destroy(&writer_queue);
    return 0;
}

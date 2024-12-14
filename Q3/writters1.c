#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

sem_t mutex;  // Controle de acesso à variável compartilhada `rc`
sem_t db;     // Controle de acesso exclusivo à base de dados
int rc = 0;   // Contador de leitores ativos

void* reader(void* arg) {
    while (1) {
        // Região crítica para atualizar o contador de leitores
        sem_wait(&mutex);  // down(&mutex)
        rc++;
        if (rc == 1) {
            sem_wait(&db); // Primeiro leitor bloqueia escritores
        }
        sem_post(&mutex);  // up(&mutex)

        // Leitura da base de dados
        printf("Reader %ld is reading the database...\n", (long)arg);
        
        // Região crítica para atualizar o contador de leitores
        sem_wait(&mutex);  // down(&mutex)
        rc--;
        if (rc == 0) {
            sem_post(&db); // Último leitor libera escritores
        }
        sem_post(&mutex);  // up(&mutex)

        // Região não crítica
        printf("Reader %ld finished using the data.\n", (long)arg);
    }
    return NULL;
}

void* writer(void* arg) {
    while (1) {
        // Região não crítica
        printf("Writer %ld is thinking up data...\n", (long)arg);

        // Região crítica: acesso exclusivo à base de dados
        sem_wait(&db);  // down(&db)
        printf("Writer %ld is writing to the database...\n", (long)arg);
        sem_post(&db);  // up(&db)
        
        // Região não crítica
        printf("Writer %ld finished writing.\n", (long)arg);
    }
    return NULL;
}

int main() {
    pthread_t readers[5], writers[2]; // Threads para leitores e escritores

    // Inicialização dos semáforos
    sem_init(&mutex, 0, 1); // Inicializa `mutex` com valor 1
    sem_init(&db, 0, 1);    // Inicializa `db` com valor 1

    // Criação de threads para leitores
    for (long i = 0; i < 5; i++) {
        pthread_create(&readers[i], NULL, reader, (void*)i);
    }

    // Criação de threads para escritores
    for (long i = 0; i < 2; i++) {
        pthread_create(&writers[i], NULL, writer, (void*)i);
    }

    // Aguardar as threads (nunca termina neste exemplo)
    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    // Destruição dos semáforos (não alcançado aqui)
    sem_destroy(&mutex);
    sem_destroy(&db);

    return 0;
}

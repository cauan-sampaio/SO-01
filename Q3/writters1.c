#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TOTAL_LEITORES 10
#define TOTAL_ESCRITORES 3

sem_t leitores_sem;
sem_t escritores_sem;
int leitores_ativos = 0;

void* leitor(void* id) {
    int leitor_id = *((int*)id);

    sem_wait(&leitores_sem);
    leitores_ativos++;
    sem_post(&leitores_sem);

    printf("Leitor %d lendo.\n", leitor_id);
    sleep(1); // Simula o tempo de leitura

    sem_wait(&leitores_sem);
    leitores_ativos--;
    sem_post(&leitores_sem);

    return NULL;
}

void* escritor(void* id) {
    sem_wait(&leitores_sem);
    while (leitores_ativos > 0) {
        sem_post(&leitores_sem); // Desbloqueia a espera
        sleep(1); // Espera enquanto há leitores
        sem_wait(&leitores_sem);
    }

    printf("Escritor %d escrevendo.\n", *((int*)id));
    sleep(2); // Simula o tempo de escrita
    sem_post(&leitores_sem);

    return NULL;
}

int main() {
    pthread_t leitores[TOTAL_LEITORES];
    pthread_t escritores[TOTAL_ESCRITORES];
    int ids[TOTAL_LEITORES];

    sem_init(&leitores_sem, 0, 1);  // Semáforo para controlar o número de leitores ativos
    sem_init(&escritores_sem, 0, 1); // Semáforo para controlar o número de escritores

    // Cria as threads de leitores
    for (int i = 0; i < TOTAL_LEITORES; i++) {
        ids[i] = i;
        pthread_create(&leitores[i], NULL, leitor, &ids[i]);
    }

    // Cria as threads de escritores
    for (int i = 0; i < TOTAL_ESCRITORES; i++) {
        pthread_create(&escritores[i], NULL, escritor, &i);
    }

    // Aguarda as threads de leitores e escritores terminarem
    for (int i = 0; i < TOTAL_LEITORES; i++) {
        pthread_join(leitores[i], NULL);
    }

    for (int i = 0; i < TOTAL_ESCRITORES; i++) {
        pthread_join(escritores[i], NULL);
    }

    sem_destroy(&leitores_sem);
    sem_destroy(&escritores_sem);

    return 0;
}

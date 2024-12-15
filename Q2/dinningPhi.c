#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5               // Número de filósofos e hashis
#define MEALS_LIMIT 3      // Limite de refeições por filósofo

pthread_mutex_t chopstick[N];

void *philosopher(void *num) {
    int id = *(int *)num;
    int left = id;              // Hashi à esquerda
    int right = (id + 1) % N;   // Hashi à direita
    int meals = 0;              // Contador de refeições

    while (meals < MEALS_LIMIT) {
        printf("Philosopher %d is thinking.\n", id);
        sleep(1); // Pensando

        // Pegando os hashis em ordem determinada para evitar deadlock
        if (id % 2 == 0) {
            pthread_mutex_lock(&chopstick[left]);
            printf("Philosopher %d: got left chopstick %d.\n", id, left);
            pthread_mutex_lock(&chopstick[right]);
            printf("Philosopher %d: got right chopstick %d.\n", id, right);
        } else {
            pthread_mutex_lock(&chopstick[right]);
            printf("Philosopher %d: got right chopstick %d.\n", id, right);
            pthread_mutex_lock(&chopstick[left]);
            printf("Philosopher %d: got left chopstick %d.\n", id, left);
        }

        // Comendo
        printf("Philosopher %d is eating.\n", id);
        sleep(2); // Tempo para comer
        meals++;

        // Liberando os hashis
        pthread_mutex_unlock(&chopstick[left]);
        printf("Philosopher %d: released left chopstick %d.\n", id, left);
        pthread_mutex_unlock(&chopstick[right]);
        printf("Philosopher %d: released right chopstick %d.\n", id, right);

        printf("Philosopher %d finished eating meal %d.\n", id, meals);
    }

    printf("Philosopher %d is done with all meals.\n", id);
    free(num);
    return NULL;
}

int main() {
    pthread_t philo[N];

    // Inicializando os mutexes (hashis)
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&chopstick[i], NULL);
    }

    // Criando as threads dos filósofos
    for (int i = 0; i < N; i++) {
        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&philo[i], NULL, philosopher, id);
    }

    // Aguardando as threads finalizarem
    for (int i = 0; i < N; i++) {
        pthread_join(philo[i], NULL);
    }

    // Destruindo os mutexes
    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&chopstick[i]);
    }

    return 0;
}

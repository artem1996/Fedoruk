#include <stdlib.h>
//#include "main.h"
#include <stdio.h>
#include <pthread.h>

#define TIME_STEP 1.0
#define LENGTHX 10.0
#define LENGTHY 10.0
#define SQR_A 1.0

int threads;
double full_time;
int nodes;
double dx, dy;
int numStrOnThread;
int turn;
double** mainMatrix;
double** tempMatrix;

pthread_barrier_t startBarrier;
pthread_barrier_t workBarrier;
pthread_barrier_t endBarrier;

double func(double x, double y, double t) {
    return (x + y) * t;
}

void* solve(void*) {
    int tempTime = 0;
    int number = turn++;
    int startStr = number * numStrOnThread + 1;
    int endStr = threads - 1 > number ? startStr + numStrOnThread : nodes - 1;
    pthread_barrier_wait(&startBarrier);
    while(tempTime < full_time) {
        for(int j = startStr; j < endStr; j++)
            for(int i = 1; i < nodes - 1; i++)
                tempMatrix[i][j] = (SQR_A * ((mainMatrix[i - 1][j] - 2 * mainMatrix[i][j] + mainMatrix[i + 1][j]) / dx +
                                             (mainMatrix[i][j - 1] - 2 * mainMatrix[i][j] + mainMatrix[i][j + 1]) / dy) + func(i, j, tempTime)) *
                                   TIME_STEP * TIME_STEP + mainMatrix[i][j];
        tempTime += TIME_STEP;
        pthread_barrier_wait(&workBarrier);
        for(int j = startStr; j < endStr; j++)
            for(int i = 1; i < nodes - 1; i++)
                mainMatrix[i][j] = tempMatrix[i][j];
        pthread_barrier_wait(&workBarrier);
    }
    pthread_barrier_wait(&endBarrier);
}

int main(int argc, char** argv) {
    if(argc != 4) {
        printf("Usage: ./progname num_threads time num_nodes\n");
        return(-1);
    }
    threads = atoi(argv[1]);
    full_time = atof(argv[2]);
    nodes = atoi(argv[3]);
    if(threads < 1 || full_time < TIME_STEP || nodes < 3) {
        printf("Некорректные данные: num_thread >= 1, time >= %f, num_nodes >= 3\n", TIME_STEP);
        return -1;
    }
    if(threads > nodes - 2) {
        printf("Будет использовано только %d потоков. Больше не имеет сысла!\n", nodes - 2);
        threads = nodes - 2;
    }
    mainMatrix = new double*[nodes];
    for(int i = 0; i < nodes; i++) {
        mainMatrix[i] = new double[nodes];
        for (int j = 0; j < nodes; j++)
            mainMatrix[i][j] = 0.0;
    }
    tempMatrix = new double*[nodes];
    for(int i = 0; i < nodes; i++) {
        tempMatrix[i] = new double[nodes];
        for (int j = 0; j < nodes; j++)
            tempMatrix[i][j] = 0.0;
    }
    dx = LENGTHX * LENGTHX / (nodes - 1) / (nodes - 1);
    dy = LENGTHY * LENGTHY / (nodes - 1) / (nodes - 1);
    numStrOnThread = (nodes - 2) / threads + ((nodes - 2) % threads ? 1 : 0);

    pthread_t tid[threads];
    pthread_attr_t pattr;
    pthread_attr_init (&pattr);
    pthread_attr_setscope (&pattr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate (&pattr, PTHREAD_CREATE_JOINABLE);

    pthread_barrier_init(&startBarrier, NULL, 2);
    pthread_barrier_init(&workBarrier, NULL, threads);
    pthread_barrier_init(&endBarrier, NULL, threads + 1);

    for(int i = 0; i < threads; i++) {
        int ret;
        if ( ret=pthread_create (tid + i, &pattr, solve, NULL) )
            perror("pthread_create");
        fprintf (stderr, "ptid = %d\n", tid[i]);
        pthread_barrier_wait(&startBarrier);
    }

    pthread_barrier_wait(&endBarrier);

    pthread_barrier_destroy(&startBarrier);
    pthread_barrier_destroy(&workBarrier);
    pthread_barrier_destroy(&endBarrier);

    for(int j = 0; j < nodes; j++) {
        for(int i = 0; i < nodes; i++)
            printf("%f\t", mainMatrix[i][j]);
        printf("\n");
    }

    for(int j = 0; j < nodes; j++) {
        delete[] mainMatrix[j];
        delete[] tempMatrix[j];
    }

    delete[] mainMatrix;
    delete[] tempMatrix;

    return 0;
}
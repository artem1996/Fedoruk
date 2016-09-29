#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <cmath>

long long mtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    long long mt = (long long)t.tv_sec * 1000 + t.tv_usec / 1000;
    return mt;
}

#define TIME_STEP 0.0001
#define V0 0.0
#define VM 10.0
#define R 1.0
#define C 0.001

int threads;
double full_time;
int nodes;
double tempTime = 0;
double* mainMatrix;
double* tempMatrix;
int work;

pthread_barrier_t endBarrier;
pthread_barrier_t forkBarrier;

void* solve(void*) {
    int myWork = work;
    pthread_barrier_wait(&forkBarrier);
    int start = (nodes - 2) / threads * myWork;
    int end = start + (nodes - 2) / threads;
    if(myWork == 0)
        start = 1;
    if(myWork + 1 == threads)
        end = nodes - 1;
    printf("%d: %d to %d\n", myWork, start, end);
    while(tempTime < full_time) {
        for(int j = start; j < end; j++) {
            tempMatrix[j] = TIME_STEP * (mainMatrix[j - 1] + mainMatrix[j + 1] - 2 * mainMatrix[j]) / R / C + mainMatrix[j];
        }
        pthread_barrier_wait(&endBarrier);
        pthread_barrier_wait(&endBarrier);
    }
    return NULL;
}

int main(int argc, char** argv) {
    FILE *f;
    char buf[1024];
    long long start = mtime();
    if(argc != 4) {
        printf("Usage: ./program_name num_threads time num_nodes\n");
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
    mainMatrix = new double[nodes];
    tempMatrix = new double[nodes];
    tempMatrix[0] = V0;
    mainMatrix[0] = V0;
    tempMatrix[nodes - 1] = VM;
    mainMatrix[nodes - 1] = VM;
    for(int i = 1; i < nodes - 1; i++) {
        mainMatrix[i] = 0.0;
        tempMatrix[i] = 0.0;
    }

    pthread_t tid[threads];
    pthread_attr_t pattr;
    pthread_attr_init (&pattr);
    pthread_attr_setscope (&pattr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate (&pattr, PTHREAD_CREATE_JOINABLE);

    pthread_barrier_init(&endBarrier, NULL, threads + 1);
    pthread_barrier_init(&forkBarrier, NULL, 2);

    for(work = 0; work < threads; ++work) {
        if (pthread_create(tid + work, &pattr, solve, NULL))
            perror("pthread_create");
        fprintf (stderr, "ptid = %d\n", (int)tid[work]);
        pthread_barrier_wait(&forkBarrier);
    }
    pthread_barrier_destroy(&forkBarrier);
    int k = 0;
    while(tempTime < full_time) {
        sprintf(buf, "gnu_%d", k);
        f=fopen(buf, "w");
        for(int i=0; i< nodes; i++)
        {
            fprintf(f, "%f\n", mainMatrix[i]);
        }
        fclose(f);
        k++;
        pthread_barrier_wait(&endBarrier);
        //tempMatrix[0] = TIME_STEP * (mainMatrix[1] - mainMatrix[0]) / R / C + mainMatrix[0];
        double* temp;
        temp = mainMatrix;
        mainMatrix = tempMatrix;
        tempMatrix = temp;
        tempTime += TIME_STEP;
        pthread_barrier_wait(&endBarrier);
    }

    pthread_barrier_destroy(&endBarrier);
    int finalTime = mtime() - start;
    /*if(threads > 3) {
      finalTime *= 2.0/3.0;
    }*/
    printf( "\nProgram takes %d miliSeconds.\n", finalTime);
    delete[] mainMatrix;
    delete[] tempMatrix;
    f=fopen("gnu", "w");
    for(int i=0; i < k; i++) {
        fprintf(f, "plot \"gnu_%d\" with lines\n", i);
       // fprintf(f, "pause %6f \n", TIME_STEP * 10);
    }
    fclose(f);
    system("gnuplot gnu");
    system("rm gnu*");
    return 0;
}

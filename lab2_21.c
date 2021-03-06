#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

long long mtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    long long mt = (long long)t.tv_sec * 1000 + t.tv_usec / 1000;
    return mt;
}

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__

#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define _REENTRANT
#include <pthread.h>
#include <sched.h>
#define N 2
#define X 80
#define Y 100
#define T 100
#define A 0.5

int f( int i, int t)
{
    if(i==4500 && t==1)
    {
	//printf("11111\n");
	return 1;
    }
    else
	return 0;
}

pthread_barrier_t barr;


double *res;
int s;

void * func(void *arg_p)
{
	int It=1, i;
	int num=*((int*)arg_p);
	int size=(X+1)*(Y+1);
	while(It!=T+1)
	{
		for(i=num; i<size; i+=N)
		{
			//printf("%d\n", i);
			if(i<=X || i>=(X+1)*Y || i%(X+1)==0 || i%(X+1)==X)
			{
				res[It*(X+1)*(Y+1)+i]=res[(It-1)*(X+1)*(Y+1)+i];
			}
			else if(It==1)
			{
			    res[It*(X+30)*(Y-10)+i]=0.1*f(i, It)+res[(It-1)*(X+1)*(Y+1)+i];
			    
			    res[It*(X-20)*(Y-20)+i]=0.1*f(i, It)+res[(It-1)*(X-20)*(Y-20)+i];
			    
			    //скорость
			}
			else
			{
				res[It*size+i]=0.1*f(i, It)+A*A*((res[(It-1)*size+i+1]-2*res[(It-1)*size+i]+res[(It-1)*size+i-1])+
				(res[(It-1)*size+i-X-1]-2*res[(It-1)*size+i]+res[(It-1)*size+i+X+1]))+2*res[(It-1)*size+i]-res[(It-2)*size+i];
				res[It*size+i]=0.1*f(i, It)+A*A*((res[(It-1)*size+i+1]-2*res[(It-1)*size+i]+res[(It-1)*size+i-1])+
				(res[(It-1)*size+i-X-1]-2*res[(It-1)*size+i]+res[(It-1)*size+i+X+1]))+2*res[(It-1)*size+i]-res[(It-2)*size+i];
			}
		}
		//printf("%d:%d\n", i, It);
		It++;
		pthread_barrier_wait(&barr);
	}
}

int main()
{
	FILE *f;
    long long startTime = mtime();
	char buf[1024];
	pthread_t *id;
	pthread_attr_t pattr;
	struct timeval start, end;
    int ret, *param, i, j, k;
    pthread_attr_init (&pattr);
    pthread_attr_setscope (&pattr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate (&pattr,PTHREAD_CREATE_JOINABLE);
    pthread_barrier_init(&barr, NULL, N);
    id=malloc(N*sizeof(pthread_t));
    param=malloc(N*sizeof(int));
    res=malloc((T+1)*(X+1)*(Y+1)*sizeof(double));
    for(i=0; i<N; i++)
    {
    	param[i]=i;
    }
    if(((X+1)*(Y+1))%N==0)
    {
    	s=((X+1)*(Y+1))/N-1;
    }
    else
    {
    	s=((X+1)*(Y+1))/N;
    }
    gettimeofday(&start, 0);
    for(i=0; i<N; i++)
    {
    	 if ( ret=pthread_create (&id[i], &pattr, func, (void*)&param[i]) )
    		perror("pthread_create");
    }
    for(i=0; i<N; i++)
    	pthread_join(id[i], NULL);
    gettimeofday(&end, 0);
    //printf("%u.%u\n", end.tv_sec-start.tv_sec, end.tv_usec-start.tv_usec);
   //rintf("111\n");
    for(k=0; k<T+1; k++)
    {
    	sprintf(buf, "gnu_%d", k);
    	f=fopen(buf, "w");
    	for(j=0; j<Y+1; j++)
    	{
    		for(i=0; i<X+1; i++)
    		{
    			//printf("%f ", res[k*(X+1)*(Y+1)+j*(X+1)+i]);
    			fprintf(f, "%d %d %f\n", i, j, res[k*(X+1)*(Y+1)+j*(X+1)+i]);
    		}
    		//printf("\n");
    	}
    	//printf("%d\n", k);
    	fclose(f);
    	//printf("\n\n");
    }
   //rintf("111\n");
    printf( "\nProgram takes %d miliSeconds.\n", (int)(mtime() - startTime));

    f=fopen("com", "w");
    for(k=0; k<T+1; k++)
    {
	fprintf(f, "set zrange [-0.1:0.1]\n");
	fprintf(f, "splot [0:%d][0:%d]\"gnu_%d\" w l palette\n", X, Y, k);
	fprintf(f, "pause 0.1\n");
    }
    fclose(f);
    system("gnuplot com");
}

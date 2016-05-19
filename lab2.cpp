#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h> 
#include <iostream>
using namespace std; 

#define R 10.
#define C 0.1
#define LAM 0.01

double *I;
double *Iprev;

typedef struct someArgs_tag {
    int id;
    int from;
    int to;
    int x;
} someArgs_t;

void* met(void *args) { //old
	
	someArgs_t *arg = (someArgs_t*) args;
	//printf("potok %d %d\n", arg->from, arg->to);
	for(int i=(arg->from); i<(arg->to); i++) {
		
		
		if(i==0) {
     	I[i]=(Iprev[i-1]-Iprev[i])*LAM/(R*C)+Iprev[i];
			cout<<"I= "<<I[i]<<endl;
			
			return 0;
		}
		if(i==arg->x-1) {
     	//I[i]=(Iprev[i-1]-Iprev[i])*LAM/(R*C)+Iprev[i];
		//	cout<<"I= "<<I[i]<<endl;
			
		//	return 0;
		}
			
		else
			I[i]=(Iprev[i+1]-2.*Iprev[i]+Iprev[i-1])*LAM/(R*C)+Iprev[i];
		
			
	}	
	return 0;
}


/*void* met(void *args) {
	
	someArgs_t *arg = (someArgs_t*) args;
	//printf("potok %d %d\n", arg->from, arg->to);
	for(int i=(arg->from); i>(arg->to); i--) {
		if(i==0) {
			I[i]=(Iprev[i-1]+Iprev[i])*LAM/(R*C)+Iprev[i];
			//cout<<"I= "<<I[i]<<endl;
		}
			
		else{
			I[i]=(Iprev[i-1]-2.*Iprev[i]+Iprev[i+1])*LAM/(R*C)+Iprev[i];
		//	cout<<"I= "<<I[i]<<endl;
			}
	}	
	return 0;
}*/

void* met2(void *args) { // old 
	someArgs_t *arg = (someArgs_t*) args;
	for(int i=(arg->from); i<(arg->to); i++) {
		Iprev[i]=I[i];
	}
}

/*void* met2(void *args) {
	someArgs_t *arg = (someArgs_t*) args;
	for(int i=(arg->from); i>(arg->to); i--) {
		Iprev[i]=I[i];
	}
}*/
	
	char filename[9];

int main(int argc, char **argv) {
	FILE *fd;
	FILE *ffd;
	
	
	pthread_t *threads;
	someArgs_t *args;
	pthread_attr_t pattr;
	
	filename[0]='a';
	filename[4]='.';
	filename[5]='d';
	filename[6]='a';
	filename[7]='t';
	filename[8]='\0';
	pthread_attr_init(&pattr);
	pthread_attr_setscope(&pattr, PTHREAD_SCOPE_SYSTEM);
	double t=0;
	double b=atof(argv[1]);	
	int x=atoi(argv[2]);
	int tr=atoi(argv[3]);
	I=(double*)calloc(x, sizeof(double));
	Iprev=(double*)calloc(x, sizeof(double));
	
	threads=(pthread_t*)calloc(tr, sizeof(pthread_t));
	args=(someArgs_t*)calloc(tr, sizeof(someArgs_t));
	int status_addr;
	
	
	ffd=fopen("plot.dat", "w");
	fclose(ffd);
	
	fd=fopen(filename, "w");
	fclose(fd);
	
	for(int i=0; i<tr; i++) {// old
	
		args[i].id=i;
		if(i==0) {
	//	cout<<"i="<<i<<endl;
			args[i].from=1;
			// cout<<"from st"<<args[i].from<<endl;
			args[i].to=x/tr;
			// cout<<"to st"<<args[i].to<<endl;
			args[i].x=x;
			continue;
		}
	//	cout<<"i="<<i<<endl;
		args[i].from=args[i-1].to;
	//	cout<<"from "<<args[i].from<<endl;
		args[i].to=args[i].from+x/tr;
	//	cout<<"to "<<args[i].to<<endl;
		args[i].x=x;
	}
	
	/*for(int i=0; i<tr; i++) {// left source
	
		args[i].id=i;
		if(i==0) {
		cout<<"i="<<i<<endl;
			args[i].from=x-1;
			// cout<<"from st"<<args[i].from<<endl;
			args[i].to=x-x/tr-1;
			// cout<<"to st"<<args[i].to<<endl;
			args[i].x=x;
			continue;
		}
		if (i==tr-1){
			//cout<<"i="<<i<<endl;
			args[i].from=args[i-1].to;
		//	cout<<"from "<<args[i].from<<endl;
			args[i].to=0;
			//cout<<"to "<<args[i].to<<endl;
			args[i].x=x;
			
		}
		else {
		//	cout<<"i="<<i<<endl;
			args[i].from=args[i-1].to;
		//	cout<<"from "<<args[i].from<<endl;
			args[i].to=args[i].from-x/tr;
		//	cout<<"to "<<args[i].to<<endl;
			args[i].x=x;
		}
	}*/
	
	/*for(int i=0;i<x;i++){
		cout<<"from "<<args[i].from<<endl;
		cout<<"to "<<args[i].to<<endl;
		cout<<"x "<<args[i].x<<endl;
		cout<<"id "<<args[i].id<<endl;
		cout<<endl;
		
		
	}*/
	
	
	
	I[x-1]=Iprev[x-1]=200.;
	//I[0]=Iprev[0]=200.;
	char a='A';
	char a1='A';
	char a2='A';
	int status=0;
	char n=0;
	
	struct timeval tim;  
    	gettimeofday(&tim, NULL);  
    	double t1=tim.tv_sec+(tim.tv_usec/1000000.0);  
	
	for(t=0; t<b; t+=LAM, n++) {
				if(n==10)
			n=0;
		if(n==0) {
			filename[2]=a1;
			filename[3]=a2;
			filename[1]=a;
			
		}
		for(int i=0; i<tr; i++) {
		
			status=pthread_create(&threads[i], &pattr, met, (void*) &args[i]);
			if (status != 0) {
            			printf("main error: can't create thread, status = %d\n", status);
            			exit(-1);
        		}
		}
		
		for (int i = 0; i < tr; i++) {
    		status = pthread_join(threads[i], (void**)&status_addr);
    		if (status != 0) {
       			 printf("main error: can't join thread, status = %d\n", status);
       			 exit(-1);
   		 }
    			//printf("joined with address %d\n", status_addr);
		}
		//распаралеллить
		for(int i=0; i<tr; i++) {
		
			status=pthread_create(&threads[i], &pattr, met2, (void*) &args[i]);
			if (status != 0) {
            			printf("main error: can't create thread, status = %d\n", status);
            			exit(-1);
        		}
		}
			
		//
		for (int i = 0; i < tr; i++) {
    		status = pthread_join(threads[i], (void**)&status_addr);
    		if (status != 0) {
       			 printf("main error: can't join thread, status = %d\n", status);
       			 exit(-1);
   		 }
		}
		if(n==0) {
			for(int i=0; i<x; i++){
				fd=fopen(filename, "a");
				printf("%s\n",filename);
				fprintf(fd, "%d %f\n", i, Iprev[i]);
				fclose(fd);
				//fprintf("%d %f\n", i, Iprev[i]);
}
			ffd=fopen("plot.dat", "a");
			fprintf(ffd, "plot \"%s\" with lines\npause 0.2\n", filename);
			fclose(ffd);
			int t=0;

			a++;
			if(a=='Z') {
				a='A';
				a1++;
			}
			if(a1=='Z'&&a=='Z') {
				a='A';
				a1='A';
				a2++;
			}
		}
	} 
	gettimeofday(&tim, NULL);  
    	double t2=tim.tv_sec+(tim.tv_usec/1000000.0);  
   	printf("%.6lf seconds elapsed\n", t2-t1);

}


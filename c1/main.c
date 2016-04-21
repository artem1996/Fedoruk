#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

/*
 fork - 1
 read
 exit - 1
 kill
 signal
 alarm
 pause

 SIGUSR1
 SIGUSR2
 SIGINT
 SIGQUIT
 */

void sigHandler(int sig)
{
    static int timer = 1;
    switch (sig) {
        case SIGALRM:
            write(1, "UP!\n", 4);
            alarm(timer);
            break;
        case SIGUSR1:
            write(1, "Timer is changed\n", 17);
            if(timer == 1)
                timer = 3;
            else
                timer = 1;
            break;
        case SIGUSR2:
            write(1, "Bay Bay\n", 8);
            exit(0);
        case SIGINT:
        case SIGQUIT:
            write(1, "Sosni\n", 6);
            break;
    }
}

pid_t createHandler()
{
    printf("start pid: %d\n", getpid());
    pid_t  p = fork();
    if (p == 0) {
        while (1);
    } else {
        char done = 1;
        kill(p, SIGALRM);
        while (done){
            char byte;
            byte = getchar();
            if(byte == EOF) {
                done = 0;
                kill(p, SIGUSR2);
            }
            if(byte == '\n')
                kill(p, SIGUSR1);
        }
    }
    return getpid();
}


int main()
{
    signal(SIGQUIT, sigHandler);
    signal(SIGINT, sigHandler);
    signal(SIGALRM, sigHandler);
    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);

    createHandler();

    return 0;
}


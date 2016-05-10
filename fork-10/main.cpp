#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <termio.h>
#include <csignal>

#define N 15
#define M 20

char word = 'A';
int textmode(int mode) {
    static struct termios con[2];
    if(mode > 0)
        return(tcsetattr(0, TCSAFLUSH, &con[1]));
    tcgetattr(0, &con[0]);
    tcgetattr(0, &con[1]);
    con[0].c_lflag &= ~(ICANON | ECHO | ISIG);
    con[0].c_iflag &= ~(ISTRIP | IXOFF | IXANY | IXON);
    con[0].c_oflag |= CS8;
    con[0].c_cc[VMIN] = 2;
    con[0].c_cc[VTIME] = 1;
    return(tcsetattr(0, TCSAFLUSH, &con[0]));
}

/* clear standart text mode window */

void clrscr() {
    unsigned char esc[11];

/* obtain set cursor to left-top window corner */

    esc[0] = 27; esc[1] = '[';
    esc[2] = 'H';
    write(1,esc,3);

/* obtain clear escape sequence */

    esc[2] = '2'; esc[3] = 'J';
    write(1,esc,4);

    return;

}

/* position cursor in standart text window */

void gotoxy(int tx, int ty, char c) {
    tx += 3;
    ty += 2;
    unsigned char esc[16];
    static unsigned char ystr[3]; /* vertical cursor location */
    static unsigned char xstr[3]; /* horizontal cursor location */
    int i;                        /* ESC-sequence current index */
    int j;                        /* cursor location current index */

/* convert cursor location to text format */

    if((tx > 99) || (ty > 99))
        tx = ty = 99;
    if((tx < 1) || (ty < 1))
        tx = ty = 1;
    xstr[0] = xstr[1] = xstr[2] = '\0';
    ystr[0] = ystr[1] = ystr[2] = '\0';

    sprintf((char *) xstr,"%d",tx);
    sprintf((char *) ystr,"%d",ty);

/* obtain goto escape sequence */

    esc[0] = 27; esc[1] = '[';
    i=2; j=0;
    while(ystr[j])
        esc[i++]=ystr[j++];
    j=0;
    esc[i++]=';';
    while(xstr[j])
        esc[i++]=xstr[j++];
    esc[i++]='H';
    esc[i++]='\b';
    esc[i++] = c;
    esc[i] = '\0';
    write(1,esc,i);

    return;
}

void action(int sig) {
    static int curx = 0;
    static int cury = 0;
    static int prev_sig = 0;
    static int way = 0;
    if(sig == SIGQUIT) {
        gotoxy(curx, cury, ' ');
        textmode(1);
        word = 0;
        exit(0);
    }
    if(sig == SIGUSR1 || sig == SIGUSR2) {
        if (prev_sig) {
            if (prev_sig == SIGUSR1) {
                switch (sig) {
                    case SIGUSR1:
                        way = 1;
                        break;
                    case SIGUSR2:
                        way = 2;
                        break;
                }
            } else {
                switch (sig) {
                    case SIGUSR1:
                        way = 3;
                        break;
                    case SIGUSR2:
                        way = 4;
                        break;
                }
            }
            prev_sig = 0;
        } else {
            prev_sig = sig;
            return;
        }
    }
    switch(way) {
        case 1: if(cury > 0) {
                gotoxy(curx, cury, ' ');
                gotoxy(curx, --cury, word);
            }
            else way = 0;
            break;
        case 2: if(cury < M - 1) {
                gotoxy(curx, cury, ' ');
                gotoxy(curx, ++cury, word);
            }
            else way = 0;
            break;
        case 3: if(curx > 0) {
                gotoxy(curx, cury, ' ');
                gotoxy(--curx, cury, word);
            }
            else way = 0;
            break;
        case 4: if(curx < N - 1) {
                gotoxy(curx, cury, ' ');
                gotoxy(++curx, cury, word);
            }
            else way = 0;
            break;
    }
    alarm(1);
    sleep(1);
}

class manipulator {
    manipulator *next, *prev;
    pid_t pid;
public:
    manipulator(manipulator *tprev, pid_t tpid) {
        if(tprev == NULL) {
            next = prev = this;
        } else {
            next = tprev -> next;
            prev = tprev;
            prev -> next = this;
            next -> prev = this;
        }
        pid = tpid;
    };
    ~manipulator() {
        next->prev = prev;
        prev->next = next;
    }
    manipulator* getnext() {
        return next;
    }
    pid_t getpid() {
        return pid;
    }
};

int main(int argc, char* argv[]) {
    clrscr();
    for(int i = 0; i < M + 2; i++) {
        if(i == 0 || i == M + 1) {
            for(int j = 0; j < N + 2; j++) {
                write(1, "#", 1);
            }
        } else {
            write(1, "#", 1);
            for(int j = 0; j < N; j++) {
                write(1, " ", 1);
            }
            write(1, "#", 1);
        }
        write(1, "\n", 1);
    }
    textmode(0);
    int done = -1;
    manipulator *current = NULL;
    while(done)
        if(done == -1) {
            char temp;
            temp = getchar();
            switch(temp) {
                case 'q': while (current != NULL) {
                        kill(current->getpid(), SIGQUIT);
                        manipulator *tempcur = current;
                        current = current -> getnext() == current ? NULL : current->getnext();
                        delete tempcur;
                    }
                    done = 0;
                    break;
                case 'w': if(current == NULL) break;
                    kill(current -> getpid(), SIGUSR1);
                    sleep(1);
                    kill(current -> getpid(), SIGUSR1);
                    break;
                case 's': if(current == NULL) break;
                    kill(current -> getpid(), SIGUSR1);
                    sleep(1);
                    kill(current -> getpid(), SIGUSR2);
                    break;
                case 'a': if(current == NULL) break;
                    kill(current -> getpid(), SIGUSR2);
                    sleep(1);
                    kill(current -> getpid(), SIGUSR1);
                    break;
                case 'd': if(current == NULL) break;
                    kill(current -> getpid(), SIGUSR2);
                    sleep(1);
                    kill(current -> getpid(), SIGUSR2);
                    break;
                case 'n': done = fork();
                    if(done) {
                        current = new manipulator(current, done);
                        done = -1;
                        word++;
                        sleep(1);
                    } else done = 1;
                    textmode(0);
                    break;
                case 'r': if(current != NULL) {
                        kill(current->getpid(), SIGQUIT);
                        manipulator *tempcur = current->getnext() == current ? NULL : current->getnext();
                        delete current;
                        current = tempcur;
                    }
                    break;
                case 'c': if(current != NULL) {current = current -> getnext();} break;
                default: break;
            }
        } else {
            signal(SIGQUIT, action);
            signal(SIGUSR1, action);
            signal(SIGUSR2, action);
            signal(SIGALRM, action);
            gotoxy(0, 0, word);
            alarm(0);
            while(word);
        }
    gotoxy(N, M + 1, '\n');
    textmode(1);
}
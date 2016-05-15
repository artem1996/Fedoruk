#include <termios.h>
#include <cstdio>
#include <zconf.h>
#include <csignal>

static int done = 1;
int queue;

int textmode(int status) {
    static struct termios savetty;
    struct termios tty;
    tcgetattr (0, &tty);
    if(status) {
        savetty = tty; /* Сохранить упр. информацию канонического режима */
        tty.c_lflag &= ~(ICANON | ECHO | ISIG);
        tty.c_cc[VMIN] = 1;
        tcsetattr(0, TCSAFLUSH, &tty);
    } else {
        tcsetattr(0, TCSAFLUSH, &savetty);
    }
    return 0;
}

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

void gotoxy(int tx, int ty, char c) {
    tx += 3 + 2 + tx * 2;
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

void sigQuit(int sig) {
    done = 0;
}

void liftProc(int sig) {
    printf("%d ", done);
    static int floor = 1;
    static int direct = 1;
    if(1 << (floor - 1) & queue) {
        queue &= (~0 ^ (1 << (floor - 1)));
        gotoxy(floor, done, 'O');
        alarm(1);
        return;
    }
    if(queue) {
        if(1 << (floor - 1) < queue && direct >= floor) {
            gotoxy(floor, done, '-');
            gotoxy(direct, done, 'X');
            floor = direct;
            direct++;
            if(1 << (direct - 1) > queue)
                direct -= 2;
        }
        if(queue % (1 << (floor - 1))&& direct <= floor) {
            gotoxy(floor, done, '-');
            gotoxy(direct, done, 'X');
            floor = direct;
            direct--;
            if(!(queue % (1 << (floor - 1))))
                direct += 2;
        }
    } else {
        direct = floor;
    }
    alarm(done);
    return;
}

int liftQueue() {
    char floor;
    alarm(1);
    while(done) {
        read(0, &floor, 1);
        putchar(floor + '0');
        queue |= (1 << (floor - 1));
    }
}

int textProc(char symbol, char &floor) {
    if(symbol >= '0' && symbol<= '9') {
        floor = symbol - '0' ? symbol - '0' : 10;
        return 2;
    }
    switch(symbol) {
        case 'q': floor = 1; return 0;
        case 'w': floor = 2; return 0;
        case 'e': floor = 3; return 0;
        case 'r': floor = 4; return 0;
        case 't': floor = 5; return 0;
        case 'y': floor = 6; return 0;
        case 'u': floor = 7; return 0;
        case 'i': floor = 8; return 0;
        case 'o': floor = 9; return 0;
        case 'p': floor = 10; return 0;
        case 'a': floor = 1; return 1;
        case 's': floor = 2; return 1;
        case 'd': floor = 3; return 1;
        case 'f': floor = 4; return 1;
        case 'g': floor = 5; return 1;
        case 'h': floor = 6; return 1;
        case 'j': floor = 7; return 1;
        case 'k': floor = 8; return 1;
        case 'l': floor = 9; return 1;
        case ';': floor = 10; return 1;
    }
    return -1;
}

int main() {
    clrscr();
    textmode(1);
    int passLift[2];
    int cargoLift[2];
    pipe(passLift);
    pipe(cargoLift);
    printf("ЛП X - - - - - - - - -\nЛГ X - - - - - - - - -");
    int lifts;
    if(fork() == 0) {
        dup2(passLift[0], 0);
        close(passLift[0]);
        close(passLift[1]);
        textmode(1);
        signal(SIGINT, sigQuit);
        signal(SIGALRM, liftProc);
        liftQueue();
        textmode(0);
        return 0;
    }
    done++;
    if(fork() == 0) {
        textmode(1);
        dup2(cargoLift[0], 0);
        close(cargoLift[0]);
        close(cargoLift[1]);
        signal(SIGINT, sigQuit);
        signal(SIGALRM, liftProc);
        liftQueue();
        textmode(0);
        return 0;
    }
    close(passLift[1]);
    close(cargoLift[1]);
    signal(SIGINT, sigQuit);
    while(done) {
        char symbol;
        char floor;
        symbol = getchar();
        lifts = textProc(symbol, floor);
        if(lifts == 2) {
            write(passLift[1], &floor, 1);
            write(cargoLift[1], &floor, 1);
        }
        if(lifts == 0) {
            write(passLift[1], &floor, 1);
        }
        if(lifts == 1) {
            write(cargoLift[1], &floor, 1);
        }
    }
    close(passLift[0]);
    close(cargoLift[0]);
    textmode(0);
    return 0;
}
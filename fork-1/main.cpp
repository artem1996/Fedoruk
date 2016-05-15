#include <unistd.h>

int main(int argc, char** argv) {
    if(argc != 3) {
        write(1, "Неверное количество аргументов!", argc);
        return -1;
    }
    int fds[2];
    pipe(fds);
    if(fork()) {
        dup2(fds[1], 1);
        close(fds[0]);
        close(fds[1]);
        execl("/bin/tar", "tar", "-c", argv[1], NULL);
    } else {
        chdir(argv[2]);
        dup2(fds[0], 0);
        close(fds[0]);
        close(fds[1]);
        execl("/bin/tar", "tar", "-x", NULL);
    }
    return 0;
}
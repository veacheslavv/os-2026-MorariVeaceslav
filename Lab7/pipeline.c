#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int
main(void)
{
    int p[2];
    if (pipe(p) < 0) { perror("pipe"); exit(1); }

    pid_t ls = fork();
    if (ls == 0) {
        close(p[0]);
        dup2(p[1], 1);
        close(p[1]);
        execlp("ls", "ls", NULL);
        perror("exec ls");
        exit(1);
    }

    pid_t wc = fork();
    if (wc == 0) {
        close(p[1]);
        dup2(p[0], 0);
        close(p[0]);
        execlp("wc", "wc", "-l", NULL);
        perror("exec wc");
        exit(1);
    }

    close(p[0]);
    close(p[1]);
    waitpid(ls, NULL, 0);
    waitpid(wc, NULL, 0);
    return 0;
}
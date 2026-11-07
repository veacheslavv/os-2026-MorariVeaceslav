#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    int p1[2], p2[2];
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        char b;
        close(p1[1]);
        close(p2[0]);
        if (read(p1[0], &b, 1) != 1) {
            fprintf(2, "child: read failed\n");
            exit(1);
        }
        printf("%d: received ping\n", getpid());
        if (write(p2[1], &b, 1) != 1) {
            fprintf(2, "child: write failed\n");
            exit(1);
        }
        close(p1[0]);
        close(p2[1]);
        exit(0);
    }

    char b = 'x';
    close(p1[0]);
    close(p2[1]);
    if (write(p1[1], &b, 1) != 1) {
        fprintf(2, "parent: write failed\n");
        exit(1);
    }
    if (read(p2[0], &b, 1) != 1) {
        fprintf(2, "parent: read failed\n");
        exit(1);
    }
    printf("%d: received pong\n", getpid());
    close(p1[1]);
    close(p2[0]);
    wait(0);
    exit(0);
}

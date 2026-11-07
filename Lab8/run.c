#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        printf("child: pid=%d\n", getpid());
        char *args[] = {"echo", "hello from exec", 0};
        exec("echo", args);
        fprintf(2, "exec failed\n");
        exit(1);
    }

    printf("parent: child=%d\n", pid);
    int status;
    wait(&status);
    printf("parent: child exited with %d\n", status);
    exit(0);
}

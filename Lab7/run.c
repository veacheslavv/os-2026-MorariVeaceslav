#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int
main(void)
{
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(1); }

    if (pid == 0) {
        printf("child: pid=%d, parent=%d\n", getpid(), getppid());
        execlp("ls", "ls", "-l", NULL);
        perror("exec");
        exit(1);
    }

    printf("parent: pid=%d, child=%d\n", getpid(), pid);
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        printf("parent: child exited with %d\n", WEXITSTATUS(status));
    return 0;
}
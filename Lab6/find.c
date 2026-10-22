#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char *name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_DEVICE:
    case T_FILE:
        p = path + strlen(path);
        while(p > path && *(p - 1) != '/')
            p--;
        if(strcmp(p, name) == 0)
            printf("%s\n", path);
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
                continue;

            if(stat(buf, &st) < 0){
                fprintf(2, "find: cannot stat %s\n", buf);
                continue;
            }

            if(strcmp(p, name) == 0)
                printf("%s\n", buf);

            if(st.type == T_DIR)
                find(buf, name);
        }
        break;
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(2, "Usage: find <dir> <name>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}

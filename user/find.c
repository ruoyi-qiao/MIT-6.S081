#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/*
Look at user/ls.c to see how to read directories.
Use recursion to allow find to descend into sub-directories.
Don't recurse into "." and "..".
Changes to the file system persist across runs of qemu; to get a clean file system run make clean and then make qemu.
You'll need to use C strings. Have a look at K&R (the C book), for example Section 5.5.
Note that == does not compare strings like in Python. Use strcmp() instead.
Add the program to UPROGS in Makefile.

 $ make qemu
    ...
    init: starting sh
    $ echo > b
    $ mkdir a
    $ echo > a/b
    $ find . b
    ./b
    ./a/b
 $ 
*/
char* getname(char *path) {
    static char buf[DIRSIZ + 1];
    char *p;
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    if (strlen(p) >= DIRSIZ) 
        return p;
    memmove(buf, p, strlen(p));
    *(buf + strlen(p)) = '\0';
    return buf;
}
void find(char *path, const char *name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
    }

    switch (st.type)
    {
    case T_FILE:
        if (strcmp(getname(path), name) == 0) {
            printf("%s\n", path);
        }
        break;
    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/'; 
        p[0] = 0;
        int x;
        while ((x = read(fd, &de, sizeof(de))) == sizeof(de)) {
            if (de.inum == 0) {
                continue;
            }
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                fprintf(2, "find: cannot find %s\n", buf);
            }
            find(buf, name);
        }
        break;
    default:

        break;
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("usage: find <path> <name>\n");
        exit(1);
    }
    char *cur_path = malloc(strlen(argv[1]) + 1);
    strcpy(cur_path, argv[1]);
    find(cur_path, argv[2]);
    exit(0);
}
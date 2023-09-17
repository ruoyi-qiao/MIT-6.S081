#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);
    char buf[5];
    
    if (fork() != 0) {
        if (write(p1[1], "ping", 4) != 4) {
            printf("write to child error\n");
            exit(1);
        }
        if (read(p2[0], buf, 4) != 4) {
            printf("read from child error\n");
            exit(1);
        }
        printf("%d: received %s\n", getpid(), buf);
    } else {
        if (read(p1[0], buf, 4) != 4) {
            printf("read from parent error\n");
            exit(1);
        }
        printf("%d: received %s\n", getpid(), buf);
        if (write(p2[1], "pong", 4) != 4) {
            printf("write to parent error\n");
            exit(1);
        }
        exit(0);
    }
    // parent close all pipe fd
    close(p1[0]);
    close(p1[1]);
    close(p2[0]);
    close(p2[1]);
    exit(0);
}
/*
 *  1 : write end fd 0 : read end fd
 *
 *  parent       child
 *  p1[1]  --->  p1[0]
 *  p2[0]  <---  p2[1]
 * 
*/
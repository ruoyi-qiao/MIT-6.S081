#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
/*
 *  p = get a number from left neighbor
 *  print p
 *  loop:
 *      n = get a number from left neighbor
 *      if (p does not divide n)
 *          send n to right neighbor
*/
int main(int argc, char* argv[]) {
    int pid;
    int p[2];
    pipe(p);
    if ((pid = fork()) != 0) {
        int pw = p[1], pr = p[0];
        close(pr);
        for (int i = 2; i <= 35; i++) {
            if (write(pw, &i, sizeof(i)) != sizeof(i)) {
                printf("write to child error\n");
                exit(1);
            }
        }
        close(pw);
        wait(0);
        exit(0);
    } else {
        int pw = p[1], pr = p[0];
        close(pw);
        while(1) {
            int n, sz;
            if ((sz = read(pr, &n, sizeof(n))) != sizeof(n)) {
                close(pr);
                if (sz == 0) {
                    exit(0);
                }
                printf("read from parent error : <sz> %d\n", sz);
                exit(1);
            } else {
                printf("prime %d from %d\n", n, getpid());
                int nxt, pid;
                int np[2];
                pipe(np);
                if ((pid = fork()) == 0) {
                    close(np[1]);
                    pr = np[0];
                } else {
                    close(np[0]);
                    pw = np[1];
                    for (; read(pr, &nxt, sizeof(nxt)) == sizeof(nxt);) {
                        if (nxt % n != 0) {
                            if (write(pw, &nxt, sizeof(nxt)) != sizeof(nxt)) {
                                printf("write to parent error\n");
                                exit(1);
                            }
                        }
                    }
                    close(pw);
                    close(pr);
                    wait(0);
                    exit(0);
                }
            }
        }
    }
    return 0;
}
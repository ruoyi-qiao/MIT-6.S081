#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
/*
The following example illustrates xarg's behavior:
    $ echo hello too | xargs echo bye
    bye hello too
    $
  
Note that the command here is "echo bye" and the additional arguments are "hello too", making the command "echo bye hello too", which outputs "bye hello too".
Please note that xargs on UNIX makes an optimization where it will feed more than argument to the command at a time. We don't expect you to make this optimization. To make xargs on UNIX behave the way we want it to for this lab, please run it with the -n option set to 1. For instance

    $ echo "1\n2" | xargs -n 1 echo line
    line 1
    line 2
    $
  
Changes to the file system persist across runs of qemu; to get a clean file system run make clean and then make qemu.
xargs, find, and grep combine well:

  $ find . b | xargs grep hello
  
will run "grep hello" on each file named b in the directories below ".".
To test your solution for xargs, run the shell script xargstest.sh. Your solution is correct if it produces the following output:

  $ make qemu
  ...
  init: starting sh
  $ sh < xargstest.sh
  $ $ $ $ $ $ hello
  hello
  hello
  $ $   
  
You may have to go back and fix bugs in your find program. The output has many $ because the xv6 shell doesn't realize it is processing commands from a file instead of from the console, and prints a $ for each command in the file.
*/
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "usage: xargs <prog> [<argslist>]\n");
        exit(1);
    }
    int cur = 0;
    int n, it = 0, nxt;
    char buf[512], argline[512];
    char* args[MAXARG], *p = argline;
    for (int i = 1; i < argc; i++) {
        args[cur++] = argv[i];
    }
    nxt = cur;
    while((n = read(0, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                argline[it++] = 0;
                args[nxt++] = p;

                nxt = cur;
                if (fork() == 0) {
                    exec(argv[1], args);
                } 
                else {
                    wait(0);
                }
            } else if (buf[i] == ' ') {
                argline[it++] = 0;
                args[nxt++] = p;
                p = &argline[it];
            } else {
                argline[it++] = buf[i];
            }
        }
    }
    exit(0);
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

#define MAXLINE 512  

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "Usage: xargs [-n num] command [args...]\n");
        exit(1);
    }

    int maxargs = 1;  
    int cmd_start = 1; 

    if (strcmp(argv[1], "-n") == 0) {
        if (argc < 4) {
            fprintf(2, "Usage: xargs [-n num] command [args...]\n");
            exit(1);
        }
        maxargs = atoi(argv[2]);
        cmd_start = 3;  
    }

    char buf[MAXLINE];
    char *newargv[MAXARG];
    int n = 0;
    
    for (n = 0; n < argc - cmd_start; n++) {
        newargv[n] = argv[cmd_start + n];
    }

    int argcount = n;

    while (1) {

        int i = 0;
        int bytesRead = 0;  

        while (i < MAXLINE) {
            bytesRead = read(0, &buf[i], 1);  
            if (bytesRead < 1) {
                break; 
            }
            if (buf[i] == '\n') {
                buf[i] = 0; 
                break;
            }
            i++;
        }

        if (i == 0 && bytesRead < 1) {
            break;
        }

        newargv[argcount++] = buf;

        if (argcount - n >= maxargs || bytesRead < 1) {
            newargv[argcount] = 0;  

            if (fork() == 0) {
                exec(newargv[0], newargv);
                fprintf(2, "exec %s failed\n", newargv[0]);
                exit(1);
            } else {
               
                wait(0);
            }

            argcount = n;
        }
    }

    if (argcount > n) {
        newargv[argcount] = 0;  

        if (fork() == 0) {
            exec(newargv[0], newargv);
            fprintf(2, "exec %s failed\n", newargv[0]);
            exit(1);
        } else {
           
            wait(0);
        }
    }

    exit(0);
}

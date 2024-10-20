#include "kernel/types.h"
#include "user/user.h"

void primes(int left_pipe)
{
    int prime;

    while (1) {
        if (read(left_pipe, &prime, sizeof(prime)) == 0) {
            close(left_pipe);
            exit(0); 
        }

        printf("prime %d\n", prime);

        int num;
        int right_pipe[2];

        if (pipe(right_pipe) < 0) {
            fprintf(2, "Error: cannot create pipe\n");
            exit(1);
        }

        int pid = fork();
        if (pid < 0) {
            fprintf(2, "Error: fork failed\n");
            exit(1);
        } else if (pid == 0) {
            close(left_pipe);          
            close(right_pipe[1]);      
            left_pipe = right_pipe[0]; 
        } else {
            close(right_pipe[0]);      

            while (read(left_pipe, &num, sizeof(num)) > 0) {
                if (num % prime != 0) {
                    write(right_pipe[1], &num, sizeof(num));  
                }
            }

            close(left_pipe);
            close(right_pipe[1]);

            wait(0);
            exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    int initial_pipe[2];

    if (pipe(initial_pipe) < 0) {
        fprintf(2, "Error: cannot create initial pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork failed\n");
        exit(1);
    } else if (pid == 0) {
        close(initial_pipe[1]);    
        primes(initial_pipe[0]);  
    } else {
        close(initial_pipe[0]);   

        
        for (int i = 2; i <= 280; i++) {
            write(initial_pipe[1], &i, sizeof(i));
        }

        
        close(initial_pipe[1]);

        
        wait(0);
        exit(0);
    }
}

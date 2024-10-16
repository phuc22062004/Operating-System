#include "kernel/types.h"
#include "user/user.h"

void primes() {
    int prime_pipe[2];
    int prime_number;

    if (read(0, (void*)&prime_number, sizeof(prime_number)) <= 0)
        return;

    printf("prime %d\n", prime_number);

    if (pipe(prime_pipe) < 0) {
        fprintf(2, "Error: cannot create pipe\n");
        exit(1);
    }

    int pid = fork(); 
    if (pid == 0) {
        close(0);               
        dup(prime_pipe[0]);    
        close(prime_pipe[0]);   
        close(prime_pipe[1]);   
        primes();               
    } else {
        close(1);               
        dup(prime_pipe[1]);     
        close(prime_pipe[0]);  
        close(prime_pipe[1]);  

        int next_number = 0;
        while (read(0, (void*)&next_number, sizeof(next_number)) > 0) {
            if (next_number % prime_number != 0) {
                write(1, (void*)&next_number, sizeof(next_number));  
            }
        }
        close(1); 
        wait(&pid);  
    }
}

int main(void) {
    int main_pipe[2]; 
    if (pipe(main_pipe) < 0) {
        fprintf(2, "Error: cannot create pipe\n");
        exit(1);
    }

    int pid = fork();  
    if (pid == 0) {
        close(0);              
        dup(main_pipe[0]);    
        close(main_pipe[0]);   
        close(main_pipe[1]);  
        primes();              
    } else {
        close(1);            
        dup(main_pipe[1]);     
        close(main_pipe[0]);   
        close(main_pipe[1]);   

        for (int i = 2; i <= 280; i++) {
            write(1, (void*)&i, sizeof(i));
        }

        close(1);  
        wait(&pid); 
    }
    exit(0);
}

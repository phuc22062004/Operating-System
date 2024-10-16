#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
    int parent_to_child_pipe[2], child_to_parent_pipe[2];
    char byte = 'a';
    char buffer[1];

    if (pipe(parent_to_child_pipe) < 0 || pipe(child_to_parent_pipe) < 0)
    {
        fprintf(2, "Error: Can't create pipe!\n");
        exit(1);
    }

    int pid = fork();

    if (pid < 0)
    {
        fprintf(2, "Error: Can't fork!\n");
        exit(1);
    }

    if (pid == 0)
    {
        close(parent_to_child_pipe[1]);
        close(child_to_parent_pipe[0]);

        if (read(parent_to_child_pipe[0], buffer, 1) > 0 && buffer[0] == 'a')
        {
            printf("%d: received ping\n", getpid());
        }

        write(child_to_parent_pipe[1], buffer, 1);

        close(parent_to_child_pipe[0]);
        close(child_to_parent_pipe[1]);
        exit(0);
    }
    else
    {
        close(parent_to_child_pipe[0]);
        close(child_to_parent_pipe[1]);

        write(parent_to_child_pipe[1], &byte, 1);

        if (read(child_to_parent_pipe[0], buffer, 1) > 0 && buffer[0] == 'a')
        {
            printf("%d: received pong\n", getpid());
        }

        close(parent_to_child_pipe[1]);
        close(child_to_parent_pipe[0]);

        wait(0);
        exit(0);
    }
}

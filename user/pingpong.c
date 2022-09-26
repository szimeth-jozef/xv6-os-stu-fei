#include "kernel/types.h"
#include "user/user.h"

#define ERROR -1
#define PIPE_READ_FD 0
#define PIPE_WRITE_FD 1

int main(int argc, char* argv[])
{
    int child_direction[2];
    int parent_direction[2];

    if (pipe(child_direction) != 0)
    {
        fprintf(2, "Unexpected error during child_direction pipe initialization.\n");
        exit(ERROR);
    }

    if (pipe(parent_direction) != 0)
    {
        fprintf(2, "Unexpected error during parent_direction pipe initialization.\n");
        exit(ERROR);
    }

    int pid = fork();
    // The execution continues from here for both processes. The order is undeterministic, but we can ensure it with wait().
    // Return values of - fork
    // -1 is error
    // 0 is child process
    // > 0 is parent
    if (pid < 0)
    {
        fprintf(2, "Fork failed.");
        exit(ERROR);
    }

    if (pid == 0)
    {
        // Child process
        char buf;
        read(child_direction[PIPE_READ_FD], &buf, 1);
        printf("%d: received ping\n", getpid());
        write(parent_direction[PIPE_WRITE_FD], "b", 1);
    }
    else
    {
        // Parent process
        write(child_direction[PIPE_WRITE_FD], "a", 1);
        char buf;
        read(parent_direction[PIPE_READ_FD], &buf, 1);
        printf("%d: received pong\n", getpid());
    }

    exit(0);
}
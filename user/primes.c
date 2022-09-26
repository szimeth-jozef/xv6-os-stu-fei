#include "kernel/types.h"
#include "user/user.h"

#define ERROR -1
#define PIPE_READ_FD 0
#define PIPE_WRITE_FD 1

#define DEBUG 0

void process_pipe(int fd)
{
    int current_num;
    read(fd, &current_num, sizeof(int));
    if (DEBUG)
        printf("PID = %d prime %d\n", getpid(), current_num);
    else
        printf("prime %d\n", current_num);

    int next_pipe[2];
    pipe(next_pipe);

    int next_num = -1;
    while (read(fd, &next_num, sizeof(int)) > 0)
    {
        if (next_num % current_num != 0)
        {
            if (DEBUG)
                printf("PID = %d writing to pipe %d\n", getpid(), next_num);
            write(next_pipe[PIPE_WRITE_FD], &next_num, sizeof(int));
        }
    }

    if (next_num == -1) {
        // Pipe is empty, do not create more children (recursion termination)
        close(next_pipe[PIPE_WRITE_FD]);
        close(next_pipe[PIPE_READ_FD]);
        close(fd);
        return;
    }

    int pid = fork();
    if (pid == 0)
    {
        // Child process
        close(next_pipe[PIPE_WRITE_FD]);
        close(fd);
        process_pipe(next_pipe[PIPE_READ_FD]);
        close(next_pipe[PIPE_READ_FD]);
    }
    else
    {
        // Parent process
        close(next_pipe[PIPE_WRITE_FD]);
        close(next_pipe[PIPE_READ_FD]);
        close(fd);
        wait(0);
    }
}

int main(int argc, char* argv[])
{
    int start_pipe[2];
    // TODO: check pipe return value
    pipe(start_pipe);

    for (int i = 2; i <= 35; i++)
        write(start_pipe[PIPE_WRITE_FD], &i, sizeof(int));

    close(start_pipe[PIPE_WRITE_FD]);
    process_pipe(start_pipe[PIPE_READ_FD]);
    close(start_pipe[PIPE_READ_FD]);

    exit(0);
}
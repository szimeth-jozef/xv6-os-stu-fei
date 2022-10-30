#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define IS_BLANK(chr) (chr == ' ' || chr == '\t')

#define ERROR -1
#define SUCCESS 0
#define STD_INPUT 0
#define CHILD_PROC_ID 0

int main(int argc, char* argv[])
{
    printf("Process id: %d\n", getpid());
    if (argc < 2)
    {
        fprintf(2, "usage: xargs <command> [argv...]\n");
        exit(ERROR);
    }

    int next_arg_index;
    int next_buf_pos = 0;
    char* xargs_arguments[MAXARG];
    char chr;
    char buf[2048];
    char* buf_ptr = buf;

    // argv[c + 1], cuz we ignore "xargs" in ["xargs",...]
    // Save command and it's args passed directly to xargs
    for (next_arg_index = 0; next_arg_index < argc - 1; next_arg_index++)
        xargs_arguments[next_arg_index] = argv[next_arg_index + 1];

    // Read from std_input byte by byte
    while (read(STD_INPUT, &chr, 1) > 0)
    {
        if (IS_BLANK(chr))
        {
            // ch is a blank character, so whatever string is in buf null-terminate it
            buf[next_buf_pos++] = 0;

            // Save string stored at buf_ptr to xargs_arguments
            xargs_arguments[next_arg_index++] = buf_ptr;

            // Shift buf_ptr, so it points to the next position in buf for the new/next argument
            // buf = ['h', 'e', 'l', 'l', 'o', \0, _ , _, ...]
            //        ^                            ^
            //        |__previous buf_ptr          |__new buf_ptr
            buf_ptr = buf + next_buf_pos;

            // Read next byte from STD_INPUT
            continue;
        }

        if (chr != '\n')
            buf[next_buf_pos++] = chr;
        else
        {
            // At this point chr must a new line character
            // So save whatever string is at buf_ptr to xargs_arguments
            xargs_arguments[next_arg_index++] = buf_ptr;
            // Shift buf_ptr by the current arg length (pipe may contain multiple lines a.k.a. \n)
            buf_ptr = buf + next_buf_pos;

            if (fork() == CHILD_PROC_ID)
                exit(exec(xargs_arguments[0], xargs_arguments));

            wait(0);

            // When chr is \n reset xargs_arguments to arguments loaded
            // from argv i.e. remove previously loade args from STD_INPUT
            next_arg_index = argc - 1;
        }
    }

    exit(SUCCESS);
}
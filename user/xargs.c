#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

#define IS_BLANK(chr) (chr == ' ' || chr == '\t')

#define ERROR -1
#define SUCCESS 0
#define STD_INPUT 0  // process reads from file descriptor

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(2, "usage: xargs <command> [argv...]\n");
        exit(ERROR);
    }

    int c;
    int blanks = 0, offset = 0;
    char* xargs_command[MAXARG];
    char ch, buf[2048];
    char* p = buf;

    // c = 1, cuz we ignore "xargs" in ["xargs",...]
    // Save command and it's args passed directly to xargs
    for (c = 1; c < argc; c++)
        xargs_command[c - 1] = argv[c];

    --c;

    // Read from std_input byte by byte
    while (read(STD_INPUT, &ch, 1) > 0)
    {
        if (IS_BLANK(ch))
        {
            blanks++;
            continue;
        }

        if (blanks)
        {
            buf[offset++] = 0;

            xargs_command[c++] = p;
            p = buf + offset;

            blanks = 0;
        }

        if (ch != '\n')
            buf[offset++] = ch;
        else
        {
            xargs_command[c++] = p;
            p = buf + offset;

            if (!fork())
                exit(exec(xargs_command[0], xargs_command));

            wait(0);
            c = argc - 1;
        }
    }

    exit(SUCCESS);
}
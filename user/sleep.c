#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: sleep <ticks>\n");
        exit(0);
    }

    int ticks = atoi(argv[1]);

    if (ticks < 1)
    {
        fprintf(2, "Invalid interval %s\n", argv[1]);
        exit(1);
    }

    sleep(ticks);
    exit(0);
}
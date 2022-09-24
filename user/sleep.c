#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: sleep <secodns>\n");
        exit(0);
    }

    int seconds = atoi(argv[1]);

    if (seconds < 1)
    {
        printf("Invalid interval %s\n", argv[1]);
        exit(1);
    }

    sleep(seconds);
    exit(0);
}
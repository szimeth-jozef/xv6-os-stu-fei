#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    uint64 uptime_in_ticks = uptime();

    printf("Uptime: %d ticks\n", uptime_in_ticks);
    exit(0);
}
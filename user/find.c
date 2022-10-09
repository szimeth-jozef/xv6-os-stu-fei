#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define ERROR -1
#define PIPE_READ_FD 0
#define PIPE_WRITE_FD 1

char* get_path_last_child(char* path)
{
    char* p;
    for (p = path + strlen(path); p >= path && *p != '/'; p--);

    return ++p;
}

void find(char* path, char* target_name)
{
    int fd;
    char* p;
    char buf[512];
    struct dirent de;
    struct stat st;

    if (!strcmp(get_path_last_child(path), target_name))
        printf("%s\n", path);

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open [%s], fd=%d\n", path, fd);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot get stat for %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        find(buf, target_name);
    }

    close(fd);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        fprintf(2, "Usage: find <start_dir> <name>\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}
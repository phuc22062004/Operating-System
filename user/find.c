#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *fmtname(char *path)
{
    char *p = path + strlen(path);
    while (p >= path && *p != '/')
        p--;
    return p + 1;
}

void find(char *path, char *file)
{
    char buffer[512], *p;
    int fd;
    struct dirent d;
    struct stat s;

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        fprintf(2, "Error: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &s) < 0)
    {
        fprintf(2, "Error: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (s.type)
    {
    case T_FILE:
        if (strcmp(fmtname(path), file) == 0)
        {
            printf("%s\n", path);
        }
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buffer))
        {
            fprintf(2, "Error: path too long\n");
            break;
        }

        strcpy(buffer, path);
        p = buffer + strlen(buffer);
        *p++ = '/';

        while (read(fd, &d, sizeof(d)) == sizeof(d))
        {
            if (d.inum == 0)
                continue;
            if (strcmp(d.name, ".") != 0 && strcmp(d.name, "..") != 0)
            {
                memmove(p, d.name, DIRSIZ);
                p[DIRSIZ] = 0;

                if (stat(buffer, &s) < 0)
                {
                    fprintf(2, "Error: cannot stat %s\n", buffer);
                    continue;
                }
                find(buffer, file);
            }
        }
        break;
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: find path file\n");
        exit(1);
    }

    if (argc == 2)
    {
        find(".", argv[1]);
    }
    else
    {
        for (int i = 1; i < argc - 1; i++)
        {
            find(argv[i], argv[argc - 1]);
        }
    }

    exit(0);
}

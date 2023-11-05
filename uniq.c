#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char* fgets(char* buf, int n, int fd) {
    int i;
    char c;
    for (i = 0; i < n - 1; i++) {
        if (read(fd, &c, 1) != 1)
            break;
        buf[i] = c;
        if (c == '\n')
            break;
    }
    buf[i] = '\0';
    return i > 0 ? buf : 0;
}

int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = (*s1 >= 'A' && *s1 <= 'Z') ? (*s1 + 32) : *s1;
        char c2 = (*s2 >= 'A' && *s2 <= 'Z') ? (*s2 + 32) : *s2;
        if (c1 != c2) {
            return c1 - c2;
        }
        s1++;
        s2++;
    }
    return 0;
}

void uniq(int fd, int cflag, int dflag, int iflag) {
    char current_line[512];
    char next_line[512];
    int count = 1;
    
    if (fgets(current_line, sizeof(current_line), fd) == 0)
        return;

    while (fgets(next_line, sizeof(next_line), fd) != 0) {
        if (iflag && strcasecmp(current_line, next_line) == 0)
            count++;
        else if (!iflag && strcmp(current_line, next_line) == 0)
            count++;
        else {
            if (dflag && count == 1) {
                if (count > 1) {
                    printf(1, "%s\n", current_line);
                }
                
            }
            
            else if (dflag && count > 1) {
                printf(1, "%s\n", current_line);
            }
             else if (!dflag) {
                if (cflag)
                    printf(1, "%d %s\n", count, current_line);
                else
                    printf(1, "%s\n", current_line);
            }
            strcpy(current_line, next_line);
            count = 1;
        }
    }

    if (dflag && count == 1) {
        if (count > 1) {
            printf(1, "%s\n", current_line);
        }
    } else if (!dflag) {
        if (cflag)
            printf(1, "%d %s\n", count, current_line);
        else
            printf(1, "%s\n", current_line);
    }
}

int main(int argc, char *argv[]) {
    int fd;
    int cflag = 0;
    int dflag = 0;
    int iflag = 0;
    int i;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-c") == 0)
                cflag = 1;
            else if (strcmp(argv[i], "-d") == 0)
                dflag = 1;
            else if (strcmp(argv[i], "-i") == 0)
                iflag = 1;
        }
    }

    if (argc <= 1) {
        uniq(0, cflag, dflag, iflag); 
        exit();
    } else {
        int i;
        for (i = 1; i < argc; i++) {
            if (argv[i][0] != '-') {
                if ((fd = open(argv[i], O_RDONLY)) < 0) {
                    printf(1, "uniq: cannot open %s\n", argv[i]);
                    exit();
                }
                uniq(fd, cflag, dflag, iflag);
                close(fd);
            }
        }
    }
    exit();
}

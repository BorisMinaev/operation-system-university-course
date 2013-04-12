#include <unistd.h>
#include <sys/wait.h>

char delim = '\n';
int buf_len = 4 * 1024;
char *buffer;
char** argv;
int argcp;

void do_main_part(int fr, int len) {
    int not_need_to_write = 0;
    char * s = (char*) malloc(len);
    int i;
    for (i = 0; i < len; i++)
        *(s + i) = *(buffer + fr + i);
    if (fork() == 0) {
        exit(0);
        char ** args;
        args = malloc(sizeof(char*) * argcp);
        for (; *(argv + i); i++) {
            if (strcmp(*(argv + i), "{}") == 0) {
                (*(args + i)) = s;
            } else {
                *(args + i) = *(argv + i);
            }
        }
        execv(args[optind], args + optind + 1);
        exit(1);
    }
    int status;
    int x = wait(&status);
    printf("%d\n", x);
    printf("%d\n", status);
    if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
        write(1, buffer + fr, len);
    }
}

int main(int argc, char ** argvv) {
    int c;
    argv = argvv;
    argcp = argc;
    while ((c = getopt(argc, argv, "nzb:")) != -1) {
        switch (c)
        {
            case 'n':
                delim = '\n';
                break;
            case 'z':
                delim = 0;
                break;
            case 'b':
                buf_len = atoi(optarg);
                break;
            default:
                abort();
        }
    }
    buffer = (char *) malloc(buf_len + 1);
    int eof = 0;
    int from = 0;
    while (1 - eof) {
        int r = read(0, buffer + from, buf_len - from);
        if (r == 0) {
            eof = 1;
            *(buffer + from) = delim;
            from++;
        }    
        from += r;
        while (1) {
            int i;
            int first_delim = -1;
            for (i = 0; i < from; i++) {
                if (*(buffer + i) == '\n') {
                    first_delim = i;
                    break;
                }
            }
            if (first_delim == -1) {
                break;
            }
//            printf("%d\n", first_delim);
            do_main_part(0, first_delim);
            for (i = first_delim + 1; i < from; i++) {
                *(buffer + i - first_delim - 1) = *(buffer + i);
            }
            from -= first_delim + 1;
        }
        if (from == buf_len) {
            return 1;
        }
    }    
    free(buffer);
    return 0;
}

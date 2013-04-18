#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

char delim = '\n';
int buf_len = 4 * 1024;
char *buffer;
char** argv;
int argc;

void do_main_part(int fr, int len) {
    char * s = (char*) malloc(len +1);
    int i=0;
    for (i = 0; i < len; i++)
        *(s + i) = *(buffer + fr + i);
    *(s + len) = 0;
    if (fork() == 0) {
        int devnull = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC |
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        int pipefd[2];
        pipe(pipefd);
        dup2(pipefd[1], 1);
        char ** args;
        args = malloc(sizeof(char*) * (argc + 1));
        int i = optind;
        for (; *(argv + i); i++) {
            if (strcmp(*(argv + i), "{}") == 0) {
                (*(args + i)) = s;
            } else {
                *(args + i) = *(argv + i);
            }
        }
        *(args + argc) = 0;
        execvp(args[optind], args + optind);
        exit(1);
    }
    int status;
    int x = wait(&status);
    if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {
        *(buffer + fr + len) = '\n';
        write(1, buffer + fr, len+1);
        *(buffer + fr + len) = delim;
    }
}

int main(int argcc, char** argvv) {
    argv = argvv;
    int c;
    argc = argcc;
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
                if (*(buffer + i) == delim) {
                    first_delim = i;
                    break;
                }
            }
            if (first_delim == -1) {
                break;
            }
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

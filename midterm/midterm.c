#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int buf_len = 100;
char * buffer;
int file_r = -1;

void my_exit(int exit_code) {
    free(buffer);
    if (file_r != -1)
        close(file_r);
    _exit(exit_code);
}

int str_len(char * s) {
    int res = 1;
    while (*s) {
        res++;
        s++;
    }
    return res;
}

void my_print(char * s, int len) {
    int alr = 0;
    while (alr < len) {
       int add_len = write(1, s, len - alr);
       if (add_len < 0)
           my_exit(1);
       alr += add_len;
    }
}

int main(int argc, char ** argv) {
    if (argc != 4) {
        puts("Usage: midterm filename string_number new_string");
        _exit(1);
    }
    buffer = (char *) malloc(buf_len);
    file_r = open(argv[1], O_RDWR);
    int lines_read = 0;
    int string_number = atoi(argv[2]);
    string_number--;
    if (string_number == 0) {
        my_print(argv[3], str_len(argv[3]));
        puts("");
    }
    while (1) {
        int re = read(file_r, buffer, buf_len);
        if (re < 0) my_exit(1);
        if (re == 0) break;
        int i = 0;
        int st = 0;
        for (i = 0; i < re; i++) {
            if ((*(buffer + i)) == '\n') {
                lines_read++;
                my_print(buffer + st, i - st + 1);
                st = i + 1;
                if (lines_read == string_number) {
                    my_print(argv[3], str_len(argv[3]));
                    puts("");
                }
            }
        }
        if (st != re) {
            my_print(buffer + st, re - st);
        }
    }
    if (lines_read < string_number) {
        while (lines_read < string_number) {
            lines_read++;
            puts("");
        }
        my_print(argv[3], str_len(argv[3]));
        puts("");
    }
    my_exit(0);
}

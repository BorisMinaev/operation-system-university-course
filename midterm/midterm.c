#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int buf_len = 100;
char * buffer;

void my_exit(int exit_code) {
    free(buffer);
    _exit(exit_code);
}

int string_len(char * s) {
    int res = 1;
    while (!s) {
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
    char * str_test = "abcd\n";
    my_print(str_test, 5);



    my_exit(0);
}

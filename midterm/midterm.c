#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int buf_len = 100;
char * buffer_to_run;
char * buffer;
char * string_to_write;
int file_r = -1;
int file_to_run = -1;

void my_exit(int exit_code) {
    free(buffer);
    free(buffer_to_run);
    if (file_r != -1)
        close(file_r);
    if (file_to_run != -1)
        close(file_to_run);
    _exit(exit_code);
}

int str_len(char * s) {
    int res = 0;
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

char * file_to_change;

void do_main_part(int string_number) {
    file_r = open(file_to_change, O_RDONLY);
    if (file_r == -1) {
        puts("Error while reading file");
        my_exit(1);
    }
    int lines_read = 0;
    string_number--;
    if (string_number == 0) {
        my_print(string_to_write, str_len(string_to_write));
        write(1, "\n", 1);
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
                    my_print(string_to_write, str_len(string_to_write));
                    write(1, "\n", 1);
                }
            }
        }
        if (st != re) {
            my_print(buffer + st, re - st);
        }
    }
    if (lines_read < string_number) {
        my_print(string_to_write, str_len(string_to_write));
        write(1, "\n", 1);
    }
    close(file_r);
    file_r = -1;
}

void work_with_param(char * s, int len) {
    int last_space = -1;
    int i = 0;
    for (i = 0; i < len; i++)
        if ((*(s+i)) == ' ') 
            last_space = i;
    if (last_space == -1)
        my_exit(1);
    my_print(s, last_space);
    puts("");
    my_print(s + last_space + 1, len - last_space - 1);
    puts("");
}

int main(int argc, char ** argv) {
    if (argc != 4) {
        puts("Usage: midterm filename_to_run filename_to_change new_string");
        _exit(1);
    }
    buffer = (char *) malloc(buf_len);
    buffer_to_run = (char *) malloc(buf_len);
    file_to_run = open(argv[1], O_RDONLY);
    file_to_change = argv[2];
    string_to_write = argv[3];
    if (file_to_run == -1) {
        puts("Error while opening file");
        my_exit(1);
    }
    int already = 0;
    int eof = 0;
    while (1) {
        int re = read(file_to_run, buffer_to_run + already, buf_len - already);
        if (re < 0) 
            my_exit(1);
        if (re == 0)
            eof = 1;
        already += re;
        while (1) {
            int first_endl = -1;
            int i;
            for (i = 0; i < already; i++) 
                if ((*(buffer_to_run + i)) == '\n') {
                    first_endl = i;
                    break;
                }
            if (first_endl == -1 && already == buf_len) 
                my_exit(1);
            if (first_endl != -1) {
                work_with_param(buffer_to_run, first_endl);
                for (i = first_endl + 1; i < already; i++)
                    *(buffer_to_run + i - first_endl - 1) = *(buffer_to_run + i);
                already -= first_endl + 1;
            } else {
                break;
            } 
        }
        if (eof) break;
    } 
    my_exit(0);
}

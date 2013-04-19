#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int buf_len = 100;

struct data {
    char * buffer_to_run;
    char * buffer;
    char * string_to_write;
    char * run_program;
    char * run_argument;
    char * file_to_change;
    int file_r;
    int file_to_run;
};

void my_exit(int exit_code, struct data d) {
    free(d.buffer);
    free(d.buffer_to_run);
    free(d.run_program);
    free(d.run_argument);
    if (d.file_r != -1)
        close(d.file_r);
    if (d.file_to_run != -1)
        close(d.file_to_run);
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

void my_print(char * s, int len, struct data d) {
    int alr = 0;
    while (alr < len) {
       int add_len = write(1, s, len - alr);
       if (add_len < 0)
           my_exit(1, d);
       alr += add_len;
    }
}

void do_main_part(int string_number, struct data d) {
    d.file_r = open(d.file_to_change, O_RDONLY);
    if (d.file_r == -1) {
        puts("Error while reading file");
        my_exit(1, d);
    }
    int lines_read = 0;
    string_number--;
    if (string_number == 0) {
        my_print(d.string_to_write, str_len(d.string_to_write), d);
        write(1, "\n", 1);
    }
    while (1) {
        int re = read(d.file_r, d.buffer, buf_len);
        if (re < 0) my_exit(1, d);
        if (re == 0) break;
        int i = 0;
        int st = 0;
        for (i = 0; i < re; i++) {
            if ((*(d.buffer + i)) == '\n') {
                lines_read++;
                my_print(d.buffer + st, i - st + 1, d);
                st = i + 1;
                if (lines_read == string_number) {
                    my_print(d.string_to_write, str_len(d.string_to_write), d);
                    write(1, "\n", 1);
                }
            }
        }
        if (st != re) {
            my_print(d.buffer + st, re - st, d);
        }
    }
    if (lines_read < string_number) {
        my_print(d.string_to_write, str_len(d.string_to_write), d);
        write(1, "\n", 1);
    }
    close(d.file_r);
    d.file_r = -1;
}

void work_with_param(char * s, int len, struct data d) {
    int last_space = -1;
    int i = 0;
    for (i = 0; i < len; i++)
        if ((*(s+i)) == ' ') 
            last_space = i;
    if (last_space == -1)
        my_exit(1, d);
    for (i = 0; i < last_space; i++)
        *(d.run_program + i) = *(s + i);
    *(d.run_program + last_space) = 0;
    for (i = last_space + 1; i < len; i++)
        *(d.run_argument + i - last_space - 1) = *(s+ i);
    *(d.run_argument + len - last_space - 1) = 0;
  
    int child;
    int pipefd[2];
    pipe(pipefd); 
    if ((child = fork()) == 0) {
        dup2(pipefd[1], 1);
        close(pipefd[0]); close(pipefd[1]);
        execlp(d.run_program, d.run_program, d.run_argument, NULL);
        my_exit(1, d);
    } 
    waitpid(child, NULL, 0);
    int ll = read(pipefd[0], d.run_program, buf_len);
    if (ll < 0)
        my_exit(1, d);
    int string_number_to_paste= atoi(d.run_program);
    do_main_part(string_number_to_paste, d);
    puts("");
}

int main(int argc, char ** argv) {
    if (argc != 4) {
        puts("Usage: midterm filename_to_run filename_to_change new_string");
        _exit(1);
    }
    struct data d;
    d.file_r = -1;
    d.file_to_run = -1;
    d.buffer = (char *) malloc(buf_len);
    d.buffer_to_run = (char *) malloc(buf_len);
    d.run_program = (char *) malloc(buf_len);
    d.run_argument = (char *) malloc(buf_len);
    d.file_to_run = open(argv[1], O_RDONLY);
    d.file_to_change = argv[2];
    d.string_to_write = argv[3];
    if (d.file_to_run == -1) {
        puts("Error while opening file");
        my_exit(1, d);
    }
    int already = 0;
    int eof = 0;
    while (1) {
        int re = read(d.file_to_run, d.buffer_to_run + already, buf_len - already);
        if (re < 0) 
            my_exit(1, d);
        if (re == 0)
            eof = 1;
        already += re;
        while (1) {
            int first_endl = -1;
            int i;
            for (i = 0; i < already; i++) 
                if ((*(d.buffer_to_run + i)) == '\n') {
                    first_endl = i;
                    break;
                }
            if (first_endl == -1 && already == buf_len) 
                my_exit(1, d);
            if (first_endl != -1) {
                work_with_param(d.buffer_to_run, first_endl, d);
                for (i = first_endl + 1; i < already; i++)
                    *(d.buffer_to_run + i - first_endl - 1) = *(d.buffer_to_run + i);
                already -= first_endl + 1;
            } else {
                break;
            } 
        }
        if (eof) break;
    } 
    my_exit(0, d);
}

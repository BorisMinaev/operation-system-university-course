#include <unistd.h>

char delim = ' ';
int buf_len = 4 * 1024;
char *buffer;
char** args;

void do_main_part(int fr, int len) {
    int not_need_to_write = 0;
    int child_id;
    if ((child_id = fork()) == 0) {
       int i = optid;
      for (; *(args + i); i++) {
        if (strcmp(*(argv + i), "{}") == 0) {
            strcpy
      } 
    }
    write(1, buffer + fr, len);
}

int main(int argc, char ** argv) {
    int c;
    args = argv;
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
    printf("%d\n", buf_len);
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
            do_main_part(0, first_delim - 1);
            for (i = first_delim + 1; i < from; i++) {
                *(buffer + i - first_delim - 1) = *(buffer + i);
            }
            from -= first_delim - 1;
        }
        if (from == buf_len) {
            return 1;
        }
    }    
    free(buffer);
    return 0;
}

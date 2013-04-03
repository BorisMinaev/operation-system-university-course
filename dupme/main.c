#include <stdlib.h>

int my_atoi(char* number) {
    int res = 0;
    while (*number) {
        res = res * 10 + (*number++) - '0';
    }
    return res;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        return -1;
    }
    int k = my_atoi(argv[1]) + 1;
    char *buffer = (char *) malloc(k);
    int from = 0;
    int now_write = 1;
    while (1) {
        int r = read(0, buffer + from, k - from);
        if (r == 0) {
            break;
        }
        from += r;
        while (1) {
            int i;
            int first_next_line = -1;
            for (i = 0; i < from; i++) {
                if (*(buffer + i) == '\n') {
                    first_next_line = i;
                    break;
                }
            }
            if (first_next_line == -1) {
                break;
            }
            if (now_write) {
                write(1, buffer, first_next_line + 1);
                write(1, buffer, first_next_line + 1);
            }
            for (i = first_next_line + 1; i < from; i++) {
                *(buffer + i - first_next_line - 1) = *(buffer + i);
            }
            from -= first_next_line + 1;
            now_write = 1;
        }
        if (from == k) {
            from = 0;
            now_write = 0;
        }
    }
    free(buffer);
    return 0;
}


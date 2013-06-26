#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int my_write(int length, char* data) {
    int wrote = 0;
    while (wrote < length) {
        int cur_write = write(1, data + wrote, length - wrote);
        if (cur_write <= 0) {
            return 1;
        }
        wrote += cur_write;
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        return -1;
    }
    int k = atoi(argv[1]) + 1;
    char *buffer = (char *) malloc(k);
    int from = 0;
    int now_write = 1;
    int eof = 0;
    while (1 - eof) {
        int r = read(0, buffer + from, k - from);
        if (r == 0) {
            eof = 1;
            *(buffer + from) = '\n';
            from++;
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
                if (my_write(first_next_line + 1, buffer)) return 1;
                if (my_write(first_next_line + 1, buffer)) return 1;
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


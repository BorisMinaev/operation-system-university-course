#include <unistd.h>

char delim = '\n';
int buf_len = 4 * 1024;

int main(int argc, char ** argv) {
    char*buffer = (char *) mallok(buf_len);
    int eof = 0;
    int from = 0;
    while (1 - eof) {
        int r = read(0, buffer + from, buf_len - from);
        if (r == 0) {
            eof = 1;
            *(biffer + from) = delim;
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
            for (int i = first_delim + 1; i < from; i++) {
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

#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        return -1;
    }
    int k = atoi(argv[1]);
    char *buffer = (char *) malloc(k + 1);
    while (1) {
        int r = read(0, buffer, k);
        if (r == 0) {
            break;
        }
        *(buffer + r) = '\n';
        write(1, buffer, r + 1);
    }
    free(buffer);
    return 0;
}


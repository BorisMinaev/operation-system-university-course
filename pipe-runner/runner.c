#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        _exit(1);
    }
    std::vector<std::vector<std::vector<char> > > data;
    int file_id = open(argv[1], O_RDONLY);
    if (file_id == -1) 
       _exit(1); 
    int buf_len = 4096;
    char * buffer = (char *) malloc(buf_len);
    int is_new_word = 1;
    int is_new_pipe = 1;
    while (true) {
        int re = read(file_id, buffer, buf_len);
        if (re < 0) {
           free(buffer);
           _exit(1);
        }
        if (re == 0) 
            break;
        for (int i = 0; i < re; i++) {
            char c = *(buffer + i);
            if (c == 0) {
                if (is_new_word == 0) {
                    is_new_word = 1;
                } else {
                    is_new_pipe = 1;
                }
            } else {
                if (is_new_pipe) {
                    std::vector<std::vector<char> > tmp;
                    data.push_back(tmp);
                    is_new_pipe = 0;
                }
                if (is_new_word) {
                    std::vector<char> tmp;
                    data[data.size() - 1].push_back(tmp);
                    is_new_word = 0;
                }
                data[data.size() - 1][data[data.size() - 1].size() - 1].push_back(c);
            }
        }
    }
    free(buffer);
    printf("%d\n", data.size());
    int i;
    for (i = 0; i < data.size(); i++) {
        printf("%d\n", data[i].size());
        int j;
        for (j = 0; j < data[i].size(); j++)
            printf("%d ", data[i][j].size());
        printf("\n");
    }

}

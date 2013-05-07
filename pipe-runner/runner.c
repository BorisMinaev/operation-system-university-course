#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>
#include <string.h>

void do_main_part(std::vector<std::vector<std::vector<char> > > data) {
    int pr_id = 0;
    for (pr_id = 0; pr_id < data.size(); pr_id++) {
        std::vector<std::vector<char> > cur_program = data[pr_id];
        char ** arguments = (char **) malloc(cur_program.size());
        int arg_id;
        for (arg_id = 0; arg_id < data[pr_id].size(); arg_id++) {
            char * cur_arg = (char *) malloc(cur_program[arg_id].size() + 1);
            cur_arg[cur_program[arg_id].size()] = 0;
            int ch;
            for (ch = 0; ch < cur_program[arg_id].size(); ch++)
                cur_arg[ch] = cur_program[arg_id][ch];
            arguments[arg_id] = cur_arg;
        }
        printf("PRORGAM #%d", pr_id);
        for (arg_id = 0; arg_id < data[pr_id].size(); arg_id++) {
            write(1, arguments[arg_id], strlen(arguments[arg_id]));
            printf("\n");
        }
    } 
}

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
    do_main_part(data);
}

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <vector>
#include <sys/stat.h>

char * last = (char *) "tmp345738590803485_last.txt";
char * cur  = (char *) "tmp345738590803485_cur.txt";

void remove_files() {
    remove(last);
    remove(cur);
}

void my_exit() {
    remove_files();
    _exit(0);
}

void run_programs(std::vector<std::vector<char*> > programs) {
    int child = 0;
    for (size_t i = 0; i < programs.size(); i++) {
        if ((child = fork()) == 0) {
            std::vector<char*> cur_program = programs[i];
            int size = cur_program.size();
            char ** params = (char **) malloc(sizeof(char *) * size); 
            for (int j = 0; j < size; j++)
                params[j] = cur_program[j];
            execvp(params[0], params);
            free(params);
            _exit(1);
        }
        waitpid(child, NULL, 0);
    }
}

int main(int argc, char ** argv) {
    close(open(last, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO));
    close(open(cur, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO));


    if (argc < 3) {
       puts("Need more arguments!");
       my_exit(); 
    }
    int time_to_sleep = atoi(argv[1]);

    while (1) {
        int cur_file = open(cur, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
        int child = 0;
        if ((child = fork()) == 0) {
            dup2(cur_file, 1);
            close(cur_file);
            execvp(argv[2], argv+ 2);
            _exit(1);
        }
        waitpid(child, NULL, 0);
        std::vector<std::vector<char *> > want_to_run;
        std::vector<char *> program1;
        program1.push_back((char*)"cat");
        program1.push_back(cur);
        want_to_run.push_back(program1);
        std::vector<char *> program2;
        program2.push_back((char*)"diff");
        program2.push_back((char*)"-u");
        program2.push_back(last);
        program2.push_back(cur);
        want_to_run.push_back(program2);
        std::vector<char *> program3;
        program3.push_back((char*)"mv");
        program3.push_back(cur);
        program3.push_back(last);
        want_to_run.push_back(program3);
        run_programs(want_to_run);
        sleep(time_to_sleep);
    } 

    remove_files();
    return 0;
}

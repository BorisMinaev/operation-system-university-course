#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

char * last = "tmp345738590803485_last.txt";
char * cur  = "tmp345738590803485_cur.txt";

void remove_files() {
    remove(last);
    remove(cur);
}

void my_exit() {
    remove_files();
    _exit(0);
}


int main(int argc, char ** argv) {
    close(open(last, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO));
    close(open(cur, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO));

    signal(SIGINT, my_exit);
    signal(SIGTERM, my_exit);

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
        if ((child = fork()) == 0) {
            execlp("cat", "cat", cur, NULL);
            _exit(1);
        } 
        waitpid(child, NULL, 0);
        if ((child = fork()) == 0) {
           execlp("diff", "diff", "-u", last, cur, NULL);
           _exit(1);
        }
        waitpid(child, NULL, 0); 
        if ((child = fork()) == 0) {
            execlp("mv", "mv", cur, last, NULL);
            _exit(1);
        }
        waitpid(child, NULL, 0);
        sleep(time_to_sleep);
    } 

    remove_files();
    return 0;
}

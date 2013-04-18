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
    puts("by!");
    _exit(0);
}


int main(int argc, char ** argv) {
    close(open(last, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO));
    close(open(cur, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO));

    signal(SIGINT, my_exit);
    signal(SIGTERM, my_exit);

    while (1) {
       sleep(1);
      puts("Hello, world");
    } 

    remove_files();
    return 0;
}

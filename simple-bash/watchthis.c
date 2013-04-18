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


int main(int argc, char ** argv) {
    int ff = open(last, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    int f22= open(cur, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    close(ff);
    close(f22);

    return 0;
}

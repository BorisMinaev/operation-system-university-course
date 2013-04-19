#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int main(int argc, char ** argv) {
    if (argc != 4) {
        puts("Usage: midterm filename string_number new_string");
        _exit(1);
    }

}

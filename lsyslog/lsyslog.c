#include <pty.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stropts.h>
#include <poll.h>
#include <errno.h>


int socket_fd;
const char * programIndent;

void openlog(const char * indent, int option, int facility) {
    programIndent = indent;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    struct addrinfo *result;
    if (getaddrinfo(NULL, "8123", &hints, &result) != 0)
    {
        perror("getaddrinfo fail");
        _exit(1);
    }
    if (result == NULL)
    {
        perror("result is null");
        _exit(1);
    }
    socket_fd = socket(result->ai_family, result->ai_socktype,
                        result->ai_protocol);
    connect(socket_fd, result->ai_addr, result->ai_addrlen);
};

int my_write(const char * s) {
    int need = strlen(s);
    int alr = 0;
    while (alr < need) {
        int wr = write(socket_fd, s + alr, need - alr);
        if (wr < 0)
            return -1;
        alr += wr;
    }
    return 0;
}

void syslog(int priority, const char *format, ...) {
    if (my_write(programIndent) == -1)
        return;
    if (my_write(":\n") == -1)
        return;
    if (my_write(format) == -1)
        return;
};

void closelog(void) {
	close(socket_fd);
};

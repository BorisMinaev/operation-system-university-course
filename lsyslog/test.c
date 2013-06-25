#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <pty.h>
#include <netdb.h>
#include <fcntl.h>
#include <syslog.h>

int main() {
    openlog("MY_PROGRAM", 0, 0);
    //int i;
    //for (i = 0; i<1000000; i++) {
    syslog(0, "error is there!\n");
    //sleep(2);
    //}
    closelog();
    return 0;
}

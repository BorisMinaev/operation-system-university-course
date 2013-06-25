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
#include <pthread.h>
#include <sys/mman.h>

int child;

void handler(int a) {
    if (child) {
        kill(-child, a);
    }
}

const int size = 4096;

int main(int argc, char** argv) {
    char *shared_block;
    shared_block = (char *) mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    pthread_mutex_t *lock;
    lock = (pthread_mutex_t*)shared_block;
    pthread_mutexattr_t mutexattr;
    pthread_mutex_init(lock, &mutexattr);
    if (argc != 2) {
       perror("Usage: lsyslogdaemon log_file_name"); 
       return -1;
    }
    int fileId = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fileId < 0) {
        perror("Can not open file");
        return -1;
    }
    child = fork();
    if (child) {
        signal(SIGINT, handler);
        wait(NULL);
    } else {
        setsid();
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        if (getaddrinfo(NULL, "8123", &hints, &res)) {
            perror("error getaddrinfo");
        }
        int sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sfd == -1) {
            perror("error socket");
        }
        int optval = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1)    {
            perror("error setsockopt");
        }

        if (bind(sfd, res->ai_addr, res->ai_addrlen) == -1) {
            perror("error bind");
        }
            if (listen(sfd, 5) == -1) {
            perror("error listen");
        }
        struct sockaddr_storage peer_addr;
        socklen_t peer_addr_size = sizeof(struct sockaddr_storage);

        while (1) {
            int ad = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size);
            printf("Connected\n");
            if (ad == -1) {
                perror("error accept");
                exit(2);
            }
            if (fork()) {
                close(ad);
            } else {
                int buf_len = 4096;
                char buf[buf_len];
                while (1) {
                    int r = read(ad, buf, buf_len);
                    if (r <= 0) break;
                    pthread_mutex_lock(lock);
                    sleep(10);
                    int alrWrite = 0;
                    while (alrWrite < r) {
                       int wr = write(fileId, buf + alrWrite, r - alrWrite);
                       if (wr < 0)
                          break;
                       alrWrite += wr; 
                    }
                    pthread_mutex_unlock(lock);
                    if (alrWrite != r)
                        break;
                }        
                printf("Disconnected\n");
            }
        }
    }
    close(fileId);
    return 0;
}

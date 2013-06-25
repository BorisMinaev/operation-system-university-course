#include <pty.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stropts.h>
#include <poll.h>
#include <errno.h>
#include <vector>

const int MAX_CLIENTS = 20;

int my_write(char * buf, int len) {
    int alr = 0;
    while (alr< len) {
        int wr = write(1, buf, len - alr);
        if (wr < 0)
            return -1;
        alr += wr;
    }
    return 0;
}

void write_message_to_vector(std::string s, std::vector<char> & v) {
    for (int i = 0; i < s.size(); i++)
        v.push_back(s[i]);
}

void write_message_to_vector(std::vector<char> s, std::vector<char> & v) {
    for (int i = 0; i < s.size(); i++)
        v.push_back(s[i]);
}

int my_write(std::vector<char> v) {
    char c[1];
    if (v.size() == 0) {
        my_write("Collision", 9);
    }
    for (int i = 0; i < v.size(); i++) {
        c[0] = v[i];
        write(1, c, 1);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        perror("Usage: vdht myport otherport1 otherport2 ... ");
        _exit(1);
    }
    std::vector<std::vector<char> > all_keys;
    std::vector<std::vector<std::vector<char> > > history;
    char* myport = argv[1];
    int i;
    std::vector<int> ok_fds;
    for (i = 2; i < argc; i++) {
        char* curport = argv[i];
        printf("try to connect %s\n", curport);
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        hints.ai_protocol = 0;

        struct addrinfo *result;
        if (getaddrinfo(NULL, curport, &hints, &result) != 0)
        {
            perror("getaddrinfo fail");
            _exit(1);
        }
        if (result == NULL)
        {
            perror("result is null");
            _exit(1);
        }
        int socket_fd = socket(result->ai_family, result->ai_socktype,
        result->ai_protocol);
        int c = connect(socket_fd, result->ai_addr, result->ai_addrlen);
        if (c != -1) {
            ok_fds.push_back(socket_fd);
            printf("Connected.\n");
        }
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, myport, &hints, &res)) {
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

    pollfd fd[MAX_CLIENTS + 1];
    std::vector<std::vector<char> > buf_read;
    std::vector<std::vector<char> > buf_write;
    for (i = 0; i < ok_fds.size() + 2; i++) {
        std::vector<char> tmp;
        buf_read.push_back(tmp);
        std::vector<char> tmp2;
        buf_write.push_back(tmp2);
    }
    int buf_len = 4096;
    char buf[buf_len];
    while (1) {
        fd[0].fd = sfd;
        fd[0].events = POLLIN;
        fd[1].fd = 0;
        fd[1].events = POLLIN;
        int i;
        for (i = 0; i < ok_fds.size(); i++) {
            fd[i + 2].fd = ok_fds[i];
            fd[i + 2].events = POLLIN;
            if (buf_write[i + 2].size() != 0) {
                fd[i + 2].events |= POLLOUT;
             }
        }
        int cnt_clients = ok_fds.size();
        poll(fd, cnt_clients + 2, -1);
        for (int i = 1; i < 2 + cnt_clients; ++i) {
            if (fd[i].revents & POLLOUT) {
                int len = buf_len;
                if (buf_write[i].size() < len)
                    len = buf_write[i].size();
                int j;
                for (j = 0; j < len; j++)
                    buf[j] = buf_write[i][j];
                int wr = write(ok_fds[i - 2], buf, len);
                if (wr > 0) {
                    std::vector<char> next_vect;
                    for (j = wr; j < buf_write[i].size(); j++)
                        next_vect.push_back(buf_write[i][j]);
                    buf_write[i] = next_vect;
                }  
            }
            if (fd[i].revents & POLLIN) {
                int r = read(i == 1 ? 0 : ok_fds[i - 2], buf, buf_len);
                if (r > 0) {
                    int j;
                    for (j = 0; j < r; j++)
                        buf_read[i].push_back(buf[j]);
                }
                while (1) {
                    int firstN = -1;
                    int j;
                    for (j = 0; j < buf_read[i].size(); j++)
                        if (buf_read[i][j] == '\n') {
                            firstN = j;
                            break;
                        }
                    if (firstN == -1)
                        break;
                    // message 
                    std::vector<char> cur_message = buf_read[i];
                    if (cur_message[0] == 'c' && cur_message[1] == 'h') {
                        // change key value1 value2
                        int numSpaces = 0;
                        for (j = 0; j < cur_message.size(); j++) {
                            if (cur_message[j] == '\n')
                                break;
                            if (cur_message[j] == ' ')
                                numSpaces++;
                        }
                        if (numSpaces == 3) {
                        int curIt = 7;
                        std::vector<char> key;
                        while (cur_message[curIt] != ' ') {
                            key.push_back(cur_message[curIt]);
                            curIt++;
                        }
                        curIt++;
                        std::vector<char> value1;
                        while (cur_message[curIt] != ' ') {
                            value1.push_back(cur_message[curIt]);
                            curIt++;
                        }
                        curIt++;
                        std::vector<char> value2;
                        while (curIt < cur_message.size() && cur_message[curIt] != ' ' && cur_message[curIt] != '\n') {
                            value2.push_back(cur_message[curIt]);
                            curIt++;
                        }
                        int key_id = -1;
                        for (j = 0; j < all_keys.size(); j++) {
                           int same = 1;
                           if (all_keys[j].size() != key.size()) {
                              continue;
                           }
                           int k;
                           for (k = 0; k < key.size(); k++)
                              if (key[k] != all_keys[j][k])
                                 same = 0;
                           if (same) {
                              key_id = j;
                              break;
                           }
                        }
                        if (key_id == -1) {
                            all_keys.push_back(key);
                            key_id = all_keys.size() - 1;
                            std::vector<std::vector<char> > cur_key_history;
                            std::vector<char> tmp;
                            cur_key_history.push_back(tmp);
                            history.push_back(cur_key_history);
                        }
                        int valueId = -1;
                        for (j = 0; j < history[key_id].size(); j++) {
                           int same = 1;
                           if (history[key_id][j].size() != value1.size() && history[key_id][j].size() != 0) {
                              continue;
                           }
                           int k;
                           if (history[key_id][j].size() != 0)
                           for (k = 0; k < value1.size(); k++)
                              if (value1[k] != history[key_id][j][k])
                                 same = 0;
                           if (same) {
                              valueId = j;
                           }
                        }
                        int is_collision = 1;
                        int is_old = 0;
                        int ok = 0;
                        if (valueId == -1) {
                            if (history[key_id].size() == 2) {
                                int same = 1;
                                if (value2.size() != history[key_id][1].size())
                                    same = 0;
                                if (same == 1)
                                    for (int k = 0; k < value2.size(); k++)
                                        if (value2[k] != history[key_id][1][k])
                                            same = 0;
                                if (same == 1) {
                                    ok = 1;
                                }
                            }
                        }
                        if (ok == 0) {
                            if (valueId == -1)
                                valueId = history[key_id].size() - 1;
                            if (valueId == history[key_id].size() - 1) {
                                is_collision = 0; 
                            } else {
                                is_old = 1;
                                int same = 1;
                                if (history[key_id][valueId + 1].size() != value2.size()) {
                                   same = 0;
                                }
                                int k;
                                if (same != 0) {
                                    for (k = 0; k < value2.size(); k++)
                                    if (value2[k] != history[key_id][valueId + 1][k])
                                        same = 0;
                                }
                                if (same)
                                   is_collision = 0;
                            }
                            if (!is_old || is_collision) {
                            if (is_collision) {
                            // collision detected
                            printf("Collision detected\n");
                            std::vector<char> tmp;
                            history[key_id].push_back(tmp);
                            for (int i = 2; i < cnt_clients + 2; i++) {
                            write_message_to_vector("collision ", buf_write[i]);
                            write_message_to_vector(key, buf_write[i]);
                            write_message_to_vector("\n", buf_write[i]);
                            }
                            } else {
                            if (!is_old) {
                                // new value
                                printf("Added a new value\n");
                                history[key_id].push_back(value2);
                                for (int i = 2; i < cnt_clients + 2; i++) {
                                    write_message_to_vector("change ", buf_write[i]);
                                    write_message_to_vector(key, buf_write[i]);
                                    write_message_to_vector(" ", buf_write[i]);
                                    write_message_to_vector(value1, buf_write[i]);
                                    write_message_to_vector(" ", buf_write[i]);
                                    write_message_to_vector(value2, buf_write[i]);
                                    write_message_to_vector("\n", buf_write[i]);
                                }
                            }
                        }
                        }
                        }
                        } else {
                            printf("Not ok\n");
                        }
                    } else {
                        if (cur_message[0] = 'c' && cur_message[1] == 'o') {
                            // collision key
                            std::vector<char> key;
                            for (j = 10; j < cur_message.size() - 1; j++)
                                key.push_back(cur_message[j]);
                            int key_id = -1;
                            for (j = 0; j < all_keys.size(); j++) {
                                int same = 1;
                                if (all_keys[j].size() != key.size()) {
                                    continue;
                                }
                                int k;
                                for (k = 0; k < key.size(); k++)
                                    if (key[k] != all_keys[j][k])
                                        same = 0;
                                if (same) {
                                    key_id = j;
                                    break;
                                }
                            }
                            if (key_id == -1) {
                                printf("No such key (collision)\n"); 
                            } else {
                                if (history[key_id][history[key_id].size() - 1].size() != 0) {
                                    std::vector<char> tmp;
                                    history[key_id].push_back(tmp);
                                    my_write("Collision for key: ", strlen("Collision for key: "));
                                    my_write(key);
                                    for (int j = 2; j < cnt_clients + 2; j++) {
                                        write_message_to_vector("collision ", buf_write[j]);
                                        write_message_to_vector(key, buf_write[j]);
                                        write_message_to_vector("\n", buf_write[j]);
                                    }
                                }
                            }
                        } else {
                            // print key
                            std::vector<char> key;
                            for (j = 6; j < cur_message.size() - 1; j++)
                                key.push_back(cur_message[j]);
                            int key_id = -1;
                            for (j = 0; j < all_keys.size(); j++) {
                                int same = 1;
                                if (all_keys[j].size() != key.size()) {
                                    continue;
                                }
                                int k;
                                for (k = 0; k < key.size(); k++)
                                    if (key[k] != all_keys[j][k])
                                        same = 0;
                                if (same) {
                                    key_id = j;
                                    break;
                                }
                            }
                            if (key_id == -1) {
                                printf("No such key\n");
                            } else {
                                my_write("History for this key:\n", 22);
                                for (j = 1; j < history[key_id].size(); j++) {
                                    my_write(history[key_id][j]);
                                }
                            }
                        }
                    }
                    //
                    std::vector<char> next_vect;
                    for (j = firstN + 1; j < buf_read[i].size(); j++)
                        next_vect.push_back(buf_read[i][j]);
                    buf_read[i] = next_vect;
                }
            }
        }
        if (fd[0].revents & POLLIN) {
            int new_fd = accept(sfd, (struct sockaddr*)&peer_addr, &peer_addr_size);
            ok_fds.push_back(new_fd);
            std::vector<char> tmp1;
            buf_read.push_back(tmp1);
            std::vector<char> tmp2;
            buf_write.push_back(tmp2);
            printf("Connect to client\n");
        }
    }
    return 0;
}

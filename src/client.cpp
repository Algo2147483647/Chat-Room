#include "include.h"

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }

    // set address
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address_server;
    bzero(&address_server, sizeof(address_server));
    address_server.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address_server.sin_addr);
    address_server.sin_port = htons(port);

    // create and init socket
    int sockfd_server = socket(PF_INET, SOCK_STREAM, 0);

    if (connect(sockfd_server, (sockaddr*)&address_server, sizeof(address_server)) < 0) {
        printf("connection failed\n");
        close(sockfd_server);
        return 1;
    }

    // epoll control multiple concurrency
    epoll_event events[2];
    int epollfd = epoll_create(2);

    char read_buf[BUFFER_SIZE];
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return -1;

    Networks::addfd(epollfd, 0, false);
    Networks::addfd(epollfd, sockfd_server, false);

    // server close the connection
    bool ended = false;

    while (true) {
        int number = epoll_wait(epollfd, events, 2, -1);

        if (number < 0 && errno != EINTR) {
            printf("epoll failture\n");
            break;
        }

        for (int i = 0; i < number; ++i) {

            int active_sockfd = events[i].data.fd;

            if (active_sockfd == 0 && (events[i].events & EPOLLIN)) {
                // standard input has data that can be read (user input data)
                splice(0, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
                splice(pipefd[0], NULL, sockfd_server, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
            }
            else if (active_sockfd == sockfd_server && (events[i].events & EPOLLIN)) {
                // there is data readable on the socket (data sent by the server)
                memset(read_buf, '\0', BUFFER_SIZE);
                int ret = recv(sockfd_server, read_buf, BUFFER_SIZE - 1, 0);

                if (ret > 0) {
                    printf("%s\n", read_buf);
                }
                else if (ret == 0) {
                    printf("server close the connection\n");
                    ended = true;
                    break;
                }
            }
        }

        if (ended)
            break;
    }

    close(sockfd_server);
    close(epollfd);
    return 0;
}


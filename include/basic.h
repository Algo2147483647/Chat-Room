#ifndef BASIC_H
#define BASIC_H

#include "include.h"
#include "thread_pool.h"

namespace Networks {

    int setnonblocking(int fd) {
        int old_option = fcntl(fd, F_GETFL),
            new_option = old_option | O_NONBLOCK;

        fcntl(fd, F_SETFL, new_option);
        return old_option;
    }

    void addfd(int epoll_fd, int fd, bool hup) {
        epoll_event event;
        event.data.fd = fd;
        event.events = EPOLLIN | EPOLLERR;

        if (hup) {
            event.events |= EPOLLRDHUP;
        }
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
        setnonblocking(fd);
    }

    /*
     * create and return a listened socket
     */
    int create_listen_socket(const char* ip, int port) {
        // set address 
        sockaddr_in address;
        bzero(&address, sizeof(address));
        address.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &address.sin_addr);
        address.sin_port = htons(port);

        // create listen socket
        int listen_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
        int on = 1;

        if (setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
            printf("setsockopt error!\n");
            return -1;
        }

        if (bind(listen_socket_fd, (sockaddr*)&address, sizeof(address)) == -1) {
            printf("bind error!\n");
            return -1;
        }

        if (listen(listen_socket_fd, 5) == -1) {
            printf("set listen socket error!\n");
            return -1;
        }

        return listen_socket_fd;
    }


    int create_connection_socket(int listen_socket_fd, vector<int>& users_fd,
        map<int, Networks::ClientData*>& users, int epoll_fd) {
        sockaddr_in client_address;
        socklen_t client_addrlength = sizeof(client_address);
        int connection_socket_fd = accept(listen_socket_fd, (sockaddr*)&client_address, &client_addrlength);

        if (connection_socket_fd < 0) {
            printf("errno is: %d\n", errno);
            return -1;
        }

        // if client quantity overflow
        if (users_fd.size() >= USER_LIMIT) {
            const char* info = "too many users\n";
            printf("%s", info);
            send(connection_socket_fd, info, strlen(info), 0);
            close(connection_socket_fd);
            return -1;
        }

        // add this new connection_socket_fd into epoll tree and array of users
        users_fd.push_back(connection_socket_fd);
        users[connection_socket_fd] = new Networks::ClientData();
        users[connection_socket_fd]->address = client_address;
        users[connection_socket_fd]->fds_index = users_fd.size() - 1;

        Networks::addfd(epoll_fd, connection_socket_fd, true);

        // add readable event
        epoll_event event;

        event.data.fd = connection_socket_fd;
        event.events = EPOLLERR | EPOLLRDHUP;
        event.events |= EPOLLOUT;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, connection_socket_fd, &event);

        return connection_socket_fd;
    }

    void delete_connection_socket(int socket_fd, vector<int>& users_fd,
        map<int, Networks::ClientData*>& users, int epoll_fd) {
        int t = users[socket_fd]->fds_index;
        swap(users_fd[t], users_fd.back());
        users[users_fd[t]]->fds_index = t;

        users_fd.pop_back();
        close(socket_fd);
    }

    /*
     * send message with epoll
     */
    void send_message_epoll(int socket_fd, int epoll_fd) {
        // Trigger writable events of other user sockets
        epoll_event event;

        event.data.fd = socket_fd;
        event.events = EPOLLERR | EPOLLRDHUP;
        event.events |= EPOLLOUT;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, socket_fd, &event);
    }

    void send_message(int connection_socket_fd, Networks::ClientData* user) {
        int cur = 0, 
            n = user->buf.size();

        while(cur < n) {
            string s = user->buf.substr(cur, min(n - cur, 65535));

            int ret = send(
                connection_socket_fd,
                s.c_str(),
                s.size(), 0
            );
            cur += 65535;
        }
    }

    void send_message_(int connection_socket_fd, Networks::ClientData* user, int epoll_fd) {
        send_message(connection_socket_fd, user);

        // Re-register the readable event and mask the read event on the current socket during the sending process
        epoll_event event;
        event.data.fd = connection_socket_fd;
        event.events = EPOLLERR | EPOLLRDHUP;
        event.events |= EPOLLIN;
        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, connection_socket_fd, &event);
    }

    void group_message_epoll(string& message, vector<int>& socket_fds,
                             map<int, Networks::ClientData*>& users, int epoll_fd) {
        int user_count = socket_fds.size();

        for (int i = 0; i < user_count; i++) {
            users[socket_fds[i]]->buf = message;
            Networks::send_message_epoll(socket_fds[i], epoll_fd);
        }
    }

}

#endif // !NETWORKS_H

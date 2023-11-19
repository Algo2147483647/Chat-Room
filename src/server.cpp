#include "Server.h"

void Server::initializeDatabase() {
    m_mysql = Networks::mysql_init();
    if (!m_mysql) {
        throw std::runtime_error("Failed to initialize MySQL.");
    }
}

void Server::createListenSocket(const char* ip, int port) {
    listen_socket_fd = Networks::create_listen_socket(ip, port);
    if (listen_socket_fd == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to create listening socket");
    }
}

void Server::runServerLoop() {
    epoll_event events[USER_LIMIT + 1];
    while (true) {
        int number = epoll_wait(epoll_fd, events, USER_LIMIT + 1, -1);
        if (number < 0) {
            if (errno != EINTR) {
                std::cerr << "epoll failure" << std::endl;
                break;
            }
            continue;
        }

        for (int i = 0; i < number; ++i) {
            handleEvent(events[i]);
        }
    }
}

void Server::handleEvent(const epoll_event &event) {
    int fd = event.data.fd;

    if (fd == listen_socket_fd) {
        handleNewConnection();
    } else if (event.events & EPOLLERR) {
        handleErrorEvent(fd);
    } else if (event.events & EPOLLRDHUP) {
        handleDisconnect(fd);
    } else if (event.events & EPOLLIN) {
        handleReadEvent(fd);
    } else if (event.events & EPOLLOUT) {
        handleWriteEvent(fd);
    } else {
        std::cerr << "Unhandled event type" << std::endl;
    }
}

void Server::handleNewConnection() {
    // event of listen socket, mean a new connection  
    int connection_socket_fd = create_connection_socket(listen_socket_fd, users_fd, users_data, epoll_fd);
    if (connection_socket_fd != -1)
        printf("comes a new user, now have %d users\n", (int)users_fd.size());
}

void Server::handleErrorEvent(int fd) {
    std::array<char, 100> errors;
    errors.fill('\0');
    socklen_t length = errors.size();

    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, errors.data(), &length) < 0) {
        std::cerr << "Error getting socket options: " << std::strerror(errno) << std::endl;
    }
}

void Server::handleReadEvent(int fd) {
    std::vector<char> buf(BUFFER_SIZE);

    int ret = recv(fd, buf.data(), buf.size() - 1, 0);
    if (ret < 0) {
        if (errno != EAGAIN) {
            std::cerr << "Error reading from socket: " << std::strerror(errno) << std::endl;
            // Handle error, possibly by disconnecting the client.
        }
    } else if (ret > 0) {
        buf[ret] = '\0'; // Ensure null-termination
        std::string dataReceived(buf.begin(), buf.begin() + ret);
        users_data[fd]->buf = dataReceived;

        std::cout << "Received " << ret << " bytes of client (" << fd << ") data: " << dataReceived << std::endl;

        threadPool.addTask(
            Networks::app_http,
            dataReceived,
            fd,
            users_data[fd].get(),
            epoll_fd,
            m_mysql
        );
    }
}

void Server::handleWriteEvent(int fd) {
    int connection_socket_fd = events[i].data.fd;

    Networks::send_message_(
        connection_socket_fd,
        users_data[connection_socket_fd],
        epoll_fd
    );
}


Server::Server() : epoll_fd(epoll_create(USER_LIMIT + 1)) {
    if (epoll_fd == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to create epoll file descriptor");
    }

    threadPool.init();
}

void Server::start(const char* ip, int port) {
    initMysql();
    createListenSocket(ip, port);
    Networks::addfd(epoll_fd, listen_socket_fd, false);
    runServerLoop();
}

Server::~Server() {
    close(listen_socket_fd);
    close(epoll_fd);
    mysql_close(&m_mysql);
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " ip_address port_number" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        const char* ip = argv[1];
        int port = std::stoi(argv[2]);

        Server server;
        server.start(ip, port);

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
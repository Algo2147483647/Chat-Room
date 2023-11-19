#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <unordered_map>
#include <array>
#include <stdexcept>
#include <system_error>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include "Application.h"
#include "ThreadPool.h"
#include "ClientData.h"
#include "NetworkUtilities.h"
#include "Database.h"

class Server {
private:
    Database m_database;
    ThreadPool m_threadPool;
    std::unordered_map<int, std::unique_ptr<ClientData>> m_usersData;
    std::vector<int> m_usersFileDescriptors;
    int m_epollFileDescriptor;
    int m_listenSocketFileDescriptor;

    void initializeDatabase();
    void createListenSocket(const std::string& ipAddress, int port);
    void runServerLoop();
    void handleEvent(const epoll_event& event);
    void handleNewConnection();
    void handleErrorEvent(int fileDescriptor);
    void handleReadEvent(int fileDescriptor);
    void handleWriteEvent(int fileDescriptor);

public:
    Server();
    void start(const std::string& ipAddress, int port);
    ~Server();
};

#endif // SERVER_H

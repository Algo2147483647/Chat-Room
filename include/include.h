#ifndef INCLUDE_H
#define INCLUDE_H

#include <algorithm>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <map>
#include <netinet/in.h>
#include <pthread.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

#define STDIN 0
#define STDOUT 1
#define STDERR 2 

#define BUFFER_SIZE 65535
#define MYSQL_IP "127.0.0.1"

#define USER_LIMIT 5
#define FD_LIMIT 65535

using namespace std;

void string_split(string str, const string& splits, vector<string>& answer) {
    if (str == "")
        return;

    size_t pos = str.find(splits);
    int step = splits.size();

    while (pos != str.npos) {
        answer.push_back(str.substr(0, pos));
        str = str.substr(pos + step, str.size());
        pos = str.find(splits);
    }
    if (str != "")
        answer.push_back(str);
}

string& readFile (string url, string& ret) {
    FILE *fin = fopen(url.c_str(), "r");

    if(fin == NULL)
        return ret = "";

    char ch;
    while ((ch = fgetc(fin)) != EOF) 
        ret += ch;
    fclose(fin);

    return ret;
}

namespace Networks {

struct ClientData {
    int fds_index;
    sockaddr_in address;
    int id = 0;
    string buf;
};

}

#endif
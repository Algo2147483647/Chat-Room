#ifndef APPLICATION_H
#define APPLICATION_H

#include "include.h"
#include "HTTP.h"
#include "logIn_register.h"

namespace Networks {
vector<string> messages;

void app_cgi(HTTP::HttpRequset& hq, HTTP::HttpResponse& hs, MYSQL& m_mysql); 


void app_http(string rev_str, int connection_socket_fd, ClientData* user, int epoll_fd, MYSQL& m_mysql) {

    // struct 
    HTTP::HttpRequset  hq;
    HTTP::HttpResponse hs;

    read(hq, rev_str);

    if (hq.urlFields.find("action") != hq.urlFields.end()) {
        app_cgi(hq, hs, m_mysql);
    }
    else {
        hq.url = HTTP::url_process(hq.url);
        hs.headerFields["content_type"] = HTTP::file_type(hq.url);
        readFile(hq.url, hs.body);
    }

    hs.headerFields["Connection"] = "keep-alive";
    user->buf = to_string(hs);
    printf(">> %s\n", user->buf.c_str());
    Networks::send_message_epoll(connection_socket_fd, epoll_fd);
}

void app_cgi(HTTP::HttpRequset& hq, HTTP::HttpResponse& hs, MYSQL& m_mysql) {

    if (hq.urlFields["action"] == "login") {
        int id = atoi(hq.urlFields["user_id"].c_str());
        long long key = atoi(hq.urlFields["password"].c_str());
        string name;

        if(!check_user_key(m_mysql, id, key, name)) {
            hs.headerFields["content_type"] = "text/plain";
            hs.body = "Wrong ID or password!";
            return;
        } else {
            hq.url = HTTP::url_process(hq.url);
            hs.headerFields["content_type"] = HTTP::file_type(hq.url);
            readFile(hq.url, hs.body);
            return;
        }
    }
    else if (hq.urlFields["action"] == "message") {
        messages.push_back(hq.urlFields["message"]);

        hs.body = "ok";
        hs.headerFields["content_type"] = "text/plain";
        return;
    }
    else if (hq.urlFields["action"] == "get_message") {
        int st = atoi(hq.urlFields["m_id"].c_str());

        for (int i=st;i<messages.size();i++) {
            hs.body += messages[i] + '&';
        }
        hs.headerFields["content_type"] = "text/plain";
        return;
    }
    return;
}

// handleAction simplification
HTTP::HttpResponse handleAction(const HTTP::HttpRequset& hq, MYSQL& m_mysql) {
    HTTP::HttpResponse hs;

    if (hq.action == "login") {
        hs = handleLogin(hq, m_mysql);
    } else if (hq.action == "message") {
        hs = handleMessage(hq);
    } else if (hq.action == "get_message") {
        hs = handleGetMessage(hq);
    }

    return hs;
}

    
}

#endif
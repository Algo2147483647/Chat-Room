#ifndef HTTP_H
#define HTTP_H

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <fstream>
#include <array>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "HTTP_message.h"

namespace HTTP {

    const std::string HTML_ROOT_DIR = "../html";
    const std::map<int, std::string> ERROR_MESSAGES = {
        {400, "Your browser sent a bad request, such as a POST without a Content-Length."},
        {500, "Error prohibited CGI execution."}
    };

    enum class Method {
        GET,
        POST,
        PUT,
        // Further methods can be added here
    };

    std::string get_content_type(const std::string& extension) {
        static const std::map<std::string, std::string> mime_types = {
            {"js", "text/javascript"},
            {"css", "text/css"},
            {"html", "text/html"},
            {"jpg", "image/jpeg"},
            {"png", "image/png"},
            {"txt", "text/plain"}
        };

        auto it = mime_types.find(extension);
        return it != mime_types.end() ? it->second : "text/plain";
    }

    std::string extract_file_extension(const std::string& url) {
        size_t last_dot = url.rfind('.');
        if (last_dot == std::string::npos) return {};
        return url.substr(last_dot + 1);
    }

    std::string url_to_path(const std::string& url) {
        if (url == "/") return HTML_ROOT_DIR + "/index.html";
        return HTML_ROOT_DIR + url;
    }

    std::string generate_bad_response(int error_code) {
        std::string response = "HTTP/1.0 " + std::to_string(error_code) + " ";
        auto message_iter = ERROR_MESSAGES.find(error_code);
        if (message_iter != ERROR_MESSAGES.end()) {
            response += message_iter->second;
        }
        response += "\r\nContent-type: text/html\r\n\r\n";
        if (message_iter != ERROR_MESSAGES.end()) {
            response += "<p>" + message_iter->second + "\r\n";
        }
        return response;
    }

    std::string execute_cgi(int client, const std::string& path, const std::string& method, const std::string& query_string, int content_length) {
        std::array<int, 2> cgi_out, cgi_in;
        if (pipe(cgi_out.data()) < 0 || pipe(cgi_in.data()) < 0) {
            return generate_bad_response(500);
        }

        pid_t pid = fork();
        if (pid < 0) {
            return generate_bad_response(500);
        }

        if (pid == 0) { // Child process
            dup2(cgi_out[1], STDOUT_FILENO);
            dup2(cgi_in[0], STDIN_FILENO);
            close(cgi_out[0]);
            close(cgi_in[1]);

            std::string env_query_string = "QUERY_STRING=" + query_string;
            std::string env_content_length = "CONTENT_LENGTH=" + std::to_string(content_length);
            char* env[] = { strdup(env_query_string.c_str()), strdup(env_content_length.c_str()), nullptr };

            if (execle(path.c_str(), path.c_str(), nullptr, env) == -1) {
                exit(1);
            }
        }
        else { // Parent process
            close(cgi_out[1]);
            close(cgi_in[0]);
            char c;
            if (method == "POST") {
                for (int i = 0; i < content_length; i++) {
                    recv(client, &c, 1, 0);
                    write(cgi_in[1], &c, 1);
                }
            }
            while (read(cgi_out[0], &c, 1) > 0) {
                send(client, &c, 1, 0);
            }

            close(cgi_out[0]);
            close(cgi_in[1]);
            waitpid(pid, nullptr, 0);
        }
        return "HTTP/1.0 200 OK\r\n";
    }

    std::string process_http_request(const std::string& in) {
        HttpRequset request;
        HttpResponse response;

        read_request(request, in); // Assuming read_request is a valid function that parses the HTTP request.
        std::string url = url_to_path(request.url);
        std::string extension = extract_file_extension(url);
        response.headerFields["Content-type"] = get_content_type(extension);
        read_file(url, response.body); // Assuming read_file is a valid function that reads the file content.

        return response.to_string(); // Assuming HttpResponse has a to_string member function.
    }

} // namespace HTTP
#endif

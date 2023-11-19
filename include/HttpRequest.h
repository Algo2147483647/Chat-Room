#include "HTTP.h"
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <iterator>

class HttpRequest {
private:
    Method method_;
    std::string url_;
    std::string version_ = "HTTP/1.1";
    std::string body_;
    std::map<std::string, std::string> header_fields_;
    std::map<std::string, std::string> url_fields_;

public:
    // Constructors
    HttpRequest(Method method, std::string url)
        : method_(method), url_(std::move(url)) {}

    // Setters
    void set_method(Method method) {
        method_ = method;
    }

    void set_url(const std::string& url) {
        url_ = url;
    }

    void set_version(const std::string& version) {
        version_ = version;
    }

    void set_body(const std::string& body) {
        body_ = body;
    }

    void add_header_field(const std::string& key, const std::string& value) {
        header_fields_[key] = value;
    }

    // Getters
    Method get_method() const {
        return method_;
    }

    std::string get_url() const {
        return url_;
    }

    std::string get_version() const {
        return version_;
    }

    std::string get_body() const {
        return body_;
    }

    std::string get_header_field(const std::string& key) const {
        auto it = header_fields_.find(key);
        return it != header_fields_.end() ? it->second : "";
    }

    // Function to read and parse HttpRequest
    void read(const std::string& in) {
        std::istringstream stream(in);
        std::string line;

        // Read the request line
        std::getline(stream, line);
        std::istringstream line_stream(line);
        std::string method_str;
        line_stream >> method_str >> url_ >> version_;

        // Convert method string to enum
        method_ = string_to_method(method_str); // You need to implement this function

        // Read headers
        while (std::getline(stream, line) && line != "\r") {
            auto colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                auto key = line.substr(0, colon_pos);
                auto value = line.substr(colon_pos + 2);
                header_fields_[key] = value;
            }
        }

        // Read body if present
        if (header_fields_.find("Content-Length") != header_fields_.end()) {
            std::getline(stream, body_, '\0');
        }
    }

    // Function to convert HttpRequest to a string
    std::string to_string() const {
        std::ostringstream out;

        // Request Line
        out << method_to_string(method_) << ' ' << url_ << ' ' << version_ << "\r\n";

        // Headers
        for (const auto& [key, value] : header_fields_) {
            out << key << ": " << value << "\r\n";
        }

        // Body
        if (!body_.empty()) {
            out << "Content-length: " << body_.length() << "\r\n";
            out << "\r\n" << body_;
        }

        return out.str();
    }

    // Helper functions to convert Method enum to string and vice versa
    std::string method_to_string(Method method) const {
        // Implement this method to convert from enum to string
    }

    Method string_to_method(const std::string& method_str) const {
        // Implement this method to convert from string to enum
    }
};

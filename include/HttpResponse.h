#include "HTTP.h"

// HttpResponse class
class HttpResponse {
private:
    int status_ = 200;
    std::string version_ = "HTTP/1.1";
    std::string body_;
    std::map<std::string, std::string> header_fields_;

public:
    // Constructor with status code
    explicit HttpResponse(int status) : status_(status) {}

    // Constructor with status code and body
    HttpResponse(int status, const std::string& body) : status_(status), body_(body) {}

    // Constructor with status code, body, and headers
    HttpResponse(int status, const std::string& body, const std::map<std::string, std::string>& headers)
        : status_(status), body_(body), header_fields_(headers) {}

    // Getters
    int get_status() const { return status_; }
    const std::string& get_version() const { return version_; }
    const std::string& get_body() const { return body_; }
    const std::map<std::string, std::string>& get_header_fields() const { return header_fields_; }

    // Setters
    void set_status(int status) { status_ = status; }
    void set_version(const std::string& version) { version_ = version; }
    void set_body(const std::string& body) { body_ = body; }
    void set_header_field(const std::string& field, const std::string& value) {
        header_fields_[field] = value;
    }


    // Function to read and parse HttpResponse
    void read(const std::string& in);

    // Function to convert HttpResponse to a string
    std::string to_string() const;
};

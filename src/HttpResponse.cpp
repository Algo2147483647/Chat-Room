#include "HttpResponse.h"

void HttpResponse::read(const std::string& in) {
    std::istringstream stream(in);
    std::string line;

    // Parse status line
    if (std::getline(stream, line)) {
        std::istringstream line_stream(line);
        line_stream >> version_ >> status_;
        // Skip the status message
    }

    // Parse headers
    while (std::getline(stream, line) && line != "\r") {
        auto colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string field = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 2);
            header_fields_[field] = value;
        }
    }

    // Parse body
    if (header_fields_.find("Content-length") != header_fields_.end()) {
        std::string content_length = header_fields_["Content-length"];
        size_t length = std::stoi(content_length);
        body_.resize(length);
        stream.read(&body_[0], length);
    }
}

std::string HttpResponse::get_status_message(int status_code) {
    switch (status_code) {
    case 200: return "OK";
    case 404: return "Not Found";
        // ... other status codes ...
    default: return "Unknown";
    }
}

std::string HttpResponse::to_string() const {
    std::ostringstream out;

    // Status Line
    out << version_ << ' ' << status_ << ' ' << get_status_message(status_) << "\r\n";

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

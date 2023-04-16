//
// Created by 26685 on 2023/3/21.
//
#include "http.h"
#include <vector>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void http_request::try_decode(const std::string &buf) {
    this->parse_internal(buf.c_str(), buf.length());
}

const string& http_request::get_method() const {
    return _method;
}
const string& http_request::get_url() const {
    return _url;
}
const unordered_map<string, string>& http_request::get_request_params() const {
    return _request_params;
}
const string& http_request::get_protocol() const {
    return _protocol;
}
const string& http_request::get_version() const {
    return _version;
}
const unordered_map<string, string>& http_request::get_headers() const {
    return _header;
}
const string& http_request::get_body() const {
    return _body;
}
/////////////////////////////////////////////////////////////////////////////////
void http_request::parse_internal(const char *buf, int size) {
    string_buffer method;
    string_buffer url;

    string_buffer request_params_key;
    string_buffer request_params_value;

    string_buffer protocol;
    string_buffer version;

    string_buffer header_key;
    string_buffer header_value;

    int body_length = 0;
    char* tmp = const_cast<char*>(buf + _next_pos);
    while (_decode_state != http_request_decode_state::INVALID
           &&_decode_state!=http_request_decode_state::INVALID_METHOD
           &&_decode_state!=http_request_decode_state::INVALID_URI
           &&_decode_state!=http_request_decode_state::INVALID_VERSION
           &&_decode_state!=http_request_decode_state::INVALID_HEADER
           &&_decode_state!=http_request_decode_state::COMPLEIE
           &&_next_pos < size)  {

        char ch = *tmp; // cur char
        char* p = tmp++; //
        int scan_pos = _next_pos++;
        switch (_decode_state) {
            case http_request_decode_state::START: {
                if (ch == CR || ch == LF || isblank(ch)) { /* nothing*/}
                else if (isupper(ch)) {
                    method.begin = p;
                }
            }
        }
    }



}
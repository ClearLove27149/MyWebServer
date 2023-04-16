//
// Created by 26685 on 2023/3/21.
//
#pragma once
#ifndef MYWEBSERVER_HTTP_H
#define MYWEBSERVER_HTTP_H

#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

#define CR '\r' // enter
#define LF '\n' // next line

struct string_buffer {
    char* begin = NULL;
    char* end = NULL;
    operator std::string() const {
        return std::string(begin, end);
    }
};
/***
 * http 请求行的状态
 * */
enum class http_request_decode_state {
    INVALID,                //无效
    INVALID_METHOD,         //无效请求方法
    INVALID_URI,            //无效的请求路径
    INVALID_VERSION,        //无效的协议版本号
    INVALID_HEADER,         //无效请求头

    START,                  //请求行开始
    METHOD,                 //请求方法

    BEFORE_URI,             //请求连接前的状态,需要'/'开头
    IN_URI,                 //url处理
    BEFORE_URI_PARAM_KEY,   //URL请求参数键之前
    URI_PARAM_KEY,          //URL请求参数键
    BEFORE_URI_PARAM_VALUE, //URL的参数值之前
    URI_PARAM_VALUE,        //URL请求参数值

    BEFORE_PROTOCAL,        //协议解析之前
    PROTOCAL,               //协议

    BEFORE_VERSION,         //版本开始之前
    VERSION_SPLIT,          //版本分割符号'.'
    VERSION,                //版本

    HEADER_KEY,
    HEADER_BEFORE_COLON,    //请求头冒号之前
    HEADER_AFTER_COLON,     //请求头冒号之后
    HEADER_VALUE,           //值

    WHEN_CR,                //遇到一个回车
    CR_LF,                  //回车换行
    CR_LF_CR,               //回车换行之后

    BODY,                   //请求体
    COMPLEIE                //完成
};

class http_request {
public:
    void try_decode(const std::string& buf);
    const string& get_method() const;
    const string& get_url() const;
    const unordered_map<string, string>& get_request_params() const;
    const string& get_protocol() const;
    const string& get_version() const;
    const unordered_map<string, string>& get_headers() const;
    const string& get_body() const;

private:
    void parse_internal(const char* buf, int size);
    std::string _method;
    std::string _url;
    std::string _protocol;
    std::string _version;
    std::unordered_map<string, string> _header;
    std::string _body;
    int _next_pos = 0;
    std::unordered_map<string, string> _request_params;
    http_request_decode_state _decode_state = http_request_decode_state::START;
};





#endif //MYWEBSERVER_HTTP_H

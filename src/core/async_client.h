//
// Created by 26685 on 2023/3/12.
//

#ifndef MYWEBSERVER_ASYNC_CLIENT_H
#define MYWEBSERVER_ASYNC_CLIENT_H

#include "asio.hpp"
#include <array>
#include <iostream>

const int MAX_PACK_SIZE = 65536;
const int HEAD_LEN = 4;

class Async_Client {
private:
    asio::ip::tcp::socket m_socket;
    asio::ip::tcp::resolver m_resolver;
    char cin_buf_[MAX_PACK_SIZE];
    std::array<char, MAX_PACK_SIZE> buf_;

public:
    Async_Client(asio::io_context& ioc,
    const std::string& host, const std::string& port)
    : m_socket(ioc), m_resolver(ioc) {
        m_resolver.async_resolve(asio::ip::tcp::v4(), host, port,
                                std::bind(&Async_Client::OnResolve, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
    }

    void OnResolve(const asio::error_code& ec, asio::ip::tcp::resolver::results_type endpoints) {
        if (ec) {
            std::cout << "Resolve: " << ec.message() << std::endl;
        } else {
            asio::async_connect(m_socket, endpoints,
                                       std::bind(&Async_Client::OnConnect, this,
                                                 std::placeholders::_1,
                                                 std::placeholders::_2));
        }
    }

    void OnConnect(const asio::error_code& ec, asio::ip::tcp::resolver::endpoint_type endpoints) {
        if (ec) {
            std::cout << "Connect failed: " << ec.message() << std::endl;
            m_socket.close();
        } else {
            DoWrite();
        }
    }
    void DoWrite() {
        std::size_t len = 0;
        //do {
            std::cout << "Enter message: ";
            std::cin.getline(cin_buf_, MAX_PACK_SIZE);
            len = strlen(cin_buf_);
        //} while (len == 0);

        asio::async_write(m_socket, asio::buffer(cin_buf_, len),
                                 std::bind(&Async_Client::OnWrite, this,
                                           std::placeholders::_1));
    }
    void OnWrite(const asio::error_code& ec) {
        if (!ec) {
            std::cout << "Reply is: ";

            m_socket.async_read_some(asio::buffer(buf_),
                                    std::bind(&Async_Client::OnRead, this,
                                              std::placeholders::_1,
                                              std::placeholders::_2));
        }
    }
    void OnRead(const asio::error_code& ec, std::size_t length) {
        if (!ec) {
            std::cout << length <<std::endl;
            std::cout.write(buf_.data(), length);
            std::cout << std::endl;
            // 如果想继续下一轮，可以在这里调用 DoWrite()。
            DoWrite();
        }
    }
};

void TestClient() {
    std::string host = "127.0.0.1";
    std::string port = "9090";

    asio::io_context ioc;
    Async_Client client(ioc, host, port);

    ioc.run();
}
#endif //MYWEBSERVER_ASYNC_CLIENT_H

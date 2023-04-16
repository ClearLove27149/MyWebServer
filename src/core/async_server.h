//
// Created by 26685 on 2023/3/12.
//

#ifndef MYWEBSERVER_ASYNC_SERVER_H
#define MYWEBSERVER_ASYNC_SERVER_H

#include "session.hpp"

class Async_Server {
private:
    asio::ip::tcp::acceptor m_acceptor;
    void DoAccept() {
        m_acceptor.async_accept([this](const asio::error_code& ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->Start();
            }
            DoAccept();
        });
    }
public:
    Async_Server(asio::io_context& ioc, std::uint16_t port)
            : m_acceptor(ioc, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
        DoAccept();
    }
};

void TestServer() {
    std::uint16_t port = 9090;

    asio::io_context ioc;
    Async_Server server(ioc, port);

    ioc.run();
}
#endif //MYWEBSERVER_ASYNC_SERVER_H

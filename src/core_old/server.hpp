//
// Created by 26685 on 2023/3/11.
//

#ifndef MYWEBSERVER_SERVER_H
#define MYWEBSERVER_SERVER_H

#include <unordered_map>
#include <list>
#include <numeric>
#include "rw_handler.hpp"
//#include "message.hpp"

const int MaxConnectionNum = 65536;
const int MaxRecvSize = 65536;

class Server {
private:
    asio::io_service& m_ios;
    asio::ip::tcp::acceptor m_acceptor;
    std::list<int> m_connIdPool;
    std::unordered_map<int, std::shared_ptr<RWHandler>> m_handlers;

    std::shared_ptr<RWHandler> CreateHandler() {
        int connId = m_connIdPool.front();
        m_connIdPool.pop_front();
        std::shared_ptr<RWHandler> handler = std::make_shared<RWHandler>(m_ios);
        handler->SetConnId(connId);
        handler->SetCallBackError([this] (int connId) {
            RecyclConnid(connId);
        });
        return handler;
    }
    void RecyclConnid(int connId) {
        auto it = m_handlers.find(connId);
        if (it != m_handlers.end()) {
            m_handlers.erase(it);
        }
        std::cout << "Current connect count: " << m_handlers.size() <<std::endl;
        m_connIdPool.push_back(connId);
    }
    void HandlerAcceptError(std::shared_ptr<RWHandler> handler, const asio::error_code& ec) {
        std::cout << "Error, error reason: " << ec.value() << ec.message() << std::endl;
        handler->CloseSocket();
        StopAccept();
    }
    void StopAccept() {
        asio::error_code ec;
        m_acceptor.cancel(ec);
        m_acceptor.close(ec);
        m_ios.stop();
    }
public:
    Server(asio::io_service& ios, short port) : m_ios(ios), m_acceptor(ios, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
                                                m_connIdPool(MaxConnectionNum) {
        m_connIdPool.resize(MaxConnectionNum);
        std::iota(m_connIdPool.begin(), m_connIdPool.end(), 1);
    }
    ~Server() { }
    void Accept() {
        std::cout << "Start Listen" <<std::endl;
        std::shared_ptr<RWHandler> handler = CreateHandler();
        m_acceptor.async_accept(handler->GetSocket(), [this, handler] (const asio::error_code& ec) {
            if (ec) {
                std::cout << ec.value() << " " << ec.message() << std::endl;
                HandlerAcceptError(handler, ec);
                return;
            }
            m_handlers.insert(std::make_pair(handler->GetConnId(), handler));
            std::cout << "Current connect count: " <<m_handlers.size() << std::endl;

            handler->HandleAsyncRead();

            Accept();
        });
    }
};

// test
void TestServer() {
    asio::io_service ios;
    Server server(ios, 9090);
    server.Accept();
    ios.run();
}
#endif //MYWEBSERVER_SERVER_H

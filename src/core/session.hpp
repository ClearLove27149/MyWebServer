//
// Created by 26685 on 2023/3/12.
//

#ifndef MYWEBSERVER_SESSION_H
#define MYWEBSERVER_SESSION_H

#include "asio.hpp"
#include <iostream>
#include <array>

const int MAX_PACK_SIZE = 65536;
const int HEAD_LEN = 4;

class Session : public std::enable_shared_from_this<Session> {
private:
    asio::ip::tcp::socket m_socket;
    std::array<char, MAX_PACK_SIZE> m_buff;
public:
    Session(asio::ip::tcp::socket socket) : m_socket(std::move(socket)) { }

    void Start(){
        DoRead();
    }
    void DoRead() {
        auto self(shared_from_this());
        m_socket.async_read_some(asio::buffer(m_buff),
                         [this,self] (const asio::error_code& ec, std::size_t len) {
                             if (!ec) {
                                 DoWrite(len);
                             }
                         });
    }
//    void DoRead() {
//        auto self(shared_from_this());
//        asio::async_read(m_socket, asio::buffer(m_buff),
//                         [this,self] (const asio::error_code& ec, std::size_t len) {
//           if (!ec) {
//               DoWrite(len);
//           }
//        });
//    }

    void DoWrite(std::size_t len) {
        std::cout << len <<std::endl;
        auto self(shared_from_this());
        asio::async_write(m_socket, asio::buffer(m_buff, len), [this,self] (const asio::error_code& ec, std::size_t len) {
            if (!ec) {
                DoRead();
            }
        });
    }
};
#endif //MYWEBSERVER_SESSION_H

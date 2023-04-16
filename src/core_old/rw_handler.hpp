//
// Created by 26685 on 2023/3/11.
//

#ifndef MYWEBSERVER_RW_HANDLER_H
#define MYWEBSERVER_RW_HANDLER_H

#include <array>
#include <functional>
#include <iostream>

#include "asio.hpp"

const int MAX_IP_PACK_SIZE = 65536;
const int HEAD_LEN = 4;

class RWHandler {
public:
    RWHandler(asio::io_service& ios) : m_sock(ios) {}
    ~RWHandler() {}

    void HandleAsyncRead() {
        // 三种情况下会返回：缓冲区满；transfer_at_least为真（收到特定数量字节）；有错误发生
        asio::async_read(m_sock,asio::buffer(m_buff),
        asio::transfer_at_least(HEAD_LEN), [this] (const asio::error_code& ec, size_t size) {
            if (ec) {
                HandleError(ec);
                return;
            }
            std::cout << " : " << m_buff.data() + HEAD_LEN << std::endl;
            HandleAsyncRead();
            //m_buff.fill('\0');
            //std::string str("you are ok\n");
            //HandleWrite(const_cast<char *>(str.c_str()), str.length() + 1);

        });
    }
    void HandleRead() {
        //asio::read(m_sock, asio::buffer(m_buff_recv));
        //asio::error_code ec;
        m_sock.receive(asio::buffer(m_buff_recv));
        std::cout << m_buff_recv.data() << std::endl;
    }
    // 同步写
    void HandleWrite(char* data, int len) {
        asio::error_code ec;
        asio::write(m_sock, asio::buffer(data, len), ec);
        if (ec) {
            HandleError(ec);
        }
        //m_sock.send(asio::buffer(data, len));

    }
    // async write
    void HandleAsyncWrite(char* data, int len) {
        //todo
        asio::async_write(m_sock, asio::buffer(data, len), asio::transfer_at_least(HEAD_LEN),
                          [this] (const asio::error_code& ec, size_t size) {
            if (!ec) {
                
                HandleAsyncRead();
            }
        });
    }
    asio::ip::tcp::socket& GetSocket() {
        return m_sock;
    }
    void CloseSocket() {
        asio::error_code ec;
        m_sock.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
        m_sock.close(ec);
    }
    void SetConnId(int connId) {
        m_connId = connId;
    }
    int GetConnId() {
        return m_connId;
    }

    template<typename F>
    void SetCallBackError(F f) {
        m_callbackError = f;
    }
    void HandleError(const asio::error_code& ec) {
        CloseSocket();
        std::cout << ec.message() << std::endl;
        if (m_callbackError) {
            m_callbackError(m_connId);
        }
    }
private:
    asio::ip::tcp::socket m_sock;
    int m_connId;
    std::array<char, MAX_IP_PACK_SIZE> m_buff, m_buff_recv;
    std::function<void(int)> m_callbackError;
};

#endif //MYWEBSERVER_RW_HANDLER_H

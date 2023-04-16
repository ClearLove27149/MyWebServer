//
// Created by 26685 on 2023/3/11.
//

#ifndef MYWEBSERVER_CONNECTOR_H
#define MYWEBSERVER_CONNECTOR_H

#include <iostream>
#include <thread>
#include "rw_handler.hpp"

using namespace asio;
using namespace std;
class Connector {
public:
    Connector(io_service& ios, const string ip, short port): m_ios(ios), m_socket(ios),
                m_serverAddr(ip::tcp::endpoint(ip::address::from_string(ip), port)),
                m_isConn(false), m_chkThread(nullptr) {
        CreateEventHandler(ios);
    }
    ~Connector() {}
    bool Start() {
        m_eventHandler->GetSocket().async_connect(m_serverAddr, [this] (const asio::error_code& ec) {
            if (ec) {
                HandlerConnectError(ec);
                return;
            }
                cout << "connect ok" << endl;
                m_isConn = true;
                m_eventHandler->HandleAsyncRead();
            //m_eventHandler->HandleAsyncWrite("i am coming", 11);
        });
        this_thread::sleep_for(std::chrono::seconds(1));
        return m_isConn;
    }
    bool IsConnected() const {
        return m_isConn;
    }
    void AsyncSend(char* data, int len) {
        if (!m_isConn) {
            return;
        }
        m_eventHandler->HandleAsyncWrite(data, len);
    }
    void AsyncRecv() {
        if (!m_isConn) {
            return;
        }
        m_eventHandler->HandleAsyncRead();
    }

    void Send(char* data, int len) {
        if (!m_isConn) {
            return;
        }
        m_eventHandler->HandleWrite(data, len);
    }
    void Recv() {
        if (!m_isConn) {
            return;
        }
        m_eventHandler->HandleRead();
    }
private:
    io_service& m_ios;
    ip::tcp::socket m_socket;
    ip::tcp::endpoint m_serverAddr;
    std::shared_ptr<RWHandler> m_eventHandler;
    bool m_isConn;
    std::shared_ptr<std::thread> m_chkThread; // 检测重连的

    void CreateEventHandler(io_service& ios) {
        m_eventHandler = make_shared<RWHandler>(ios);
        m_eventHandler->SetCallBackError([this](int connid) {
            HandlerRWError(connid);
        });
    }
    void HandlerRWError(int connid) {
        m_isConn = false;
        CheckConnect();
    }
    void CheckConnect() {
        if (m_chkThread != nullptr) {
            return;
        }
        m_chkThread = std::make_shared<std::thread>([this] {
            while (true) {
                if (!IsConnected()) {
                    Start();
                }
                this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }
    void HandlerConnectError(const asio::error_code& ec) {
        m_isConn = false;
        cout << ec.message() << endl;
        m_eventHandler->CloseSocket();
        CheckConnect();
    }

};

void TestConnector() {
    io_service ios;
    io_service::work work(ios);
    std::thread th([&ios] { ios.run();});

    Connector connector(ios, "127.0.0.1", 9090);
    connector.Start();

    std::string str;
    if (!connector.IsConnected()) {
        cin >> str;
        return;
    }
    const int len = 512;
    char line[len] = "";
    while (cin >> str) {
        char header[HEAD_LEN] = {};
        int totalLen = str.length() + HEAD_LEN;
        std::sprintf(header, "%d", totalLen);
        memcpy(line, header, HEAD_LEN);
        memcpy(line + HEAD_LEN, str.c_str(), str.length() + 1);
        connector.AsyncSend(line, totalLen);
        //connector.Recv();
    }
}
#endif //MYWEBSERVER_CONNECTOR_H

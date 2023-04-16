#include "Logging.h"
#include <thread>
#include <string>
#include <unistd.h>
#include <vector>
#include <memory>
#include <iostream>
using namespace std;

std::string Logger::m_logFileName = "WebServer.log";


void threadFunc()
{
    for (int i = 0; i < 100; ++i)
    {
        LOG << i;
    }
}

void type_test()
{
    // 13 lines
    cout << "----------type test-----------" << endl;
    ERROR << 0;
    INFO << true;
    DEBUG << 1234567890123;
    LOG << 1.0f;
    TRACE << 3.1415926;
    LOG << (short) 1;
    LOG << (long long) 1;
    LOG << (unsigned int) 1;
    LOG << (unsigned long) 1;
    WARNING<< (long double) 1.6555556;
    LOG << (unsigned long long) 1;
    LOG << 'c';
    LOG << "abcdefg";
    LOG << string("This is a string");
}

void stressing_single_thread()
{
    // 100000 lines
    cout << "----------stressing test single thread-----------" << endl;
    for (int i = 0; i < 100000; ++i)
    {
        LOG << i;
    }
}

void stressing_multi_threads(int threadNum = std::thread::hardware_concurrency())
{
    // threadNum * 100000 lines
    cout << "----------stressing test multi thread-----------" << endl;
    vector<shared_ptr<thread>> vsp;
    for (int i = 0; i < threadNum; ++i)
    {
        //shared_ptr<thread> tmp(new thread(threadFunc, "testFunc"));
        shared_ptr<thread> tmp = std::make_shared<thread>(thread(threadFunc));
        vsp.push_back(tmp);
    }
    for (int i = 0; i < threadNum; ++i)
    {
        vsp[i]->join();
    }
    sleep(3);
}

void other()
{
    // 1 line
    cout << "----------other test-----------" << endl;
    LOG << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
}


int main()
{
    // 共500014行
    type_test();
    sleep(3);
//
//    stressing_single_thread();
//    sleep(3);

//    other();
//    sleep(3);

//    stressing_multi_threads();
//    sleep(3);
    return 0;
}
cmake_minimum_required(VERSION 3.24)
project(MyWebServer)

set(CMAKE_CXX_STANDARD 17)

include_directories("src/thrid_party/asio/")
include_directories("src/core/")
include_directories("src/core_old/")

add_executable(MyWebServer main.cpp src/core/router.hpp)
add_executable(thread_pool_test src/threadpool/thread_pool_test.cpp src/threadpool/thread_pool.hpp src/threadpool/sync_queue.hpp)

add_executable(server src/core_old/server.cpp src/core_old/rw_handler.hpp src/core_old/server.hpp)
add_executable(client src/core_old/client.cpp src/core_old/rw_handler.hpp src/core_old/connector.hpp)
add_executable(client1 src/core_old/client1.cpp src/core_old/rw_handler.hpp src/core_old/connector.hpp)

add_executable(async_server src/core/async_server.cpp src/core/async_server.h src/core/session.hpp)
add_executable(async_client src/core/async_client.cpp src/core/async_client.h src/core/session.hpp src/aop/aop_template.h)

add_executable(old_test_http src/core_old/test_http.cpp src/core_old/http.cpp src/core_old/http.h)

add_executable(test_aop src/aop/test_aop.cpp src/aop/aop_template.h)
add_executable(test_ioc src/ioc/test_ioc.cpp src/ioc/ioc_template.h)

add_executable(test_log src/log/test_log.cpp src/log/LogConfig.h src/log/LogStream.h src/log/Logging.h src/log/AsyncLogging.h src/log/LogFile.h
        src/log/Semaphore.h src/log/FileUtil.h)

add_executable(test_timer src/timer/test_InvokeTimer.cpp src/timer/InvokeTimer.h)

if(MINGW)
    target_link_libraries(server ws2_32 mswsock)
    target_link_libraries(client ws2_32 mswsock)
    target_link_libraries(client1 ws2_32 mswsock)

    target_link_libraries(async_server ws2_32 mswsock)
    target_link_libraries(async_client ws2_32 mswsock)

    target_link_libraries(test_timer ws2_32 mswsock)
endif()
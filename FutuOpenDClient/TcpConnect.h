#pragma once

#include <uv.h>
#include <vector>
#include "ITcpHandler.h"
#include "Buffer.h"

namespace ftq {

class TcpConnect
{
public:
    TcpConnect();
    ~TcpConnect();
    bool init(uv_loop_t *pUvLoop, ITcpHandler *pHandler);
    void close();
    bool connect(const char *pHost, int nPort);
    bool send(const char *pData, int nLen);
public:
    static void after_connect(uv_connect_t *pReq, int nStatus);
    static void after_close(uv_handle_t *pHandle);
    static void after_read(uv_stream_t* stream,
                           ssize_t nread,
                           const uv_buf_t* buf);
    static void after_write(uv_write_t* req, int status);
    static void on_alloc_buf(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

    char* get_host();
    int get_port();
private:
    uv_loop_t *uv_loop_ {nullptr};
    uv_tcp_t uv_tcp_ {};
    uv_connect_t uv_connect_ {};
    uv_write_t uv_write_ {};
    uv_buf_t uv_read_buf_ {};
    uv_buf_t uv_write_buf_ {};
    std::vector<char> write_stores_[2];
    int cur_using_write_store_idx_ {-1};
    Buffer read_store_ {100 * 1024 * 1024};
    ITcpHandler *handler_ {nullptr};

    char host_[50];
    int port_;
};

}


#ifndef TcpConnect_hpp
#define TcpConnect_hpp

#include <stdio.h>
#include <uv.h>
#include <vector>

namespace ftq {

class Buffer {
public:
    Buffer(int nBufLen);
    ~Buffer();
    int get_total_len()
    {
        return buf_len_;
    }
    char *get_data()
    {
        return buffer_;
    }
    int get_data_len()
    {
        return data_len_;
    }
    void set_data_len(int nLen);
    int get_remain_len();
    char *get_write_start()
    {
        return buffer_ + data_len_;
    }
    bool resize(int nNewLen);
    void remove_front(int nLen);
private:
    int data_len_ {0};
    int buf_len_ {0};
    char *buffer_ {nullptr};
};

class TcpConnect;

class ITcpHandler {
public:
    virtual void on_connect(TcpConnect *conn) = 0;
    virtual void on_recv(TcpConnect *conn, Buffer *buff) = 0;
    virtual void on_error(TcpConnect *conn, int uv_err) = 0;
    virtual void on_disconnect(TcpConnect *conn) = 0;
};

class TcpConnect {
public:
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
private:
    uv_loop_t *uv_loop_ {nullptr};
    uv_tcp_t uv_tcp_ {};
    uv_connect_t uv_connect_ {};
    uv_write_t uv_write_ {};
    uv_buf_t uv_read_buf_ {};
    uv_buf_t uv_write_buf_ {};
    std::vector<char> write_stores_[2];
    int cur_using_write_store_idx_ {-1};
    Buffer read_store_ {10 * 1024 * 1024};
    ITcpHandler *handler_ {nullptr};
};
}

#endif /* TcpConnect_hpp */

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
        return nBufLen;
    }
    char *get_data()
    {
        return pBuf;
    }
    int get_data_len()
    {
        return nDataLen;
    }
    void set_data_len(int nLen);
    int get_remain_len();
    char *get_write_start()
    {
        return pBuf + nDataLen;
    }
    bool resize(int nNewLen);
    void remove_front(int nLen);
private:
    int nDataLen {0};
    int nBufLen {0};
    char *pBuf {nullptr};
};

class TcpConnect;

class ITcpHandler {
public:
    virtual void on_connect(TcpConnect *pConn) = 0;
    virtual void on_recv(TcpConnect *pConn, Buffer *pBuf) = 0;
    virtual void on_error(TcpConnect *pConn, int nUvErr) = 0;
    virtual void on_disconnect(TcpConnect *pConn) = 0;
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
    uv_loop_t *m_pUvLoop {nullptr};
    uv_tcp_t m_tcp {};
    uv_connect_t m_connect {};
    uv_write_t m_write {};
    uv_buf_t m_readBuf {};
    uv_buf_t m_writeBuf {};
    std::vector<char> m_writeStores[2];
    int m_nCurUsingWriteStoreIdx {-1};
    Buffer m_readStore {10 * 1024 * 1024};
    ITcpHandler *m_pHandler {nullptr};
};
}

#endif /* TcpConnect_hpp */

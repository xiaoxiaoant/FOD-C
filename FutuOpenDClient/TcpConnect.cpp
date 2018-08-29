#include "DebugLog.h"
#include "TcpConnect.h"
#include <iostream>
#include <string>
#include <cassert>
#include <string.h>

using namespace std;

namespace ftq {


TcpConnect::TcpConnect()
{
    memset(host_, 0, sizeof(host_));
    port_ = 0;
}

TcpConnect::~TcpConnect()
{
}

bool TcpConnect::init(uv_loop_t *pUvLoop, ITcpHandler *pHandler)
{
    assert(pUvLoop);
    assert(pHandler);

    uv_loop_ = pUvLoop;
    int ret = uv_tcp_init(pUvLoop, &uv_tcp_);
    if (ret != 0)
    {
        return false;
    }

    uv_tcp_.data = this;
    uv_connect_.data = this;
    uv_write_.data = this;
    handler_ = pHandler;
    return true;
}

void TcpConnect::close()
{
    uv_close((uv_handle_t*)&uv_tcp_, after_close);
}

bool TcpConnect::connect(const char *pHost, int nPort)
{
    sockaddr_in addr_in;

    int ret = uv_ip4_addr(pHost, nPort, &addr_in);
    if (ret != 0)
    {
        return false;
    }

    strncpy(host_, pHost, sizeof(host_));
    port_ = nPort;

    ret = uv_tcp_connect(&uv_connect_, &uv_tcp_, (const struct sockaddr*)&addr_in, after_connect);
    if (ret != 0)
    {
        return false;
    }
    return true;
}

char* TcpConnect::get_host()
{
    return host_;
}

int TcpConnect::get_port()
{
    return port_;
}

void TcpConnect::after_close(uv_handle_t *pHandle)
{
    //todo
}


void TcpConnect::after_connect(uv_connect_t *request, int status)
{
    TcpConnect *self = (TcpConnect*)request->data;

    if (status != 0)
    {
        self->handler_->on_error(self, status);
        return;
    }

    int ret = uv_read_start(request->handle, on_alloc_buf, after_read);
    if (ret != 0)
    {
        self->handler_->on_error(self, status);
        return;
    }

    self->handler_->on_connect(self);
}

void TcpConnect::after_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    TcpConnect *conn_self = (TcpConnect*)stream->data;

    if (nread < 0)
    {
        if (nread == UV_EOF)
        {
            //cout << "remote closed" << endl;
            LOGD("remote closed");
            conn_self->handler_->on_disconnect(conn_self);
        }
        else if (nread == UV_ECONNRESET)
        {
            //cout << "conn reset" << endl;
            LOGD("conn reset");
            conn_self->handler_->on_disconnect(conn_self);
        }
        else {
            //cout << "error: " << uv_strerror((int)nread) << endl;
            LOGD("error: %s", uv_strerror((int)nread));
            conn_self->handler_->on_error(conn_self, (int)nread);
        }
        return;
    }

    conn_self->read_store_.set_data_len((int)nread + conn_self->read_store_.get_data_len());
    conn_self->handler_->on_recv(conn_self, &conn_self->read_store_);
}

void TcpConnect::after_write(uv_write_t *req, int status)
{
    int nErr = status;
    TcpConnect *conn_self = (TcpConnect*)req->data;
    conn_self->write_stores_[conn_self->cur_using_write_store_idx_].clear();
    int other_write_storeIdx = conn_self->cur_using_write_store_idx_ == 0 ? 1 : 0;
    conn_self->cur_using_write_store_idx_ = -1;

    if (status == 0)
    {
        std::vector<char> *pOtherWriteStore = &conn_self->write_stores_[other_write_storeIdx];
        if (pOtherWriteStore->size() > 0)
        {
            conn_self->cur_using_write_store_idx_ = other_write_storeIdx;
            conn_self->uv_write_buf_ = uv_buf_init(pOtherWriteStore->data(), (int)pOtherWriteStore->size());
            int ret = uv_write(&conn_self->uv_write_, (uv_stream_t *)&conn_self->uv_tcp_, &conn_self->uv_write_buf_, 1, after_write);
            if (ret != 0)
            {
                nErr = ret;
            }
        }
    }

    if (nErr != 0)
    {
        conn_self->handler_->on_error(conn_self, nErr);
    }
}

void TcpConnect::on_alloc_buf(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    TcpConnect *pSelf = (TcpConnect*)handle->data;
    buf->base = pSelf->read_store_.get_write_start();
    buf->len = pSelf->read_store_.get_remain_len();
}

bool TcpConnect::send(const char *pData, int nLen)
{
    std::vector<char> *pWriteStore = nullptr;
    if (cur_using_write_store_idx_ < 0)
    {
        pWriteStore = &write_stores_[0];
        write_stores_[0].insert(write_stores_[0].end(), pData, pData + nLen);
        uv_write_buf_ = uv_buf_init(write_stores_[0].data(), (int)write_stores_[0].size());
        cur_using_write_store_idx_ = 0;

        int ret = uv_write(&uv_write_, (uv_stream_t *)&uv_tcp_, &uv_write_buf_, 1, after_write);
        if (ret != 0)
        {
            return false;
        }
    }
    else {
        int nWriteStoreIdx = cur_using_write_store_idx_ == 0 ? 1 : 0;
        pWriteStore = &write_stores_[nWriteStoreIdx];
        pWriteStore->insert(pWriteStore->end(), pData, pData + nLen);
    }

    return true;
}
}

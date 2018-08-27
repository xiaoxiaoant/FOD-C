#include "TcpConnect.h"
#include <iostream>
#include <string>
#include <cassert>
#include <string.h>

using namespace std;

namespace ftq {

Buffer::Buffer(int nBufLen)
{
    assert(nBufLen > 0);

    this->buf_len_ = nBufLen;
    this->buffer_ = (char *)malloc(sizeof(char) * nBufLen);
}

Buffer::~Buffer()
{
    free(buffer_);
    buffer_ = nullptr;
    data_len_ = 0;
    buf_len_ = 0;
}

int Buffer::get_remain_len()
{
    return buf_len_ - data_len_;
}

bool Buffer::resize(int nNewLen)
{
    assert(nNewLen > 0);

    if (nNewLen == buf_len_)
    {
        return true;
    }

    char *pNewBuf = (char*)realloc(buffer_, nNewLen);
    if (!pNewBuf)
    {
        return false;
    }
    buffer_ = pNewBuf;
    buf_len_ = nNewLen;
    return true;
}

void Buffer::set_data_len(int nLen)
{
    assert(nLen <= buf_len_);
    data_len_ = nLen;
}

void Buffer::remove_front(int nLen)
{
    assert(nLen <= data_len_);

    int nRemainDataLen = data_len_ - nLen;
    if (nRemainDataLen > 0)
    {
        memmove(buffer_, buffer_ + nLen, nRemainDataLen);
    }
    data_len_ = nRemainDataLen;
}

bool TcpConnect::init(uv_loop_t *pUvLoop, ITcpHandler *pHandler)
{
    assert(pUvLoop);
    assert(pHandler);

    uv_loop_ = pUvLoop;
    int nRet = uv_tcp_init(pUvLoop, &uv_tcp_);
    if (nRet != 0)
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
    struct sockaddr_in addr_in;
    int nRet = uv_ip4_addr(pHost, nPort, &addr_in);
    if (nRet != 0)
    {
        return false;
    }

    nRet = uv_tcp_connect(&uv_connect_, &uv_tcp_, (const struct sockaddr*)&addr_in, after_connect);
    if (nRet != 0)
    {
        return false;
    }
    return true;
}

void TcpConnect::after_close(uv_handle_t *pHandle)
{
    //todo
}


void TcpConnect::after_connect(uv_connect_t *pReq, int nStatus)
{
    TcpConnect *pSelf = (TcpConnect*)pReq->data;

    if (nStatus != 0)
    {
        pSelf->handler_->on_error(pSelf, nStatus);
        return;
    }

    int nRet = uv_read_start(pReq->handle, on_alloc_buf, after_read);
    if (nRet != 0)
    {
        pSelf->handler_->on_error(pSelf, nStatus);
        return;
    }

    pSelf->handler_->on_connect(pSelf);
}

void TcpConnect::after_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    TcpConnect *pSelf = (TcpConnect*)stream->data;

    if (nread < 0)
    {
        if (nread == UV_EOF)
        {
            cout << "remote closed" << endl;
            pSelf->handler_->on_disconnect(pSelf);
        }
        else if (nread == UV_ECONNRESET)
        {
            cout << "conn reset" << endl;
            pSelf->handler_->on_disconnect(pSelf);
        }
        else {
            cout << "error: " << uv_strerror((int)nread) << endl;
            pSelf->handler_->on_error(pSelf, (int)nread);
        }
        return;
    }

    pSelf->read_store_.set_data_len((int)nread + pSelf->read_store_.get_data_len());
    pSelf->handler_->on_recv(pSelf, &pSelf->read_store_);
}

void TcpConnect::after_write(uv_write_t *req, int status)
{
    int nErr = status;
    TcpConnect *pSelf = (TcpConnect*)req->data;
    pSelf->write_stores_[pSelf->cur_using_write_store_idx_].clear();
    int nOtherWriteStoreIdx = pSelf->cur_using_write_store_idx_ == 0 ? 1 : 0;
    pSelf->cur_using_write_store_idx_ = -1;

    if (status == 0)
    {
        std::vector<char> *pOtherWriteStore = &pSelf->write_stores_[nOtherWriteStoreIdx];
        if (pOtherWriteStore->size() > 0)
        {
            pSelf->cur_using_write_store_idx_ = nOtherWriteStoreIdx;
            pSelf->uv_write_buf_ = uv_buf_init(pOtherWriteStore->data(), (int)pOtherWriteStore->size());
            int nRet = uv_write(&pSelf->uv_write_, (uv_stream_t *)&pSelf->uv_tcp_, &pSelf->uv_write_buf_, 1, after_write);
            if (nRet != 0)
            {
                nErr = nRet;
            }
        }
    }

    if (nErr != 0)
    {
        pSelf->handler_->on_error(pSelf, nErr);
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

        int nRet = uv_write(&uv_write_, (uv_stream_t *)&uv_tcp_, &uv_write_buf_, 1, after_write);
        if (nRet != 0)
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

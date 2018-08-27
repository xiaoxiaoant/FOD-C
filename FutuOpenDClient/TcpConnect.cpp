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

    this->nBufLen = nBufLen;
    this->pBuf = (char *)malloc(sizeof(char) * nBufLen);
}

Buffer::~Buffer()
{
    free(pBuf);
    pBuf = nullptr;
    nDataLen = 0;
    nBufLen = 0;
}

int Buffer::get_remain_len()
{
    return nBufLen - nDataLen;
}

bool Buffer::resize(int nNewLen)
{
    assert(nNewLen > 0);

    if (nNewLen == nBufLen)
    {
        return true;
    }

    char *pNewBuf = (char*)realloc(pBuf, nNewLen);
    if (!pNewBuf)
    {
        return false;
    }
    pBuf = pNewBuf;
    nBufLen = nNewLen;
    return true;
}

void Buffer::set_data_len(int nLen)
{
    assert(nLen <= nBufLen);
    nDataLen = nLen;
}

void Buffer::remove_front(int nLen)
{
    assert(nLen <= nDataLen);

    int nRemainDataLen = nDataLen - nLen;
    if (nRemainDataLen > 0)
    {
        memmove(pBuf, pBuf + nLen, nRemainDataLen);
    }
    nDataLen = nRemainDataLen;
}

bool TcpConnect::init(uv_loop_t *pUvLoop, ITcpHandler *pHandler)
{
    assert(pUvLoop);
    assert(pHandler);

    m_pUvLoop = pUvLoop;
    int nRet = uv_tcp_init(pUvLoop, &m_tcp);
    if (nRet != 0)
    {
        return false;
    }

    m_tcp.data = this;
    m_connect.data = this;
    m_write.data = this;
    m_pHandler = pHandler;
    return true;
}

void TcpConnect::close()
{
    uv_close((uv_handle_t*)&m_tcp, after_close);
}

bool TcpConnect::connect(const char *pHost, int nPort)
{
    struct sockaddr_in addr_in;
    int nRet = uv_ip4_addr(pHost, nPort, &addr_in);
    if (nRet != 0)
    {
        return false;
    }

    nRet = uv_tcp_connect(&m_connect, &m_tcp, (const struct sockaddr*)&addr_in, after_connect);
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
        pSelf->m_pHandler->on_error(pSelf, nStatus);
        return;
    }

    int nRet = uv_read_start(pReq->handle, on_alloc_buf, after_read);
    if (nRet != 0)
    {
        pSelf->m_pHandler->on_error(pSelf, nStatus);
        return;
    }

    pSelf->m_pHandler->on_connect(pSelf);
}

void TcpConnect::after_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    TcpConnect *pSelf = (TcpConnect*)stream->data;

    if (nread < 0)
    {
        if (nread == UV_EOF)
        {
            cout << "remote closed" << endl;
            pSelf->m_pHandler->on_disconnect(pSelf);
        }
        else if (nread == UV_ECONNRESET)
        {
            cout << "conn reset" << endl;
            pSelf->m_pHandler->on_disconnect(pSelf);
        }
        else {
            cout << "error: " << uv_strerror((int)nread) << endl;
            pSelf->m_pHandler->on_error(pSelf, (int)nread);
        }
        return;
    }

    pSelf->m_readStore.set_data_len((int)nread + pSelf->m_readStore.get_data_len());
    pSelf->m_pHandler->on_recv(pSelf, &pSelf->m_readStore);
}

void TcpConnect::after_write(uv_write_t *req, int status)
{
    int nErr = status;
    TcpConnect *pSelf = (TcpConnect*)req->data;
    pSelf->m_writeStores[pSelf->m_nCurUsingWriteStoreIdx].clear();
    int nOtherWriteStoreIdx = pSelf->m_nCurUsingWriteStoreIdx == 0 ? 1 : 0;
    pSelf->m_nCurUsingWriteStoreIdx = -1;

    if (status == 0)
    {
        std::vector<char> *pOtherWriteStore = &pSelf->m_writeStores[nOtherWriteStoreIdx];
        if (pOtherWriteStore->size() > 0)
        {
            pSelf->m_nCurUsingWriteStoreIdx = nOtherWriteStoreIdx;
            pSelf->m_writeBuf = uv_buf_init(pOtherWriteStore->data(), (int)pOtherWriteStore->size());
            int nRet = uv_write(&pSelf->m_write, (uv_stream_t *)&pSelf->m_tcp, &pSelf->m_writeBuf, 1, after_write);
            if (nRet != 0)
            {
                nErr = nRet;
            }
        }
    }

    if (nErr != 0)
    {
        pSelf->m_pHandler->on_error(pSelf, nErr);
    }
}

void TcpConnect::on_alloc_buf(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    TcpConnect *pSelf = (TcpConnect*)handle->data;
    buf->base = pSelf->m_readStore.get_write_start();
    buf->len = pSelf->m_readStore.get_remain_len();
}

bool TcpConnect::send(const char *pData, int nLen)
{
    std::vector<char> *pWriteStore = nullptr;
    if (m_nCurUsingWriteStoreIdx < 0)
    {
        pWriteStore = &m_writeStores[0];
        m_writeStores[0].insert(m_writeStores[0].end(), pData, pData + nLen);
        m_writeBuf = uv_buf_init(m_writeStores[0].data(), (int)m_writeStores[0].size());
        m_nCurUsingWriteStoreIdx = 0;

        int nRet = uv_write(&m_write, (uv_stream_t *)&m_tcp, &m_writeBuf, 1, after_write);
        if (nRet != 0)
        {
            return false;
        }
    }
    else {
        int nWriteStoreIdx = m_nCurUsingWriteStoreIdx == 0 ? 1 : 0;
        pWriteStore = &m_writeStores[nWriteStoreIdx];
        pWriteStore->insert(pWriteStore->end(), pData, pData + nLen);
    }

    return true;
}
}

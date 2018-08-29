#include "DebugLog.h"
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

}

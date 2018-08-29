#pragma once

#include "Buffer.h"
#include "TcpConnect.h"

namespace ftq {

class TcpConnect;

class ITcpHandler
{
public:
    virtual void on_connect(TcpConnect *conn) = 0;
    virtual void on_recv(TcpConnect *conn, Buffer *buff) = 0;
    virtual void on_error(TcpConnect *conn, int uv_err) = 0;
    virtual void on_disconnect(TcpConnect *conn) = 0;
};

}

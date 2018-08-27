#include <iostream>
#include "NetCenter.h"
#include "QuoteHandler.h"

using namespace ftq;

int main(int argc, const char * argv[])
{
    NetCenter::instance()->init(uv_default_loop());

    IProtoHandler *quote_handler = new QuoteHandler();
    NetCenter::instance()->set_proto_handler(quote_handler);

    NetCenter::instance()->connect("127.0.0.1", 11111);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}

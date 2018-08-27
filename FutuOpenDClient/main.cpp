#include <iostream>
#include <assert.h>
#include "NetCenter.h"
#include "QuoteHandler.h"

using namespace ftq;

int main(int argc, const char * argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s ip port\n", argv[0]);
    }
    
    assert(argc == 3);

    NetCenter::instance()->init(uv_default_loop());

    IProtoHandler *quote_handler = new QuoteHandler();
    NetCenter::instance()->set_proto_handler(quote_handler);

    NetCenter::instance()->connect(argv[1], atoi(argv[2]));
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}

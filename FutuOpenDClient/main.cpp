#include <iostream>
#include "NetCenter.h"
#include "QuoteHandler.h"

using namespace ftq;

int main(int argc, const char * argv[])
{
    // insert code here...
    QuoteHandler *pQuoteHandler = new QuoteHandler();
    NetCenter::default()->init(uv_default_loop());
    NetCenter::default()->set_proto_handler(pQuoteHandler);
    NetCenter::default()->connect("127.0.0.1", 11111);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    return 0;
}

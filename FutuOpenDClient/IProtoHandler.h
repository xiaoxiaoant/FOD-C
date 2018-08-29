#pragma once

//#include "Common.h"
#include "APIProtoHeader.h"

namespace ftq
{

class IProtoHandler
{
public:
    virtual std::string on_request_init_connect(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;

    virtual void my_request() = 0;

    // 处理OpenD发过来的数据包
    virtual void on_request_keep_alive(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_get_global_state(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_qot_sub(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_reg_qot_push(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_update_ticker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_update_broker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_update_order_book(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_update_stock_basic(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
};

}



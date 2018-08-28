#pragma once

#include "NetCenter.h"

namespace ftq
{
class QuoteHandler: public IProtoHandler
{

public:
    virtual std::string on_request_init_connect(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;

    virtual void my_request() override;

    virtual void on_request_keep_alive(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;
    virtual void on_request_get_global_state(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;
    virtual void on_request_qot_sub(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;
    virtual void on_request_reg_qot_push(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;
    virtual void on_request_update_ticker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;
    virtual void on_request_update_broker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;
    virtual void on_request_update_order_book(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;
    virtual void on_request_update_stock_basic(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) override;

private:
    i32_t keep_alive_interval_ {5};
    u64_t user_id_ {0};
    std::string conn_aes_key;
};
}

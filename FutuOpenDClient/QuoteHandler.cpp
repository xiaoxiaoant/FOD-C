#include "QuoteHandler.h"
#include <iostream>
#include <vector>
#include "google/protobuf/message.h"
#include "NetCenter.h"
#include "pb/pb_header.h"

using namespace std;

namespace ftq
{
bool ParsePb(google::protobuf::Message *pb_obj, u32_t proto_id, const i8_t *data, i32_t len)
{
    if (!pb_obj->ParseFromArray(data, len))
    {
        //cerr << "Protobuf parse error: ProtoID=" << nProtoID << endl;
        DEBUGLOG("Protobuf parse error: ProtoID = %d", proto_id);
        return false;
    }
    string a = pb_obj->DebugString();
    DEBUGLOG("pb proto_id: %d len: %d %s", proto_id, len, a.c_str());
    return true;
}

std::string QuoteHandler::on_request_init_connect(const APIProtoHeader &header, const i8_t *data, i32_t len)
{
    //cout << "OnRsp_InitConnect: " << endl;
    DEBUGLOG("OnRsp_InitConnect: ");

    InitConnect::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, data, len))
    {
        return std::string();
    }

    conn_aes_key = rsp.s2c().connaeskey();

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    DEBUGLOG("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
    if (rsp.rettype() != 0)
    {
        return std::string();
    }

    keep_alive_interval_ = rsp.s2c().keepaliveinterval();
    user_id_ = rsp.s2c().loginuserid();

    return conn_aes_key;
}

void QuoteHandler::my_request()
{
    //启动心跳定时器
    //NetCenter::instance()->start_keep_alive_timer(keep_alive_interval_ * 4 / 5);
    NetCenter::instance()->start_keep_alive_timer(2);

    //获取市场全局状态
    NetCenter::instance()->req_get_global_state(user_id_);

    //subscribe stock
    Qot_Common::Security stock;

    vector<Qot_Common::Security> stocks;

    stock.set_market(Qot_Common::QotMarket_HK_Security);
    stock.set_code("00700");
    stocks.push_back(stock);

    stock.set_market(Qot_Common::QotMarket_US_Security);
    stock.set_code("GOOG");
    stocks.push_back(stock);

    vector<Qot_Common::SubType> subTypes;
    subTypes.push_back(Qot_Common::SubType_OrderBook);
    subTypes.push_back(Qot_Common::SubType_Broker);

    vector<Qot_Common::RehabType> rehabTypes;
    rehabTypes.push_back(Qot_Common::RehabType_None);

    //订阅00700的逐笔数据
    NetCenter::instance()->req_subscribe(stocks, subTypes, true, true, rehabTypes, true);

    //注册接收逐笔推送
    NetCenter::instance()->req_reg_push(stocks, subTypes, rehabTypes, true, true);
}

void QuoteHandler::on_request_keep_alive(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_KeepAlive: " << endl;
    DEBUGLOG("OnRsp_KeepAlive:");
}

void QuoteHandler::on_request_get_global_state(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_GetGlobalState: " << endl;
    DEBUGLOG("OnRsp_Qot_Sub:");

    GetGlobalState::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << "; ServerVer=" << rsp.s2c().serverver() << "; " << endl;
    DEBUGLOG("Ret=%d; Msg=%s; ServerVer=%d;", rsp.rettype(), rsp.retmsg().c_str(), rsp.s2c().serverver());
}

void QuoteHandler::on_request_qot_sub(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_Sub: " << endl;
    DEBUGLOG("OnRsp_Qot_Sub:");

    Qot_Sub::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    DEBUGLOG("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
}

void QuoteHandler::on_request_reg_qot_push(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_RegQotPush:" << endl;
    DEBUGLOG("OnRsp_Qot_RegQotPush:");

    Qot_RegQotPush::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    DEBUGLOG("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
}

void QuoteHandler::on_request_update_ticker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_UpdateTicker:" << endl;
    DEBUGLOG("OnRsp_Qot_UpdateTicker:");

    Qot_UpdateTicker::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    DEBUGLOG("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
    if (rsp.rettype() != 0)
    {
        return;
    }

    for (int i = 0; i < rsp.s2c().tickerlist_size(); ++i)
    {
        const Qot_Common::Ticker &data = rsp.s2c().tickerlist(i);
        //cout << "Ticker: Code=" << rsp.s2c().security().code() << "; Time=" << data.time() << "; Price=" << data.price() << ";" << endl;
        DEBUGLOG("Ticker: Code=%s; Time=%s; Price=%lf;", rsp.s2c().security().code().c_str(), data.time().c_str(), data.price());
    }
}

void QuoteHandler::on_request_update_broker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_UpdateBroker:" << endl;
    DEBUGLOG("OnRsp_Qot_UpdateBroker:");

    Qot_UpdateBroker::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    DEBUGLOG("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
    if (rsp.rettype() != 0)
    {
        return;
    }

    for (int i = 0; i < rsp.s2c().brokerasklist_size(); ++i)
    {
        const Qot_Common::Broker &data = rsp.s2c().brokerasklist(i);
        //cout << "Broker: ID=" << data.id() << ";" << endl;
        DEBUGLOG("Broker: ID=%lld;", data.id());
    }
}

void QuoteHandler::on_request_update_order_book(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_UpdateOrderBook:" << endl;
    DEBUGLOG("OnRsp_Qot_UpdateOrderBook:");

    Qot_UpdateOrderBook::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    DEBUGLOG("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
    if (rsp.rettype() != 0)
    {
        return;
    }

    for (int i = 0; i < rsp.s2c().orderbookasklist_size(); ++i)
    {
        const Qot_Common::OrderBook &data = rsp.s2c().orderbookasklist(i);
        cout << "Price: " << data.price() << endl;
        DEBUGLOG("Price: %lf", data.price());
    }
}

}

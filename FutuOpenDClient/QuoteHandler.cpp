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
        LOGE("Protobuf parse error: ProtoID = %d", proto_id);
        return false;
    }
    string a = pb_obj->DebugString();
    LOGD("pb proto_id: %d len: %d %s", proto_id, len, a.c_str());
    return true;
}

std::string QuoteHandler::on_request_init_connect(const APIProtoHeader &header, const i8_t *data, i32_t len)
{
    //cout << "OnRsp_InitConnect: " << endl;
    LOGD("OnRsp_InitConnect: ");

    InitConnect::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, data, len))
    {
        return std::string();
    }

    conn_aes_key = rsp.s2c().connaeskey();

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    LOGD("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
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
    NetCenter::instance()->start_keep_alive_timer(keep_alive_interval_ * 4 / 5);

    //获取市场全局状态
    NetCenter::instance()->req_get_global_state(user_id_);

    //subscribe stock
    Qot_Common::Security stock;

    vector<Qot_Common::Security> stocks;

    FILE *fp = fopen("stock_list", "r");
    if(fp)
    {
        int type = 0;
        char code[50] = {0};
        while(fscanf(fp, "%d %s\n", &type, code) == 2)
        {
            stock.set_market(type);
            stock.set_code(code);
            stocks.push_back(stock);
            LOGD("reading %d %s\n", type, code);
        }
        fclose(fp);
    }

    vector<Qot_Common::SubType> subTypes;
    subTypes.push_back(Qot_Common::SubType_OrderBook);
    subTypes.push_back(Qot_Common::SubType_Broker);
    subTypes.push_back(Qot_Common::SubType_Ticker);
    subTypes.push_back(Qot_Common::SubType_Basic);

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
    LOGD("OnRsp_KeepAlive:");
}

void QuoteHandler::on_request_get_global_state(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_GetGlobalState: " << endl;
    LOGD("OnRsp_Qot_Sub:");

    GetGlobalState::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << "; ServerVer=" << rsp.s2c().serverver() << "; " << endl;
    LOGD("Ret=%d; Msg=%s; ServerVer=%d;", rsp.rettype(), rsp.retmsg().c_str(), rsp.s2c().serverver());
}

void QuoteHandler::on_request_qot_sub(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_Sub: " << endl;
    LOGD("OnRsp_Qot_Sub:");

    Qot_Sub::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    LOGD("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
}

void QuoteHandler::on_request_reg_qot_push(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_RegQotPush:" << endl;
    LOGD("OnRsp_Qot_RegQotPush:");

    Qot_RegQotPush::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    LOGD("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
}

void QuoteHandler::on_request_update_stock_basic(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_UpdateTicker:" << endl;
    LOGD("OnRsp_Qot_UpdateBasicQot:");

    Qot_UpdateBasicQot::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }
}

void QuoteHandler::on_request_update_ticker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_UpdateTicker:" << endl;
    LOGD("OnRsp_Qot_UpdateTicker:");

    Qot_UpdateTicker::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    LOGD("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
    if (rsp.rettype() != 0)
    {
        return;
    }

    /*
	required string time = 1; //时间字符串
	required int64 sequence = 2; // 唯一标识
	required int32 dir = 3; //TickerDirection, 买卖方向
	required double price = 4; //价格
	required int64 volume = 5; //成交量
	required double turnover = 6; //成交额
	optional double recvTime = 7; //收到推送数据的本地时间戳，用于定位延迟
	optional int32 type = 8; //TickerType, 逐笔类型
	optional int32 typeSign = 9; //逐笔类型符号
    */
    for (int i = 0; i < rsp.s2c().tickerlist_size(); ++i)
    {
        const Qot_Common::Ticker &data = rsp.s2c().tickerlist(i);
        //cout << "Ticker: Code=" << rsp.s2c().security().code() << "; Time=" << data.time() << "; Price=" << data.price() << ";" << endl;
        LOGD("Ticker: C:%s T:%s P:%lf V:%lld D:%d TP:%d S:%d",
             rsp.s2c().security().code().c_str(),
             data.time().c_str(),
             data.price(),
             data.volume(),
             data.dir(),
             data.type(),
             data.typesign());
    }
}

void QuoteHandler::on_request_update_broker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_UpdateBroker:" << endl;
    LOGD("OnRsp_Qot_UpdateBroker:");

    Qot_UpdateBroker::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    LOGD("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
    if (rsp.rettype() != 0)
    {
        return;
    }

    for (int i = 0; i < rsp.s2c().brokerasklist_size(); ++i)
    {
        const Qot_Common::Broker &data_ask = rsp.s2c().brokerasklist(i);
        //cout << "Broker: ID=" << data_ask.id() << ";" << data_bid.id() << endl;
        LOGD("Broker: bid ID=%lld", data_ask.id());
    }
    for (int i = 0; i < rsp.s2c().brokerbidlist_size(); ++i)
    {
        const Qot_Common::Broker &data_bid = rsp.s2c().brokerbidlist(i);
        LOGD("Broker: bid ID=%lld", data_bid.id());
    }
}

void QuoteHandler::on_request_update_order_book(const APIProtoHeader &header, const i8_t *pData, i32_t nLen)
{
    //cout << "OnRsp_Qot_UpdateOrderBook:" << endl;
    LOGD("OnRsp_Qot_UpdateOrderBook:");

    Qot_UpdateOrderBook::Response rsp;
    if (!ParsePb(&rsp, header.proto_id_, pData, nLen))
    {
        return;
    }

    //cout << "Ret=" << rsp.rettype() << "; Msg=" << rsp.retmsg() << endl;
    LOGD("Ret=%d; Msg=%s", rsp.rettype(), rsp.retmsg().c_str());
    if (rsp.rettype() != 0)
    {
        return;
    }

    float order[20] = {0};

    //cout << rsp.s2c().orderbookasklist_size() << " " << rsp.s2c().orderbookbidlist_size() << endl;

    for (int i = 0; i < rsp.s2c().orderbookasklist_size(); ++i)
    {
        const Qot_Common::OrderBook &data_ask = rsp.s2c().orderbookasklist(i);
        const Qot_Common::OrderBook &data_bid = rsp.s2c().orderbookbidlist(i);
        order[10 + i] = data_ask.price();
        order[ 9 - i] = data_bid.price();
        //LOGD("Price: %lf", data.price());
    }
    if(0)printf("%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %d-%s %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
        order[0],
        order[1],
        order[2],
        order[3],
        order[4],
        order[5],
        order[6],
        order[7],
        order[8],
        order[9],
        rsp.s2c().security().market(),
        rsp.s2c().security().code().c_str(),
        order[10],
        order[11],
        order[12],
        order[13],
        order[14],
        order[15],
        order[16],
        order[17],
        order[18],
        order[19]);
}

}

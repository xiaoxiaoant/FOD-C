#include "NetCenter.h"
#include "SHA1.h"
#include <iostream>
#include <time.h>
#include "pb/pb_header.h"

using namespace std;

namespace ftq
{
NetCenter *NetCenter::instance_ = new NetCenter();

NetCenter::~NetCenter()
{
    if (quote_conn_)
    {
        quote_conn_->close();
        quote_conn_ = nullptr;
    }
}


bool NetCenter::init(uv_loop_t *uv_loop)
{
    if (!uv_loop)
    {
        return false;
    }

    loop_ = uv_loop;
    quote_conn_ = new TcpConnect();
    quote_conn_->init(uv_loop, this);
    return true;
}


void NetCenter::connect(const char *ip, i32_t port)
{
    quote_conn_->connect(ip, port);
}


NetCenter * NetCenter::instance()
{
    return instance_;
}

void NetCenter::set_proto_handler(IProtoHandler *handler)
{
    proto_handler_ = handler;
}

void NetCenter::on_connect(TcpConnect *conn)
{
    //连接成功后需要调用InitConnect
    req_init_connect(100, "demo", true);
}

void NetCenter::on_recv(TcpConnect *conn, Buffer *buffer)
{
    for (;;)
    {
        APIProtoHeader header;
        const char *pData = buffer->get_data();
        i32_t nLen = buffer->get_data_len();
        if (nLen < (i32_t)sizeof(header))
        {
            return;
        }

        header = *(APIProtoHeader*)pData;
        if (nLen < (i32_t)sizeof(header) + header.body_len_)
        {
            return;
        }

        u8_t sha1[20] = {0};
        const char *pBody = pData + sizeof(header);
        SHA1((char*)sha1, pBody, header.body_len_);
        if (memcmp(sha1, header.body_sha1_, 20) != 0)
        {
            //error
            //cerr << "sha check fail" << endl;
            DEBUGLOG("sha check fail");
            buffer->remove_front((i32_t)sizeof(header) + header.body_len_);
            continue;
        }

        handle_packet(header, (const i8_t*)pBody, header.body_len_);

        buffer->remove_front((i32_t)sizeof(header) + header.body_len_);
    }
}

void NetCenter::on_error(TcpConnect *conn, int uv_err)
{
    //cerr << "Net error: " << uv_err << " " << uv_strerror(uv_err) << endl;
    DEBUGLOG("Net error: %d %s", uv_err, uv_strerror(uv_err));
}

void NetCenter::on_disconnect(TcpConnect *conn)
{
    //cerr << "Disconnected" << endl;
    DEBUGLOG("Disconnected");
}


void NetCenter::on_keep_alive_timer(uv_timer_t* handle)
{
    NetCenter *pSelf = (NetCenter*)handle->data;
    pSelf->req_keep_alive();
}

void NetCenter::start_keep_alive_timer(i32_t interval)
{
    keep_alive_timer_.data = this;
    uv_timer_init(loop_, &keep_alive_timer_);
    uv_timer_start(&keep_alive_timer_, on_keep_alive_timer, interval * 1000, interval * 1000);
}


u32_t NetCenter::req_get_global_state(u64_t user_id)
{
    GetGlobalState::C2S *c2s = new GetGlobalState::C2S();
    c2s->set_userid(user_id);
    GetGlobalState::Request req;
    req.set_allocated_c2s(c2s);
    return net_send(API_ProtoID_GlobalState, req);
}

u32_t NetCenter::req_init_connect(i32_t client_ver, const char *client_id, bool recv_notify)
{
    InitConnect::C2S *c2s = new InitConnect::C2S();
    c2s->set_clientver(client_ver);
    c2s->set_clientid(client_id);
    c2s->set_recvnotify(recv_notify);
    InitConnect::Request req;
    req.set_allocated_c2s(c2s);
    return net_send(API_ProtoID_InitConnect, req);
}


u32_t NetCenter::req_subscribe(const std::vector<Qot_Common::Security> &stocks,
                               const std::vector<Qot_Common::SubType> &sub_types,
                               bool is_sub,
                               bool reg_push,
                               const std::vector<Qot_Common::RehabType> &rehab_types,
                               bool first_push)
{
    Qot_Sub::C2S *c2s = new Qot_Sub::C2S();

    for (auto &stock : stocks)
    {
        Qot_Common::Security *security = c2s->add_securitylist();
        *security = stock;
    }

    for (auto &subType : sub_types)
    {
        c2s->add_subtypelist(subType);
    }
    c2s->set_issuborunsub(is_sub);
    c2s->set_isregorunregpush(reg_push);
    c2s->set_isfirstpush(first_push);

    Qot_Sub::Request req;
    req.set_allocated_c2s(c2s);
    return net_send(API_ProtoID_Qot_Sub, req);
}

u32_t NetCenter::req_keep_alive()
{
    KeepAlive::C2S *c2s = new KeepAlive::C2S();
    c2s->set_time(time(NULL));
    KeepAlive::Request req;
    req.set_allocated_c2s(c2s);
    return net_send(API_ProtoID_KeepAlive, req);
}

u32_t NetCenter::req_reg_push(const std::vector<Qot_Common::Security> &stocks,
                              const std::vector<Qot_Common::SubType> &sub_types,
                              const std::vector<Qot_Common::RehabType> &rehab_types,
                              bool is_reg_push,
                              bool first_push)
{
    Qot_RegQotPush::C2S *c2s = new Qot_RegQotPush::C2S();

    for (auto &stock : stocks)
    {
        Qot_Common::Security *security = c2s->add_securitylist();
        *security = stock;
    }

    for (auto &subType : sub_types)
    {
        c2s->add_subtypelist(subType);
    }

    for (auto &rehab_type : rehab_types)
    {
        c2s->add_rehabtypelist(rehab_type);
    }

    c2s->set_isregorunreg(is_reg_push);
    c2s->set_isfirstpush(first_push);

    Qot_RegQotPush::Request req;
    req.set_allocated_c2s(c2s);
    return net_send(API_ProtoID_Qot_RegQotPush, req);
}

u32_t NetCenter::net_send(u32_t proto_id, const google::protobuf::Message &pb_obj)
{
    u32_t packet_no = 0;

    i32_t nSize = (i32_t)pb_obj.ByteSize();
    string body_data;
    body_data.resize(nSize);
    if (!pb_obj.SerializeToString(&body_data))
    {
        return packet_no;
    }

    APIProtoHeader header;
    header.init();
    header.proto_id_ = proto_id;
    header.proto_fmt_type_ = 0; //pb
    header.proto_ver_ = 0;
    header.serial_no_ = next_packet_no_++;
    header.body_len_ = nSize;
    SHA1((char*)header.body_sha1_, body_data.c_str(), nSize);
    const char *p_header = (const char *)&header;

    string packetData;
    packetData.append(p_header, p_header + sizeof(header));
    packetData.append(body_data.begin(), body_data.end());
    if (quote_conn_->send(packetData.data(), (int)packetData.size()))
    {
        packet_no = header.serial_no_;
    }
    return packet_no;
}

void NetCenter::handle_packet(const APIProtoHeader &header, const i8_t *data, i32_t len)
{
    switch (header.proto_id_)
    {
        case API_ProtoID_InitConnect:
            proto_handler_->on_request_init_connect(header, data, len);
            break;
        case API_ProtoID_GlobalState:
            proto_handler_->on_request_get_global_state(header, data, len);
            break;
        case API_ProtoID_Qot_Sub:
            proto_handler_->on_request_qot_sub(header, data, len);
            break;
        case API_ProtoID_Qot_UpdateTicker:
            proto_handler_->on_request_update_ticker(header, data, len);
            break;
        case API_ProtoID_Qot_RegQotPush:
            proto_handler_->on_request_reg_qot_push(header, data, len);
            break;
        case API_ProtoID_KeepAlive:
            proto_handler_->on_request_keep_alive(header, data, len);
            break;
        case API_ProtoID_Qot_UpdateBroker:
            proto_handler_->on_request_update_broker(header, data, len);
            break;
        case API_ProtoID_Qot_UpdateOrderBook:
            proto_handler_->on_request_update_order_book(header, data, len);
            break;
        default:
            break;
    }
}

}

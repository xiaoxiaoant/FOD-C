#include "NetCenter.h"
#include "SHA1.h"
#include "myrsa.h"
#include "myaes.h"
#include <iostream>
#include <time.h>
#include "pb/pb_header.h"
#include "APIProtoPkg.h"

#include <time.h>
#include <sys/time.h>
//#include <unistd.h>

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
    uv_timer_stop(&keep_alive_timer_);
    //连接成功后需要调用InitConnect
    req_init_connect(100, "ant_demo", true);
}

void NetCenter::on_recv(TcpConnect *conn, Buffer *buffer)
{
    LOGT("len: %d", buffer->get_data_len());
    for (;;)
    {
        if (buffer->get_data_len() == 0)
        {
            break;
        }

        APIProtoPkg pkg;

        API_PKG_ERR ret = API_ERR_NO_ERROR;
        if (conn_aes_key == "")
        {
            ret = pkg.recv_with_rsa(buffer->get_data(), buffer->get_data_len(), "rsa_key.txt");
        }
        else
        {
            ret = pkg.recv_with_aes(buffer->get_data(), buffer->get_data_len(), conn_aes_key);
        }
        
        LOGD("recv ret = %d", ret);
        switch (ret)
        {
            case API_ERR_NO_ERROR:
                handle_packet(*(APIProtoHeader*)pkg.get_head_pointer(), (const i8_t*)pkg.get_body_pointer(), pkg.get_body_len());
                buffer->remove_front(pkg.get_whole_pkg_len());
                break;

            case API_ERR_HEAD_PROTO_ERR:
                buffer->remove_front(1);
                break;

            case API_ERR_BODY_LEN_IS_NOT_ENOUGH:
                // wait the whole body
                return;

            case API_ERR_SHA_ERR:
                buffer->remove_front(pkg.get_whole_pkg_len());
                break;
        }

    }
}

void NetCenter::on_error(TcpConnect *conn, int uv_err)
{
    //cerr << "Net error: " << uv_err << " " << uv_strerror(uv_err) << endl;
    LOGE("Net error: %d %s", uv_err, uv_strerror(uv_err));
}

void NetCenter::on_disconnect(TcpConnect *conn)
{
    //cerr << "Disconnected" << endl;
    LOGD("Disconnected");

    uv_timer_stop(&keep_alive_timer_);
    uv_timer_init(loop_, &keep_alive_timer_);
    //根据正常连接时间来设置心跳
    uv_timer_start(&keep_alive_timer_, on_connect_again, 4 * 1000, 4 * 1000);
}

void NetCenter::on_connect_again(uv_timer_t* handle)
{
    NetCenter *pSelf = (NetCenter*)handle->data;
    pSelf->req_connect_again();
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

void NetCenter::req_connect_again()
{
    char last_host[50];
    strncpy(last_host, quote_conn_->get_host(), sizeof(last_host));

    int last_port = quote_conn_->get_port();
    LOGD("Connect Again! ip: %s port: %d", last_host, last_port);

    conn_aes_key = "";

    delete quote_conn_;
    quote_conn_ = new TcpConnect();
    quote_conn_->init(loop_, this);
    //quote_conn_->set_socket(last_host, last_port);
    quote_conn_->connect(last_host, last_port);
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
    header.init(proto_id);
    header.set_body_len(nSize);

    SHA1((char*)header.get_sha(), body_data.c_str(), nSize);

    unsigned char body[40960] = {0};
    memcpy(body, body_data.c_str(), nSize);


    if (conn_aes_key == "")
    {
        LOGT("aes_key is NULL, use pub.key to encrypt");
        //int a = my_encrypt_pub((char*)body, strlen((char*)body), "pub.key", (char*)body);
        int a = my_encrypt_pub((char*)body, static_cast<const int>(body_data.size()), "pub.key", (char*)body);
        header.set_body_len(a);
    }
    else
    {
        LOGT("use aes_key %s to encrypt", conn_aes_key.c_str());
        int a = my_encrypt_aes(body, nSize, conn_aes_key, body);
        LOGT("ret %d body_len %d", a, header.body_len_);
        header.set_body_len(a);
    }

    string packetData;

    const char *p_header = (const char *)&header;
    packetData.append(p_header, p_header + sizeof(header));

    //packetData.append(body_data.begin(), body_data.end());
    packetData.append(body, body + header.get_body_len());

    if (quote_conn_->send(packetData.data(), (int)packetData.size()))
    {
        packet_no = header.get_serial_no();
    }
    return packet_no;
}

void NetCenter::save_to_file(const APIProtoHeader &header, const i8_t *data, i32_t len)
{
    const int head_size = sizeof(header);

    struct timeval tv;
    struct timezone tz;
    gettimeofday (&tv , &tz);//获取时区保存tz中
    time_t utc_now = tz.tz_minuteswest * 60 + tv.tv_sec;
    //printf("%d %d %d %d %d\n", tz.tz_minuteswest, tz.tz_dsttime, tv.tv_sec, tv.tv_usec, utc_now);


    char strTime[64];
    memset(strTime, 0, sizeof(strTime));
    strftime(strTime, sizeof(strTime), "[%Y-%m-%d %T]", localtime(&utc_now));

    std::string time_fmt(strTime);
    time_fmt = time_fmt.substr(1, 13);
    time_fmt.replace(10, 1, "-");
    std::string fn = "data/quant" + time_fmt + ".data";

#if !((defined _WIN32) || (defined _WIN64))
    static pthread_mutex_t MutexSingle = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&MutexSingle);
#endif

    FILE *fp0 = fopen(fn.c_str(), "aw");
    if (fp0)
    {
        fwrite("VER01", 5, 1, fp0);
        fwrite(&tv, sizeof(tv), 1, fp0);
        fwrite(&tz, sizeof(tz), 1, fp0);
        fwrite(&header, sizeof(header), 1, fp0);
        fwrite(data, len, 1, fp0);

        fclose(fp0);
    }


#if !((defined _WIN32) || (defined _WIN64))
    pthread_mutex_unlock(&MutexSingle);
#endif
    return;
}


void NetCenter::handle_packet(const APIProtoHeader &header, const i8_t *data, i32_t len)
{
    u32_t proto_id = header.get_proto_id();
    LOGD("proto_id: %d", proto_id);

    bool need_save = true;
    switch (proto_id)
    {
        case API_ProtoID_InitConnect:
            conn_aes_key = proto_handler_->on_request_init_connect(header, data, len);
            proto_handler_->my_request();
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
            need_save = false;
            break;
        case API_ProtoID_Qot_UpdateBroker:
            proto_handler_->on_request_update_broker(header, data, len);
            break;
        case API_ProtoID_Qot_UpdateStockBasic:
            proto_handler_->on_request_update_stock_basic(header, data, len);
            break;
        case API_ProtoID_Qot_UpdateOrderBook:
            proto_handler_->on_request_update_order_book(header, data, len);
            break;
        default:
            LOGW("no func for proto_id: %d", proto_id);
            break;
    }

    if(need_save)
    {
        save_to_file(header, data, len);
    }
}

}

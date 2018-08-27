#ifndef NetCenterImpl_hpp
#define NetCenterImpl_hpp

#include "Common.h"
#include <vector>
#include <string>
#include "TcpConnect.h"
#include "pb/pb_header.h"

namespace ftq
{

class IProtoHandler
{
public:
    virtual void on_request_init_connect(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;

    // 处理OpenD发过来的数据包
    virtual void on_request_keep_alive(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_get_global_state(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_qot_sub(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_reg_qot_push(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_update_ticker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_update_broker(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
    virtual void on_request_update_order_book(const APIProtoHeader &header, const i8_t *pData, i32_t nLen) = 0;
};

class NetCenter: public ITcpHandler
{
public:
    virtual ~NetCenter();
    bool init(uv_loop_t *uv_loop);
    void connect(const char *ip, i32_t port);

    static NetCenter *instance();
public:
    void set_proto_handler(IProtoHandler *handler);
public:
    virtual void on_connect(TcpConnect *conn) override;
    virtual void on_recv(TcpConnect *conn, Buffer *buffer) override;
    virtual void on_error(TcpConnect *conn, int uv_err) override;
    virtual void on_disconnect(TcpConnect *conn) override;
public:
    // 心跳定时器回调
    static void on_keep_alive_timer(uv_timer_t* handle);

    // 开始心跳定时器
    void start_keep_alive_timer(i32_t interval);

    // 获取全局状态
    u32_t req_get_global_state(u64_t user_id);

    // 初始化连接，连接上FutuOpenD后首先要调用这个函数
    u32_t req_init_connect(i32_t client_ver, const char *client_id, bool recv_notify);

    // 订阅股票，有些api比如推送，需要先订阅
    u32_t req_subscribe(const std::vector<Qot_Common::Security> &stocks,
                        const std::vector<Qot_Common::SubType> &sub_types,
                        bool is_sub,
                        bool reg_push,
                        const std::vector<Qot_Common::RehabType> &rehab_types,
                        bool first_push
    );

    // 心跳，每隔一定时间需要向OpenD发送心跳包，间隔的时长见InitConnect协议的返回值
    u32_t req_keep_alive();

    // 注册推送，必须先注册推送，然后才能收到推送数据包，比如逐笔、报价
    u32_t req_reg_push(const std::vector<Qot_Common::Security> &stocks,
                       const std::vector<Qot_Common::SubType> &sub_types,
                       const std::vector<Qot_Common::RehabType> &rehab_types,
                       bool reg_push,
                       bool first_push
    );
private:
    u32_t net_send(u32_t proto_id, const google::protobuf::Message &pb_obj);
    void handle_packet(const APIProtoHeader &header, const i8_t *data, i32_t len);
private:
    uv_loop_t *loop_ {nullptr};
    uv_timer_t keep_alive_timer_ {};
    IProtoHandler *proto_handler_ {nullptr};
    TcpConnect *quote_conn_ {nullptr};
    u32_t next_packet_no_ {1};

private:
    static NetCenter *instance_;
};
}


#endif /* NetCenter_hpp */

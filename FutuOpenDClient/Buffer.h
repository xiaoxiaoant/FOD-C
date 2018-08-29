#pragma once

namespace ftq {

class Buffer {
public:
    Buffer(int nBufLen);
    ~Buffer();
    int get_total_len()
    {
        return buf_len_;
    }
    char *get_data()
    {
        return buffer_;
    }
    int get_data_len()
    {
        return data_len_;
    }
    void set_data_len(int nLen);
    int get_remain_len();
    char *get_write_start()
    {
        return buffer_ + data_len_;
    }
    bool resize(int nNewLen);
    void remove_front(int nLen);
private:
    int data_len_ {0};
    int buf_len_ {0};
    char *buffer_ {nullptr};
};

}


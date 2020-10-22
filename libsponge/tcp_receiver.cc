#include "tcp_receiver.hh"

#include <iostream>
// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    string head_str = seg.header().serialize();
    std::cout << seg.header().to_string() << endl;
    uint8_t flags = head_str[13];
    uint32_t raw_seqno = (static_cast<uint8_t>(head_str[4]) << 24) + (static_cast<uint8_t>(head_str[5]) << 16) +
                         (static_cast<uint8_t>(head_str[6]) << 8) + static_cast<uint8_t>(head_str[7]);
    bool issyn = flags & 0b0000'0010;
    bool isfin = flags & 0b0000'0001;
    //得到不同的index
    if (issyn) {
        //判断是否有syn, 这个分支表示有syn标识
        isn = wrap(uint64_t(raw_seqno), WrappingInt32(0));
        // _checkpoint = raw_seqno;
        _ackno = isn + 1;  //设置ackno
        raw_seqno++;
    }

    WrappingInt32 seqno(raw_seqno);
    uint64_t absloute_index = unwrap(seqno, isn, 0);
    uint64_t stream_index = absloute_index - 1;  // 两个index相差1
    std::cout << "absloute_index is " << absloute_index << "stream_index is " << stream_index << endl;
    //为什么这里不直接用if else , 因为想让下面两个模块共用上面3个变量, 不想定义两遍
    //这里表示没有syn.fin标识, 不是握手, 是正常传输
    if (not _ackno.has_value()) {
        //还没握手已经收到序号, 直接返回
        return;
    }
    Buffer payload = seg.payload();
    _reassembler.push_substring(payload.copy(), stream_index, false);
    _ackno = isn + 1 + _reassembler.stream_out().bytes_written() + ((end_flag && !unassembled_bytes())? 1 : 0);

    if (isfin) {
        // judge if fin , this branch suggest fin
        end_flag = true;
        uint64_t end_index = absloute_index + seg.payload().size();
        end_index = end_index == 0 ? 0 : end_index - 1;
        _reassembler.push_substring("", end_index, true);
        _ackno = _ackno.value() + ((end_flag && !unassembled_bytes())? 1 : 0);
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const { return _ackno; }

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }

#include "wrapping_integers.hh"

#include <algorithm>
// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

inline uint64_t get_gap(uint64_t lhs, uint64_t rhs) {
    if (lhs > rhs) {
        return lhs - rhs;
    } else {
        return rhs - lhs;
    }
}
//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    n += static_cast<uint32_t>(isn.raw_value());
    uint64_t tmp_value = static_cast<uint64_t>(UINT32_MAX) + 1;
    while (n >= tmp_value) {
        n -= tmp_value;
    }
    return WrappingInt32{static_cast<uint32_t>(n)};
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    n -= isn;
    uint64_t res = static_cast<int64_t>(n.raw_value());
    uint64_t gap = (1ll << 32);
    while (true) {
        gap = get_gap(res , checkpoint);
        if(gap < (1ll << 32)){                
            break;
        } 
        res += gap & ~((1ll << 32) - 1);
    }
    if(res >= checkpoint || gap <= (1ll << 31)){
        return res;
    } else {
        return res += (1ll << 32);
    }
    return {};
}


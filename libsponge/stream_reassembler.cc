#include "stream_reassembler.hh"

#include <iostream>
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity), _capacity(capacity), _assembled_bytes(0), _unassembled_bytes(0), ranges{} {
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // first , check is eof and set some value , and pop the bytes out of cap
    if (eof) {
        iseof = true;
        eof_seq = index + data.size() - 1;
    }
    string out = "";
    string tmp_data = data;
    if (index + data.size() > real_capacity()) {
        tmp_data = data.substr(0, real_capacity() - index);
    }

    // second , push the bytes into ranges
    if (_assembled_bytes <= index) {  // unassembled
        out = add_range(tmp_data, index);
    } else if (_assembled_bytes <= index + data.size()) {
        out = add_range(tmp_data.substr(_assembled_bytes - index), _assembled_bytes);
    } else {
        // empty , in this section , bytes is assembled.
    }

    // third , insert out string into output
    if (out != "")
        _output.write(out);

    // final , end input and indict eof
    if (iseof && (eof_seq < _assembled_bytes || eof_seq == static_cast<size_t>(0) - static_cast<size_t>(1))) {
        _output.end_input();
    }
}
size_t StreamReassembler::unassembled_bytes() const { return _unassembled_bytes; }

bool StreamReassembler::empty() const { return _output.buffer_empty(); }

std::string StreamReassembler::add_range(const std::string &data, size_t index) {
    // first , check if index + string is overlapping
    std::pair<size_t, std::string> index_data = std::make_pair(index, data);
    if (!ranges.empty()) {
        auto iter = lower_bound(ranges.begin(), ranges.end(), index_data);
        if ((iter == ranges.begin() || (iter != ranges.end() && iter->first == index))) {
            if (iter->first <= index && iter->first + iter->second.size() >= index + data.size()) {
                // overlapping
                return {};
            }
        } else {
            --iter;
            if (iter->first + iter->second.size() >= index + data.size()) {
                // overlapping
                return {};
            }
        }
    }
    // second , add range and pop string (if ness)
    size_t position = 0;
    vector<std::pair<size_t, std::string>> tmp_ranges;
    tmp_ranges.reserve(ranges.size());
    string res = "";
    if (index <= _assembled_bytes) {
        // this branch can assembled string
        res = data.substr(_assembled_bytes - index);
        while (position < ranges.size()) {
            size_t seq = index + res.size();
            size_t next_seq = ranges[position].first;
            if (seq >= next_seq) {
                if (seq < next_seq + ranges[position].second.size()) {
                    res += ranges[position].second.substr(seq - next_seq);
                }
                // 减少unassembled bytes
                _unassembled_bytes -= ranges[position].second.size();
            } else {
                break;
            }
            position++;
        }
        _assembled_bytes += res.size();
    } else {
        // this branch can not assembled string ,but we should merge range
        //首先将没交叉的范围插入
        while (position < ranges.size()) {
            size_t next_seq = ranges[position].first;
            if (next_seq <= index) {
                tmp_ranges.push_back(ranges[position]);
            } else {
                break;
            }
            position++;
        }
        //插入index 和 data
        if (tmp_ranges.empty() || tmp_ranges.back().first + tmp_ranges.back().second.size() < index) {
            tmp_ranges.push_back(index_data);
            //增加unassembled bytes
            _unassembled_bytes += data.size();
        } else {
            size_t last_seq = tmp_ranges.back().first + tmp_ranges.back().second.size();
            tmp_ranges.back().second += data.substr(last_seq - index);
            //增加unassembled bytes
            _unassembled_bytes += data.size() - (last_seq - index);
        }
        //插入后续可能相交的范围
        while (position < ranges.size()) {
            size_t last_seq = tmp_ranges.back().first + tmp_ranges.back().second.size();
            if (last_seq >= ranges[position].first) {
                if (last_seq < ranges[position].first + ranges[position].second.size()) {
                    tmp_ranges.back().second += ranges[position].second.substr(last_seq - ranges[position].first);
                    _unassembled_bytes -= last_seq - ranges[position].first;
                } else {
                    _unassembled_bytes -= ranges[position].second.size();
                }
                position++;
            } else {
                break;
            }
        }
    }
    //将最后不相交的范围插入
    while (position < ranges.size()) {
        tmp_ranges.push_back(ranges[position++]);
    }
    ranges.swap(tmp_ranges);
    return res;
}
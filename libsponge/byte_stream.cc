#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity):_capacity(capacity),_size(0){}

size_t ByteStream::write(const string &data) {
    size_t remaining_place = remaining_capacity();
    size_t res = 0;
    if(remaining_place < data.size()) {
        bufferlist.append(data.substr(0, remaining_place));
        res = remaining_capacity();
        _size = _capacity;
    } else {
        string str = data;
        Buffer buf{std::move(str)};
        bufferlist.append(buf);
        res = data.size();
        _size += res;
    }
    write_bytes += res;
    return res;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    //first use slowly algorithm
    string res = bufferlist.concatenate();
    if(len > _size) {
        return res;
    } 
    return res.substr(0, len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    if(len > _size) {
        bufferlist.remove_prefix(_size);
        _size = 0;
    } else {
        _size -= len;
        bufferlist.remove_prefix(len);
    }
    read_bytes += len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() { IsInputEnding = true; }

bool ByteStream::input_ended() const { return IsInputEnding; }

size_t ByteStream::buffer_size() const { return _size; }

bool ByteStream::buffer_empty() const { return _size == 0; }

bool ByteStream::eof() const { return IsInputEnding && buffer_empty(); }

size_t ByteStream::bytes_written() const { return write_bytes; }

size_t ByteStream::bytes_read() const { return read_bytes; }

size_t ByteStream::remaining_capacity() const { return _capacity - _size; }

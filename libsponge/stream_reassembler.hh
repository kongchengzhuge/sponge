#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <queue>
#include <unordered_map>
#include <string>

using namespace std;


//     bool operator()(const pair<size_t, const string> &p1, const pair<size_t, const string> &p2) {
//         return p1.first < p2.first;
//     }
// };
//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.
    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes
    size_t _assembled_bytes;
    size_t _unassembled_bytes;
    bool iseof{false};
    size_t eof_seq{0};
    //first , I think using priority_queue is enough, but pq can not handle overlap string 
    //so , maybe using hashmap is better
    //update :: hashmap is not right , priority_queue + range vector is better
    //update :: maybe , just uses vector is best , time can not smaller than O(n)
    // priority_queue<std::pair<size_t, string>, vector<std::pair<size_t, string>>, greater<std::pair<size_t, string>>> unassembled;
    vector<std::pair<size_t, std::string>> ranges;
  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const size_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;

  private:
    size_t assembled_but_not_read() const { return _assembled_bytes - _output.buffer_size(); }
    size_t real_capacity() const { return _output.bytes_read() + _capacity; }
    // void index_bigger_than_assembled(const string &data, const size_t index);
    std::string add_range(const string &data, size_t index);
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "stream_reassembler.hh"
#include <iostream>
#include <limits>
#include <vector>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

/**
 *  Test whether the new passed string is duplicate with the existed string
 *      e.g.: 
 *          first: "abc" 0
 *          second: "bcdefg" 1
 *              Then the "bc" are duplicated part
 *  @param it: iterator, pointer to pairs, contains two values left index and right, represent the range of received byte
 *  @param l: start index of the passed in string;
 *  @param r: end index of the passed in string;
 *      e.g.: "bcdefg" 6: l: 6; r: 6 + len(str) = 11;
 *  
 */
// void StreamReassembler::remove_segment(const Type1 &it, size_t l, size_t r, Type2 &_erase, Type2 &_insert){
//     DUMMY_CODE(it, l, r);

//     // it->second is the second value of the pair
//     // If no duplicate, stop this function;
//     if(l >= it->second || r <= it->first) return;

//     // Else has duplicated part, just insert the whole string into the _erase vector
//     //     Then insert the not duplicate part into the _insert vector
//     _erase.push_back(*it);
//     // if part of the byte is outside of the range
//     // From it->first to left boundary is outside
//     if(l >= it->first){
//         // not duplicate part; from the end of the existed string to the end of this passed in string
//         auto new_node = make_pair(r, it->second);
        
//         // Store the length of duplicated part;
//         _stored_bytes += min(r, it->second) - l;

//         auto node = *it;
//         node.second = l;
        
//         if(node.second > node.first){
//             _insert.push_back(node);
//         }
//         if(new_node.second > new_node.first){
//             _insert.push_back(new_node);
//         }
//     }
//     // If the start of byte in the range
//     else{
//         // stored byte only store the range of used bytes, if l < it->first which means from l to it->first will out side of this stream, discard this part
//         _stored_bytes += min(r, it->second) - it->first;
//         auto node = *it;
//         node.second = r;
//         if(node.second > node.first){
//             _insert.push_back(node);
//         }
//     }
// }
void StreamReassembler::remove_segment(const Type1 &it, size_t l, size_t r, Type2 &_erase, Type2 &_insert) {
    DUMMY_CODE(it, l, r);
    if(l >= it -> second || r <= it -> first) return;
    _erase.push_back(*it);
    if(l >= it -> first) {
        auto new_node = make_pair(r, it -> second);
        _stored_bytes += min(r, it -> second) - l;
        auto node = *it;
        node.second = l;
        if(node.second > node.first)
            _insert.push_back(node);
        if(new_node.second > new_node.first)
            _insert.push_back(new_node);
    } else {
        _stored_bytes += min(r, it -> second) - it -> first;
        auto node = *it;
        node.second = r;
        if(node.second > node.first)
            _insert.push_back(node);
    }
}


StreamReassembler::StreamReassembler(const size_t capacity) : 
    _output(capacity), 
    _capacity(capacity), 
    _assembled_bytes(0),
    _stored_bytes(0), 
    _str_to_assemble(), 
    _existed(), 
    _eof(false),  
    _used_byte{std::make_pair(0, std::numeric_limits<size_t>::max())
    } 
{}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
// data is like "abc": represent the string that passed in in stream
// index is a number: represent the position of the first char in data: e.g.: 'bcdefg' 6: index is 6 means 'b' is at index 6 position
// eof is boolean default value is false: represent whether the stream has end or not.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    cout << "data value: " << data << '\n';
    cout << "index: " << index << '\n';
    cout << "Eof: " << eof << '\n';
    DUMMY_CODE(data, index, eof);

    // If the input data's index is larger than capacity
    if(index >= _capacity + _output.bytes_read()) return;

    string tmp(data);
    // cout << "This is tmp: " << tmp;

    // Total number of bytes popped
    if(index + tmp.size() > _capacity + _output.bytes_read()){
        tmp.resize(_capacity + _output.bytes_read() - index);
    } else _eof |= eof;

    if(index + tmp.size() <= _assembled_bytes){
        if(empty()) _output.end_input();
        return;
    }

    // Calculate stored bytes
    // Iterator of _used_byte type(set of pairs) which is pair type
    // Lower bound is to find the first iterator in _used_byte which first value larger or equalto index and same as second value
    // Lower bound for vector of pairs will be: return first pairs whose both first and second value larger than the given value
    decltype(_used_byte)::iterator it_l = _used_byte.lower_bound(make_pair(index, index));
    decltype(_used_byte)::iterator it_r = _used_byte.lower_bound(make_pair(index+tmp.size(), index + tmp.size()));

    // Define two vectors, both contain pair as inside element
    vector<pair<size_t, size_t>> _need_to_erase{}, _need_to_insert{};


    // for (auto itr : _used_byte){
    //     cout << itr.first << " " << itr.second << '\n';
    // }

    if(it_l != _used_byte.begin()) it_l--;
    
    while (it_l != _used_byte.end()){
        remove_segment(it_l, index, index+tmp.size(), _need_to_erase, _need_to_insert);
        if(it_l == it_r) return;
        it_l++;
    }

    for (auto v: _need_to_erase){
        cout << "_need_to_erase is : " << v.first << " " << v.second << '\n';
        _used_byte.erase(v);
    };
    for (auto v: _need_to_insert){ 
        cout << "_need_to_insert is : " << v.first << " " << v.second << '\n';
        _used_byte.insert(v);
    };

    decltype(_existed)::iterator it = _existed.lower_bound(index);
    if(it == _existed.end()){
        _str_to_assemble.push_back(make_pair(tmp, index));
        _existed[index] = (--_str_to_assemble.end()); 
    }else{
        if(it -> first == index){
            if(tmp.size() > (it->second->first).size()){
                *(it -> second) = make_pair(tmp, index);
            }
        }else{
            // list.insert will return a iterator point to it->second and will insert value ahead of the indicator
            _existed[index] = _str_to_assemble.insert(it -> second, make_pair(tmp, index));
        }
    }

    // .front() return a reference at the beginning of the _str_to_assemble
    while(!_str_to_assemble.empty() && _str_to_assemble.front().second <= _assembled_bytes){
        size_t _index = _str_to_assemble.front().second;
        string _data = _str_to_assemble.front().first;
        if(_index + _data.size() > _assembled_bytes){
            _output.write(_data.substr(_assembled_bytes - _index, _index + data.size() - _assembled_bytes));
            _assembled_bytes = _index + _data.size();
        }
        _existed.erase(_index);
        _str_to_assemble.pop_front();
    }
    if(empty()) _output.end_input();


}

size_t StreamReassembler::unassembled_bytes() const { return _stored_bytes - _assembled_bytes; }

bool StreamReassembler::empty() const { return (_stored_bytes == _assembled_bytes) && _eof; }



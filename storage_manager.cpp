#include "storage_manager.h"
#include <iostream>
#include <algorithm>
#include <cassert>

using std::cout;
using std::endl;

auto StorageManager::findChunkBySize(size_t no_less_than) const{
    return std::find_if( entries_.cbegin(), entries_.cend(),
        [&no_less_than](const auto it){return (it.size >= no_less_than && it.is_free);} );
}

auto StorageManager::findChunkByFirstIndex(size_t first_index) const{
    return std::find_if( entries_.cbegin(), entries_.cend(),
        [&first_index](const auto it){return (it.first_index == first_index && !it.is_free);} );
}

StorageManager::StorageManager(size_t size_0) : size_(size_0) {

    assert(size_ != 0);

    entries_.push_back( TableEntry{0, size_, true} );
    data_ = std::vector<unsigned char>(size_);
}

void* StorageManager::Allocate(size_t bytes_wanted){

    cout << "Storage manager Allocate() called with " << bytes_wanted << " bytes" << endl;

    auto chunk = findChunkBySize(bytes_wanted);

    if (chunk != entries_.cend()){
        if (bytes_wanted == chunk->size){
            auto non_const_chunk = entries_.erase(chunk, chunk);//cast to non const iterator!
            non_const_chunk->is_free = false;
            return static_cast<void*>(data_.data() + non_const_chunk->first_index);
        }
        else if (bytes_wanted == 0)
            throw; //Can allocate no less than 1 byte
        else{
            TableEntry entry = *chunk;
            auto non_const_chunk = entries_.erase(chunk);
            non_const_chunk = entries_.insert( non_const_chunk,
                TableEntry{entry.first_index+(entry.size-bytes_wanted), entry.size-bytes_wanted, true} );
            non_const_chunk = entries_.insert( non_const_chunk,
                TableEntry{entry.first_index, bytes_wanted, false} );
            return static_cast<void*>(data_.data() + non_const_chunk->first_index);
        }
    }
    else{
        throw; //No memory for your chunk
    }
}

void StorageManager::Free(void *ptr){

    cout << "Storage manager Free() called" << endl;

    auto chunk = findChunkByFirstIndex(static_cast<unsigned char*>(ptr) - data_.data());
    auto non_const_chunk = entries_.erase(chunk, chunk);
    non_const_chunk->is_free = true;

    if (chunk != entries_.cend()){
        bool is_begin = non_const_chunk==entries_.begin();
        bool is_rbegin = std::make_reverse_iterator(non_const_chunk)==entries_.rbegin();

        if (!is_begin){
            auto left_chunk = std::prev(non_const_chunk, 1);
            if (left_chunk->is_free){
                non_const_chunk->first_index -= left_chunk->size;
                non_const_chunk->size += left_chunk->size;
                entries_.erase(left_chunk);
            }
        }

        if (!is_rbegin){
            auto right_chunk = std::next(non_const_chunk, 1);
            if (right_chunk->is_free){
                non_const_chunk->size += right_chunk->size;
                entries_.erase(right_chunk);
            }
        }
    }
    else{
        throw; //Memory for this pointer has not been allocated
    }
}

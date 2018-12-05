#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <list>
#include <vector>

struct TableEntry{
    size_t first_index;
    size_t size;
    bool is_free;
};

class StorageManager{
private:
    std::list<TableEntry> entries_;
    std::vector<unsigned char> data_;
    const size_t size_;

    auto findChunkBySize(size_t no_less_than) const;

    auto findChunkByFirstIndex(size_t first_index) const;

public:
    StorageManager(size_t size);

    void* Allocate(size_t bytes_wanted);

    void Free(void *ptr);
};

#endif // STORAGE_MANAGER_H

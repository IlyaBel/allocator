#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <iostream>
#include <memory>
#include <list>
#include <algorithm>
#include <vector>
#include <iterator>
#include <cassert>

#define USE_COUT \
    using std::cout; \
    using std::endl;

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

template<class T>
class MyAllocator{
private:
    std::shared_ptr<StorageManager> storage_manager_;

public:
    using value_type = T;

    MyAllocator(std::shared_ptr<StorageManager> storage_manager_0) :
        storage_manager_(storage_manager_0) {}

    T* allocate( std::size_t n ){

        USE_COUT

        cout << "Allocator allocated memory for " << n << " objects" << endl;

        return static_cast<T*>( storage_manager_->Allocate(n * sizeof(T)) );
    }

    void deallocate( T* p, std::size_t n ){

        USE_COUT

        cout << "Allocator deallocated memory for " << n << " objects" << endl;

        storage_manager_->Free(p);
    }

    template <class U, class... Types>
    void construct( U* p, Types&&... args ){

        USE_COUT

        cout << "Allocator constructed an object" << endl;

        new (p) U(args...);
    }

    template <class U>
    void destroy( U *p ){

        USE_COUT

        cout << "Allocator destroyed an object" << endl;

        (*p).~U();
    }
};

#endif // ALLOCATOR_H

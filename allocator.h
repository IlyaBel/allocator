#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "storage_manager.h"
#include <iostream>
#include <memory>

#define USE_COUT \
    using std::cout; \
    using std::endl;

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

#include <iostream>
#include <memory>
#include <list>
#include <algorithm>
#include <vector>
#include <iterator>
#include <cassert>

using std::cout;
using std::endl;

class C{
private:
    static size_t count;
    const size_t id;

public:
    C() : id(count){
        cout << "Object with id " << id << " created." << endl;
        count++;
    }
};

size_t C::count = 0;

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

    auto findChunkBySize(size_t no_less_than) const{
        return std::find_if( entries_.cbegin(), entries_.cend(),
            [&no_less_than](const auto it){return (it.size >= no_less_than && it.is_free);} );
    }

    auto findChunkByFirstIndex(size_t first_index) const{
        return std::find_if( entries_.cbegin(), entries_.cend(),
            [&first_index](const auto it){return (it.first_index == first_index && !it.is_free);} );
    }

public:
    StorageManager(size_t size_0) : size_(size_0) {

        assert(size_ != 0);

        entries_.push_back( TableEntry{0, size_, true} );
        data_ = std::vector<unsigned char>(size_);
    }

    void* Allocate(size_t bytes_wanted){

        cout << "Storage manager Allocate() called with " << bytes_wanted << " bytes" << endl;

        auto chunk = findChunkBySize(bytes_wanted);

        if (chunk != entries_.cend()){
            if (bytes_wanted == chunk->size){
                auto non_const_chunk = entries_.erase(chunk, chunk);
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

    void Free(void *ptr){

        cout << "Storage manager Free() called" << endl;

        auto chunk = findChunkByFirstIndex(static_cast<unsigned char*>(ptr) - data_.data());
        auto non_const_chunk = entries_.erase(chunk, chunk);
        non_const_chunk->is_free = true;

        if (chunk != entries_.cend()){
            bool is_begin = non_const_chunk==entries_.begin()?true:false;
            bool is_rbegin = std::make_reverse_iterator(non_const_chunk)==entries_.rbegin()?true:false;

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

        cout << "Allocator allocated memory for " << n << " objects" << endl;

        return static_cast<T*>( storage_manager_->Allocate(n * sizeof(T)) );
    }

    void deallocate( T* p, std::size_t n ){

        cout << "Allocator deallocated memory for " << n << " objects" << endl;

        storage_manager_->Free(p);
    }

    template <class U, class... Types>
    void construct( U* p, Types&&... args ){

        cout << "Allocator constructed an object" << endl;

        new (p) U(args...);
    }

    template <class U>
    void destroy( U *p ){

        cout << "Allocator destroyed an object" << endl;

        (*p).~U();
    }
};

int main() {

    constexpr size_t N = 5;
    
    //1. Static array
    {
        C arr[N];
    }

    //2. Dynamic array via "new" operator
    {
        C *arr = new C[N];
        delete [] arr;
    }

    //3. The same job with "placement new"
    {
        C *arr = static_cast<C*>( malloc(N*sizeof(C)) );

        for (size_t i = 0; i < N; i++){
            new (arr+i) C{};
        }

        for (size_t i = 0; i < N; i++){
            (*(arr+i)).~C();
        }

        free(arr);
    }
    //4, 5. Using std::allocaator and custom allocator
    {
        //std::allocator
//        std::allocator allocator;

        //Or custom allocator
        constexpr size_t storage_size = 100000;
        std::shared_ptr<StorageManager> storage_manager =
            std::make_shared<StorageManager>(storage_size);
        MyAllocator<C> allocator(storage_manager);

        //Then we can work with them in the same way
         C *arr = allocator.allocate(N);

         for (size_t i = 0; i < N; i++){
            allocator.construct(arr+i);
         }

         for (size_t i = 0; i < N; i++){
            allocator.destroy(arr+i);
         }

         allocator.deallocate(arr, N);
    }
    
    //6. How vector works
    {
        constexpr size_t storage_size = 100000;
        std::shared_ptr<StorageManager> storage_manager =
            std::make_shared<StorageManager>(storage_size);
        MyAllocator<int> allocator(storage_manager);

        cout << "Creating a vector:" << endl;

        std::vector<int, MyAllocator<int>> v(1, allocator);

        cout << "push_back():" << endl;

        v.push_back(1);

        cout << "reserve():" << endl;

        v.reserve(10);
    }

    return 0;
}

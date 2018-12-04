#include "allocator.h"
#include <iostream>

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
        //To use std::allocator uncomment the string below
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
//    {
//        constexpr size_t storage_size = 100000;
//        std::shared_ptr<StorageManager> storage_manager =
//            std::make_shared<StorageManager>(storage_size);
//        MyAllocator<int> allocator(storage_manager);

//        cout << "Creating a vector:" << endl;

//        std::vector<int, MyAllocator<int>> v(1, allocator);

//        cout << "push_back():" << endl;

//        v.push_back(1);

//        cout << "reserve():" << endl;

//        v.reserve(10);
//    }

    return 0;
}

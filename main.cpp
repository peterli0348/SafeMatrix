/**
 * Peter Li (23375692)
 * CSCI 381 Advance C++
 * Midterm: Memory Management
 * Due May 8th, 2020 before 11:59PM
 *
 * Requirements:
 * Rework the one-dimensional Safearray template class so that it allocates memory by including a Block object
 * Blocks are objects allocated from a memory pool that is maintained in a boundary tag implementation
 * Blocks are tagged memory regions that are either on a liked list of available blocks or currently allocated for use
 * Block class will define its own versions of allocate and free and will allocate blocks from the memory pool that it controls
 * The Block class will allocate a large area of memory when the first block is created
 * Anytime a new block is required, allocate from that memory pool
 * The block class will get the initial memory by calling the system new
 *
 *********************************
 *
 * These variables from Block class effect memory efficiency and throughput:
 * INITDATASIZE     is the amount of words a block can store at initialization
 * INITBLOCKCOUNT   is the amount of blocks created at initialization
 * MINDATASIZE      is the minimum size of data in split(top) block
 *
 *
 * Allocator:
 * 1.   Contiguous allocation
 * 2.   Next Fit
 * 3.   Variable partition
 * 4.   Immediate coalescing
 * 5.   Explicit free list
 * 6.   LIFO
 * 7.   Static memory pool
 *
 * Statistical information is output to console if SAFEARRAY_BLOCK_DEBUG is true
 *
 */

#include <ctime>
#include "SafeMatrix.h"
using namespace std;

template <typename T>
size_t Block<T>::requestSize = 0;
template <typename T>
size_t Block<T>::blockSize = 0;
template <typename T>
int Block<T>::blockCnt = 0;
template <typename T>
int Block<T>::searchCnt = 0;
template <typename T>
int Block<T>::requestCnt = 0;
template <typename T>
int Block<T>::failureCnt = 0;
template <typename T>
int Block<T>::splitCnt = 0;
template <typename T>
int Block<T>::coalesceCnt = 0;
template <typename T>
double Block<T>::avgSearchCnt = 0;
template <typename T>
double Block<T>::successRate = 0;
template <typename T>
double Block<T>::failureRate = 0;

template <typename T>
typename Block<T>::Header * Block<T>::MEMPOOL;
template <typename T>
uintptr_t * Block<T>::LMEMPOOLBOUND;
template <typename T>
uintptr_t * Block<T>::RMEMPOOLBOUND;
template <typename T>
typename Block<T>::Header * Block<T>::AV;

template <typename T>
int Block<T>::MINDATASIZE = 32; // must be greater than 6

int main() {

    /*
     * random block requests
     * SafeArray constructor with low and high index not used to avoid random invalid bounds
     * bounds are from 0(default) through 1-100(random)
     */

    srand(time(nullptr));
    for (int i = 0; i < 3; i++) {
        std::cout << std::endl << "Press Enter:";
        std::cin.get();

        SafeArray<int> * test0 = new SafeArray<int>(rand() % 100 + 1);
        SafeArray<int> * test1 = new SafeArray<int>(rand() % 100 + 1);
        delete test1;
        SafeArray<int> * test2 = new SafeArray<int>(rand() % 100 + 1);
        delete test2;
        delete test0;
    }

    // arithmetic using safe array with custom memory allocator

    SafeArray<int> array0{1,2,3,4,5};

    SafeArray<int> array1{2,3,4,5,6};

    std::cout << array0 << std::endl << "-" << std::endl << array1 << std::endl << "=" << std::endl;
    array1 = array0 - array1;
    std::cout << array1 << std::endl;

    // arithmetic using safe matrix with custom memory allocator

    SafeMatrix<int> matrix0{{1,2,3},{4,5,6},{7,8,9}};

    SafeMatrix<int> matrix1{{2,2,2},{2,2,2},{2,2,2}};

    std::cout << matrix0 << "*" << std::endl << matrix1 << "=" << std::endl;
    matrix1 = matrix0 * matrix1;
    std::cout << matrix1 << std::endl;

    return 0;
}
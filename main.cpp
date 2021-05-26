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
#include <fstream>
#include "VNT.h"
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

    std::cout << array0 << std::endl << "-" << std::endl
        << array1 << std::endl << "=" << std::endl;
    array1 = array0 - array1;
    std::cout << array1 << std::endl;

    // arithmetic using safe matrix with custom memory allocator

    SafeMatrix<int> matrix0{{1,2,3},{4,5,6},{7,8,9}};

    SafeMatrix<int> matrix1{{2,2,2},{2,2,2},{2,2,2}};

    std::cout << matrix0 << "*" << std::endl << matrix1 << "=" << std::endl;
    matrix1 = matrix0 * matrix1;
    std::cout << matrix1 << std::endl;

/**
 * Peter Li (23375692)
 * CSCI 381 Advance C++
 * Project 3: VNT
 * Due April 3rd, 2020 before 11:59PM
 * Cut-off April 13th, 2020 before 11:59PM
 * -10 points for each day late
 *
 * We want a class called VNT which is going to handle an m by n matrix
 * Entries of each row are in sorted order from left to right
 * Entries of each column are in sorted order from top to bottom
 * Entries of a VNT may be INT_MAX , which we treat as nonexistent elements
 * Thus, a VNT can be used to hold r ≤ mn integers
 *
 * We want the class to be able to do the following things:
 * 1.   If A is a VNT object then A[i][j] is the i,j th element of the underlying 2 dimensional Matrix
 *      So if A is empty then A[0][[0] = INT_MAX
 *      and if A is full then A[m-1][n-1] < INT_MAX
 *
 * 2.   VNT(int m, int n) will create an m x n VNT object     e.g. VNT A(5,7)
 *
 * 3.   A.getMin() will extract the smallest element for A and leave A a VNT
 *
 * 4.   A.sort(int k[], int size)  will sort the n x n numbers in k using A
 *      and not calling any sort routine as a subroutine
 *
 * 5.   A.find(int i) will return true if i is in A and false otherwise
 *
 * CONSTRAINTS:
 * 1.   function add() should work in time proportional to m+n
 * 2.   function getMin() should work in time proportional to m+n
 * 3.   function sort() should work in time proportional to n^3
 * 4.   function find() should work in time proportional to m+n
 */

    std::ofstream outFile;
    outFile.open("VNT_output.txt");

    VNT<int> table(3, 99);

    outFile << "VNT 3x3 with INT_MAX = 99" << std::endl << table << std::endl;

    table.add(12);
    outFile << "add()" << std::endl << table << std::endl;
    table.add(87);
    outFile << "add()" << std::endl << table << std::endl;
    table.add(38);
    outFile << "add()" << std::endl << table << std::endl;
    table.add(3);
    outFile << "add()" << std::endl << table << std::endl;
    table.add(87);
    outFile << "add()" << std::endl << table << std::endl;
    table.add(12);
    outFile << "add()" << std::endl << table << std::endl;
    table.add(12);
    outFile << "add()" << std::endl << table << std::endl;
    table.add(74);
    outFile << "add()" << std::endl << table << std::endl;
    table.add(1);
    outFile << "add()" << std::endl << table << std::endl;

    outFile << "getMin()" << std::endl << table.getMin() << std::endl << std::endl;

    // n x n int[] should not contain values higher than VNT._MAX
    int sortF[9] = {12, 73, 38, 88, 8, 4, 57, 53, 1};
    VNT<int> sortedTable = table.sort(sortF, 9);
    outFile << "sort()" << std::endl << sortedTable << std::endl;

    int findElement = 88;
    outFile << "find(" << findElement << ")" << std::endl << (sortedTable.find(findElement) ? "true" : "false") << std::endl;

    outFile.close();
    return 0;
}

/*
 * Block class implements a custom memory allocator for 1D arrays
 *
 * These variables from Block class effect memory efficiency and throughput:
 * INITDATASIZE     is the amount of words a block can store at initialization
 * INITBLOCKCOUNT   is the amount of blocks created at initialization
 * MINDATASIZE      is the minimum size of data in split(top) block
 *
 * Details:
 *
 * 1.   Contiguous allocation
 * 2.   Next Fit
 * 3.   Variable partition
 * 4.   Immediate coalescing
 * 5.   Explicit free list
 * 6.   LIFO
 * 7.   Static memory pool
 */

#ifndef SAFEARRAY_BLOCK_H
#define SAFEARRAY_BLOCK_H
#define SAFEARRAY_BLOCK_DEBUG true

#include <iostream>

template <typename T>
class Block {
private:
    // information located at "top" of block using 4 words
    struct Header {
        Header * LLINK, * RLINK;
        // size includes size of header and footer
        std::size_t SIZE;
        bool TAG;
    };

    // information located at "bottom" of block using 2 words
    struct Footer {
        bool TAG;
        Header * UPLINK;
    };

    // minimum block size in words
    static int MINDATASIZE;

    // initial data size in words
    enum { INITDATASIZE = 64, INITBLOCKCOUNT = 100, SIZEOFHEAD = 4, SIZEOFFOOT = 2, OFFSET = SIZEOFHEAD + SIZEOFFOOT};

    // memory pool
    static uintptr_t * LMEMPOOLBOUND;
    static uintptr_t * RMEMPOOLBOUND;
    static Header * MEMPOOL;
    static Header * AV;

public:

    // data stored inside block
    T data[1];

    Block<T>() {
        constructorMsg();
    }

    ~Block<T>() {
        // destructor message in operator delete
    }

    static Header * initialPool() {
        // get contiguous memory pool from OS
        Header * _poolStart = reinterpret_cast<Header *>(new uintptr_t[(INITDATASIZE + OFFSET) * INITBLOCKCOUNT]);
        if (SAFEARRAY_BLOCK_DEBUG) {
            std::cout << (INITDATASIZE + OFFSET) * INITBLOCKCOUNT * sizeof(uintptr_t) << " bytes initialized"
                      << std::endl;
        }

        // failed to get contiguous memory pool
        if (!_poolStart) {
            if (SAFEARRAY_BLOCK_DEBUG) std::cout << "Error: failed calloc" << std::endl;
            exit(1);
        }

        // left bound of memory pool
        LMEMPOOLBOUND = reinterpret_cast<uintptr_t *>(_poolStart);

        // beginning of memory pool
        Header * _head = reinterpret_cast<Header *>(_poolStart);

        // initialize blocks
        for (int i = 0; i < INITBLOCKCOUNT; i++) {
            // set header
            // total size = block size + offset
            _head->SIZE = INITDATASIZE + OFFSET;
            _head->TAG = false;
            _head->LLINK = reinterpret_cast<Header *>(reinterpret_cast<uintptr_t *>
                    (_head) - INITDATASIZE - OFFSET);
            _head->RLINK = reinterpret_cast<Header *>(reinterpret_cast<uintptr_t *>
                    (_head) + INITDATASIZE + OFFSET);

            // set footer
            reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                (_head) + INITDATASIZE + SIZEOFHEAD)->TAG = false;
            reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                (_head) + INITDATASIZE + SIZEOFHEAD)->UPLINK = _head;

            // initialize next block in memory
            _head = _head->RLINK;
        }

        // right bound of memory pool
        RMEMPOOLBOUND = reinterpret_cast<uintptr_t *>(_head);

        // last block in memory pool
        _head = reinterpret_cast<Header *>(reinterpret_cast<uintptr_t *>
                (_head) - INITDATASIZE - OFFSET);

        // link blocks at both ends of memory pool
        _head->RLINK = _poolStart;
        _poolStart->LLINK = _head;

        // set starting position of next search
        AV = _head;

        // statistic collection
        blockCnt = INITBLOCKCOUNT;

        // return start address of memory pool
        return _poolStart;
    }

    static Block<T> * allocate(std::size_t _size) {
        // initialize memory pool
        if (!MEMPOOL)
            MEMPOOL = initialPool();

        // start search from free list
        Header * _freeHead = AV->RLINK;

        // align and set size in terms of words including header and footer offset
        _size = align(_size) / sizeof(uintptr_t) + OFFSET;

        // statistic collection
        requestCnt++;
        requestSize = _size - OFFSET;
        searchCnt = 1;

        // search through blocks on the free list
        while (_freeHead->TAG == false && _freeHead->SIZE >= _size) {
            // statistic collection
            searchCnt++;

            // block with enough memory
            if (_freeHead->SIZE >= _size) {
                int _difference = _freeHead->SIZE - _size;

                // insignificant inner fragmentation
                if (_difference <= MINDATASIZE + OFFSET) {
                    // remove block from free list
                    _freeHead->LLINK->RLINK = _freeHead->RLINK;
                    _freeHead->RLINK->LLINK = _freeHead->LLINK;

                    // set header and footer tags
                    _freeHead->TAG = true;
                    reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                        (_freeHead) + _freeHead->SIZE - SIZEOFFOOT)->TAG = true;

                    // set starting position of next search
                    AV = _freeHead->LLINK;

                    // statistic collection
                    blockSize = _freeHead->SIZE;
                    avgSearchCnt = ((avgSearchCnt * (double) (requestCnt - 1)) + searchCnt) / (double) requestCnt;
                    successRate = 1 - (double) failureCnt / (double) requestCnt;
                    failureRate = (double) failureCnt / (double) requestCnt;

                    // return data memory address
                    return reinterpret_cast<Block<T> *>(reinterpret_cast<uintptr_t *>
                        (_freeHead) + SIZEOFHEAD);
                }
                // split block to maximize memory efficiency and return bottom block
                else {
                    // top block size
                    _freeHead->SIZE = _difference;

                    // uplink of top block
                    reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                        (_freeHead) + _freeHead->SIZE - SIZEOFFOOT)->UPLINK = _freeHead;

                    // set starting position of next search
                    AV = _freeHead->LLINK;

                    // bottom block
                    Header * _newHead = reinterpret_cast<Header *>(reinterpret_cast<uintptr_t *>
                            (_freeHead) + _freeHead->SIZE);

                    // set size of bottom block
                    _newHead->SIZE = _size;

                    // set header and footer tags of bottom block
                    _newHead->TAG = true;
                    reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                        (_newHead) + _newHead->SIZE - SIZEOFFOOT)->TAG = true;

                    // set uplink of bottom block
                    reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                        (_newHead) + _newHead->SIZE - SIZEOFFOOT)->UPLINK = _newHead;

                    // statistic collection
                    blockSize = _newHead->SIZE;
                    blockCnt++;
                    splitCnt++;
                    avgSearchCnt = ((avgSearchCnt * (double) (requestCnt - 1)) + searchCnt) / (double) requestCnt;
                    successRate = 1 - (double) failureCnt / (double) requestCnt;
                    failureRate = (double) failureCnt / (double) requestCnt;

                    // return data memory address
                    return reinterpret_cast<Block<T> *>(reinterpret_cast<uintptr_t *>
                        (_newHead) + SIZEOFHEAD);
                }
            }
            // search next block on free list
            _freeHead = _freeHead->RLINK;
        }
        // statistic collection
        blockSize = 0;
        failureCnt++;
        avgSearchCnt = ((avgSearchCnt * (double) (requestCnt - 1)) + searchCnt) / (double) requestCnt;
        successRate = 1 - (double) failureCnt / (double) requestCnt;
        failureRate = (double) failureCnt / (double) requestCnt;

        // no free block with enough memory
        return nullptr;
    }

    static void deallocate(Block<T> * _block) {
        // get header and footer of block
        Header * _head = _block->getHead();
        Footer * _foot = _block->getFoot();

        // initialize head and tag of adjacent block
        bool _leftTag = true;
        bool _rightTag = true;
        Header * _leftHead = nullptr;
        Header * _rightHead = nullptr;

        // get head and tag of adjacent blocks within memory bounds
        if (LMEMPOOLBOUND != reinterpret_cast<uintptr_t *>(_head)) {
            _leftHead = reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                (_head) - SIZEOFFOOT)->UPLINK;
            _leftTag = _leftHead->TAG;
        }
        if (RMEMPOOLBOUND != reinterpret_cast<uintptr_t *> (_foot) + SIZEOFFOOT) {
            _rightHead = reinterpret_cast<Header *>(reinterpret_cast<uintptr_t *>
                (_foot) + SIZEOFFOOT);
            _rightTag = _rightHead->TAG;
        }

        // coalesce adjacent free blocks

        // adjacent blocks in use
        if (_leftTag == true && _rightTag == true) {
            // set tag on both header and footer to false
            _head->TAG = false;
            _foot->TAG = false;

            // insert block to right of free list
            _head->LLINK = AV;
            _head->RLINK = AV->RLINK;
            _head->LLINK->RLINK = _head;
            _head->RLINK->LLINK = _head;
        }
        // right block free
        else if (_leftTag == true && _rightTag == false) {
            // insert block at right block's position on free list
            _rightHead->LLINK->RLINK = _head;
            _rightHead->RLINK->LLINK = _head;
            _head->LLINK = _rightHead->LLINK;
            _head->RLINK = _rightHead->RLINK;

            // combine block size
            _head->SIZE = _head->SIZE + _rightHead->SIZE;

            // set right block uplink to block
            reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                (_head) + _head->SIZE - SIZEOFFOOT)->UPLINK = _head;

            // set tag on header to false
            // right block tag is already false
            _head->TAG = false;

            // set starting position of next search
            AV = _head->LLINK;

            // statistic collection
            blockCnt--;
            coalesceCnt++;
        }
        // left block free
        else if (_leftTag == false && _rightTag == true) {
            // combine block size
            _leftHead->SIZE = _leftHead->SIZE + _head->SIZE;

            // set block uplink to left block
            _foot->UPLINK = _leftHead;

            // set tag on footer to false;
            // left block tag is already false
            _foot->TAG = false;

            // set starting position of next search
            AV = _leftHead->LLINK;

            // statistic collection
            blockCnt--;
            coalesceCnt++;
        }
        // adjacent blocks free
        else if (_leftTag == false && _rightTag == false) {
            // remove right block from free list
            _rightHead->LLINK->RLINK = _rightHead->RLINK;
            _rightHead->RLINK->LLINK = _rightHead->LLINK;

            // combine block size
            _leftHead->SIZE = _leftHead->SIZE + _head->SIZE + _rightHead->SIZE;

            // set right block uplink to left block
            reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>
                (_rightHead) + _rightHead->SIZE - SIZEOFFOOT)->UPLINK = _leftHead;

            // set starting position of next search
            AV = _leftHead->LLINK;

            // statistic collection
            blockCnt -= 2;
            coalesceCnt++;
        }
    }

    static void * operator new(std::size_t _block, std::size_t _size) {
        return allocate(_size);
    }

    static void operator delete(void * _block) {
        deallocate(reinterpret_cast<Block<T> *>(_block));
        destructorMsg();
    }

    inline T & operator[](const int & index) {
        return data[index];
    }

private:

    /*
     * !!! only call from Block<T>->data memory address !!!
     * getHead() and getFoot() returns the address of the Block<T> object's header or footer
     */

    inline Header * getHead() {
        return reinterpret_cast<Header *>(reinterpret_cast<uintptr_t *>(this) - SIZEOFHEAD);
    }

    inline Footer * getFoot() {
        return reinterpret_cast<Footer *>(reinterpret_cast<uintptr_t *>(this) + this->getHead()->SIZE - OFFSET);
    }

    // align size in terms of uintptr_t using bit-wise operators
    inline static std::size_t align(std::size_t _size) {
        return (_size + sizeof(uintptr_t) - 1) & ~(sizeof(uintptr_t) - 1);
    }

    static void constructorMsg() {
        if (SAFEARRAY_BLOCK_DEBUG) {
            std::cout << std::endl << "Constructor message ----------------" << std::endl;
            std::cout << "Request size:\t\t" << Block<T>::requestSize
                      << "(+" << OFFSET << ")" << std::endl;
            std::cout << "Block size:\t\t" << Block<T>::blockSize << std::endl;
            std::cout << "Block count:\t\t" << Block<T>::blockCnt << std::endl;
            std::cout << "Split count:\t\t" << Block<T>::splitCnt << std::endl;
            std::cout << "Search count:\t\t" << Block<T>::searchCnt << std::endl;
            std::cout << "Search count avg:\t" << Block<T>::avgSearchCnt << std::endl;
            std::cout << std::endl;
            std::cout << "Request count:\t\t" << Block<T>::requestCnt << std::endl;
            std::cout << "Failure count:\t\t" << Block<T>::failureCnt << std::endl;
            std::cout << "Success rate:\t\t" << Block<T>::successRate << std::endl;
            std::cout << "Failure rate:\t\t" << Block<T>::failureRate << std::endl;
        }
    }

    static void destructorMsg() {
        if (SAFEARRAY_BLOCK_DEBUG) {
            std::cout << std::endl << "Destructor message ----------------" << std::endl;
            std::cout << "Block count:\t\t" << Block<T>::blockCnt << std::endl;
            std::cout << "Coalesce count:\t\t" << Block<T>::coalesceCnt << std::endl;
        }
    }

public:
    // statistics

    static std::size_t requestSize; // size of request without offset
    static std::size_t blockSize; // size of block
    static int blockCnt; // number of blocks on free list
    static int searchCnt; // number of blocks searched until request satisfied
    static int requestCnt; // number of requests for blocks made
    static int failureCnt; // number of times block request failed
    static int splitCnt; // number of times blocks split
    static int coalesceCnt; // number of times blocks coalesced

    static double avgSearchCnt; // average number of blocks searched until request satisfied
    static double successRate; // rate of satisfied request
    static double failureRate; // rate of fail requests
};

#endif //SAFEARRAY_BLOCK_H
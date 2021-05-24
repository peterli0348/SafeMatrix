/*
 * SafeArray class uses Block class to deal with memory management
 */

#ifndef SAFEARRAY_SAFEARRAY_H
#define SAFEARRAY_SAFEARRAY_H
#define SAFEARRAY_SAFEARRAY_DEBUG true

#include "Block.h"

template <typename T>
class SafeArray {
private:
    int low, high;
    Block<T> * array = nullptr;

public:
    // default constructor to allow creation on stack "SafeArray<T> a;"
    SafeArray()
            : low(0), high(-1) { }

    // overload constructor to make array with explicit higher bound
    explicit SafeArray(int l_high)
            : low(0), high(l_high){
        if (high < 0) {
            if (SAFEARRAY_SAFEARRAY_DEBUG) {
                std::cout << "Constructor error: bounds definition" << std::endl;
            }
            exit(1);
        }
        array = new ((high + 1) * sizeof(T)) Block<T>;
        if (array)
            fillArray(T());
    }

    // construct array with explicit lower and upper bounds
    explicit SafeArray(int l_low, int l_high)
            : low(l_low), high(l_high) {
        if (high - low < 0) {
            if (SAFEARRAY_SAFEARRAY_DEBUG) {
                std::cout << "Constructor error: bounds definition" << std::endl;
            }
            exit(1);
        }
        array = new ((high - low + 1) * sizeof(T)) Block<T>;
        if (array)
            fillArray(T());
    }

    // initializer_list constructor to allow "SafeArray<T> a{ t0, t1 }"
    explicit SafeArray(const std::initializer_list<T> & init_list)
            : low(0), high(init_list.size() - 1) {
        array = new ((high - low + 1) * sizeof(T)) Block<T>;
        auto it = begin(init_list);
        for (int col = 0; col <= high; col++) {
            (* array)[col] = (* it);
            it++;
        }
    }

    // copy constructor
    SafeArray(const SafeArray & l_SafeArray)
            : low(l_SafeArray.low), high(l_SafeArray.high) {
        int cols = high - low + 1;
        array = new (cols * sizeof(T)) Block<T>;
        for (int col = 0; col < cols; col++) {
            (* array)[col] = (* l_SafeArray.array)[col];
        }
    }

    ~ SafeArray() {
        if (array)
            delete array;
    }

    void fillArray(T element) {
        int cols = high - low + 1;
        for (int col = 0; col < cols; col++) {
            (* array)[col] = element;
        }
    }

    // overload the [] operator to allow "a[index] = T();"
    T & operator[](const int & index) {
        if (index < low || index > high) {
            if (SAFEARRAY_SAFEARRAY_DEBUG) {
                std::cout << "Index selector error: bounds selection " << index << " " << low << "-" << high
                          << std::endl;
            }
            exit(1);
        }
        return (* array)[index - low];
    }

    // overload the = operator to allow "SafeArray<T> sa1 = sa2;"
    SafeArray<T> & operator=(const SafeArray & l_SafeArray) {
        if (this == & l_SafeArray) return * this;
        low = l_SafeArray.low;
        high = l_SafeArray.high;
        int cols = high - low + 1;
        delete array;
        array = new (cols * sizeof(T)) Block<T>;
        for (int col = 0; col < cols; col++) {
            (* array)[col] = (* l_SafeArray.array)[col];
        }
        return * this;
    }

    SafeArray<T> & operator+(SafeArray l_SafeArray) {
        // SafeArray a(x) = b(x) + c(y) if and only if x = y
        if (high - low != l_SafeArray.high - l_SafeArray.low) {
            if (SAFEARRAY_SAFEARRAY_DEBUG) {
                std::cout << "Arithmetic error: array addition " << high - low + 1 << " "
                          << l_SafeArray.high - l_SafeArray.low + 1 << std::endl;
            }
            exit(1);
        }
        SafeArray<T> * result = new SafeArray<T>(* this);
        int y = l_SafeArray.low;
        for (int x = this->low; x <= this->high; x++) {
            (* result)[x] = (* array)[x] + l_SafeArray[y];
            y++;
        }
        return * result;
    }

    SafeArray<T> & operator-(SafeArray l_SafeArray) {
        // SafeArray a(x) = b(x) - c(y) if and only if x = y
        if (high - low != l_SafeArray.high - l_SafeArray.low) {
            if (SAFEARRAY_SAFEARRAY_DEBUG) {
                std::cout << "Arithmetic error: array subtraction " << high - low + 1 << " "
                          << l_SafeArray.high - l_SafeArray.low + 1 << std::endl;
            }
            exit(1);
        }
        SafeArray<T> * result = new SafeArray<T>(* this);
        int y = l_SafeArray.low;
        for (int x = this->low; x <= this->high; x++) {
            (* result)[x] = (* array)[x] - l_SafeArray[y];
            y++;
        }
        return * result;
    }

    SafeArray<T> & operator*(SafeArray l_SafeArray) {
        // SafeArray a(x) = b(x) * c(y) if and only if x = y
        if (high - low != l_SafeArray.high - l_SafeArray.low) {
            if (SAFEARRAY_SAFEARRAY_DEBUG) {
                std::cout << "Arithmetic error: array multiplication " << high - low + 1 << " "
                          << l_SafeArray.high - l_SafeArray.low + 1 << std::endl;
            }
            exit(1);
        }
        SafeArray<T> * result = new SafeArray<T>(* this);
        int y = l_SafeArray.low;
        for (int x = this->low; x <= this->high; x++) {
            (* result)[x] = (* array)[x] * l_SafeArray[y];
            y++;
        }
        return * result;
    }

    friend std::ostream & operator<<(std::ostream & l_ostream, const SafeArray<T> & l_SafeArray) {
        int cols = l_SafeArray.high - l_SafeArray.low + 1;
        for (int col = 0; col < cols; col++) {
            l_ostream << (* l_SafeArray.array)[col] << "\t";
        }
        return l_ostream;
    }
};

#endif //SAFEARRAY_SAFEARRAY_H
/*
 * SafeMatrix class uses SafeArray class to create a 2D matrix
 */

#ifndef SAFEARRAY_SAFEMATRIX_H
#define SAFEARRAY_SAFEMATRIX_H
#define SAFEARRAY_SAFEMATRIX_DEBUG true

#include "SafeArray.h"

template <typename T>
class SafeMatrix {
public:
    int rowLow, rowHigh, colLow, colHigh;
    SafeArray<T> ** matrix;

public:
    // default constructor to allow "SafeMatrix<T> a;"
    SafeMatrix()
            : rowLow(0), rowHigh(-1), colLow(0), colHigh(-1) {};

    // construct matrix with upper bound
    explicit SafeMatrix(int l_High)
            : rowLow(0), rowHigh(l_High), colLow(0), colHigh(l_High) {
        if (l_High < 0) {
            if (SAFEARRAY_SAFEMATRIX_DEBUG) {
                std::cout << "Constructor error: bounds definition" << std::endl;
            }
            exit(1);
        }
        // create array of SafeArray
        this->matrix = new SafeArray<T> * [l_High + 1];
        for (int row = 0; row <= l_High; row++) {
            this->matrix[row] = new SafeArray<T>(l_High);
        }
    }

    // construct matrix with upper bounds for row and column
    explicit SafeMatrix(int l_rowHigh, int l_colHigh)
            : rowLow(0), rowHigh(l_rowHigh), colLow(0), colHigh(l_colHigh) {
        if (rowHigh < 0 || colHigh < 0) {
            if (SAFEARRAY_SAFEMATRIX_DEBUG) {
                std::cout << "Constructor error: bounds definition" << std::endl;
            }
            exit(1);
        }
        // create array of SafeArray
        matrix = new SafeArray<T> * [rowHigh + 1];
        for (int row = 0; row <= rowHigh; row++) {
            matrix[row] = new SafeArray<T>(colHigh);
        }
    }

    // construct matrix with lower and upper bounds for row and column
    explicit SafeMatrix(int l_rowLow, int l_rowHigh, int l_colLow, int l_colHigh)
            : rowLow(l_rowLow), rowHigh(l_rowHigh), colLow(l_colLow), colHigh(l_colHigh) {
        if ((rowHigh - rowLow) < 0 || (colHigh - colLow) < 0) {
            if (SAFEARRAY_SAFEMATRIX_DEBUG) {
                std::cout << "Constructor error: bounds definition" << std::endl;
            }
            exit(1);
        }
        // create array of SafeArray
        matrix = new SafeArray<T> * [rowHigh - rowLow + 1];
        for (int row = 0; row <= (rowHigh - rowLow); row++) {
            matrix[row] = new SafeArray<T>(colLow, colHigh);
        }
    }

    // initializer_list constructor to allow "SafeMatrix<T> a{ { t00, t01 }, { t10, t11 } }"
    explicit SafeMatrix(const std::initializer_list<std::initializer_list<T>> & int_list)
            : rowLow(0), rowHigh(int_list.size() - 1), colLow(0), colHigh((* begin(int_list)).size() - 1) {
        int rows = rowHigh - rowLow + 1;
        matrix = new SafeArray<T> * [rows];
        auto it = begin(int_list);
        for (int row = 0; row < rows; row++) {
            if ((* it).size() - 1 != colHigh) {
                if (SAFEARRAY_SAFEMATRIX_DEBUG) {
                    std::cout << "Constructor error: bounds definition" << std::endl;
                }
                exit(1);
            }
            matrix[row] = new SafeArray<T>(* it);
            it++;
        }
    };

    // copy constructor
    SafeMatrix(const SafeMatrix<T> & l_SafeMatrix)
            : rowLow(l_SafeMatrix.rowLow), rowHigh(l_SafeMatrix.rowHigh), colLow(l_SafeMatrix.colLow), colHigh(l_SafeMatrix.colHigh) {
        int rows = rowHigh - rowLow + 1;
        matrix = new SafeArray<T> * [rows];
        for (int row = 0; row < rows; row++) {
            matrix[row] = new SafeArray<T>(* l_SafeMatrix.matrix[row]);
        }
    }

    ~ SafeMatrix() {
        delete[] matrix;
    }

    void fillMatrix(T element) {
        int rows = rowHigh - rowLow + 1;
        for (int row = 0; row < rows; row++) {
            matrix[row]->fillArray(element);
        }
    }

    // overload the [] operator to allow "a[row][column] = T();"
    SafeArray<T> & operator[](int index) {
        if (index < rowLow || index > rowHigh) {
            if (SAFEARRAY_SAFEMATRIX_DEBUG) {
                std::cout << "Index selector error: bounds selection " << index << " in " << rowLow << "-" << rowHigh
                          << std::endl;
            }
            exit(1);
        }
        return * matrix[index - colLow];
    }

    // overload the = operator to allow "SafeMatrix<T> a = b;"
    SafeMatrix<T> & operator=(const SafeMatrix<T> & l_SafeMatrix) {
        if (this == & l_SafeMatrix) return * this;
        rowLow = l_SafeMatrix.rowLow;
        rowHigh = l_SafeMatrix.rowHigh;
        colLow = l_SafeMatrix.colLow;
        colHigh = l_SafeMatrix.colHigh;
        int rows = rowHigh - rowLow + 1;
        delete[] matrix;
        matrix = new SafeArray<T> * [rows];
        for (int row = 0; row < rows; row++) {
            matrix[row] = l_SafeMatrix.matrix[row];
        }
        return * this;
    }

    SafeMatrix<T> & operator+(SafeMatrix<T> l_SafeMatrix) {
        // SafeMatrix a(x,y) = b(x,y) + c(m,n) if and only if x = m and y = n
        if ( rowHigh - rowLow != l_SafeMatrix.rowHigh - l_SafeMatrix.rowLow
            || colHigh - colLow != l_SafeMatrix.colHigh - l_SafeMatrix.colLow) {
            if (SAFEARRAY_SAFEMATRIX_DEBUG) {
                std::cout << "Arithmetic error: matrix addition "
                          << rowHigh - rowLow + 1 << ","
                          << colHigh - colLow + 1 << " "
                          << l_SafeMatrix.rowHigh - l_SafeMatrix.rowLow + 1 << ","
                          << l_SafeMatrix.colHigh - l_SafeMatrix.colLow + 1 << std::endl;
            }
            exit(1);
        }
        // create resulting matrix a(x,y)
        SafeMatrix * result = new SafeMatrix(* this);
        int m = l_SafeMatrix.rowLow;
        for (int x = this->rowLow; x <= this->rowHigh; x++){
            (* result)[x] = (* matrix[x]) + l_SafeMatrix[m];
        }
        return * result;
    }

    SafeMatrix<T> & operator-(SafeMatrix<T> l_SafeMatrix) {
        // SafeMatrix a(x,y) = b(x,y) - c(m,n) if and only if x = m and y = n
        if ( rowHigh - rowLow != l_SafeMatrix.rowHigh - l_SafeMatrix.rowLow
             || colHigh - colLow != l_SafeMatrix.colHigh - l_SafeMatrix.colLow) {
            if (SAFEARRAY_SAFEMATRIX_DEBUG) {
                std::cout << "Arithmetic error: matrix subtraction "
                          << rowHigh - rowLow + 1 << ","
                          << colHigh - colLow + 1 << " "
                          << l_SafeMatrix.rowHigh - l_SafeMatrix.rowLow + 1 << ","
                          << l_SafeMatrix.colHigh - l_SafeMatrix.colLow + 1 << std::endl;
            }
            exit(1);
        }
        // create resulting matrix a(x,y)
        SafeMatrix * result = new SafeMatrix(* this);
        int m = l_SafeMatrix.rowLow;
        for (int x = this->rowLow; x <= this->rowHigh; x++){
            (* result)[x] = (* matrix[x]) - l_SafeMatrix[m];
        }
        return * result;
    }

    SafeMatrix<T> & operator*(SafeMatrix<T> l_SafeMatrix) {
        // SafeMatrix a(x,m) = b(x,y) * c(m,n) if and only if y = m
        if (colHigh - colLow != l_SafeMatrix.rowHigh - l_SafeMatrix.rowLow) {
            if (SAFEARRAY_SAFEMATRIX_DEBUG) {
                std::cout << "Arithmetic error: matrix multiplication "
                          << colHigh - colLow + 1<< " " << l_SafeMatrix.rowHigh - l_SafeMatrix.rowLow + 1 << std::endl;
            }
            exit(1);
        }
        // create resulting matrix a(x,m)
        SafeMatrix * result = new SafeMatrix(rowLow, rowHigh, l_SafeMatrix.colLow, l_SafeMatrix.colHigh);
        int commonSize = colHigh - colLow + 1;
        // visit each index in matrix a
        for(int row = rowLow; row <= rowHigh; row++) {
            for (int column = l_SafeMatrix.colLow; column <= l_SafeMatrix.colHigh; column++) {
                T sum = T();
                // store sum of row column products into index
                for (int i = 0; i < commonSize; i++) {
                    sum += (* this)[row][colLow + i] * l_SafeMatrix[l_SafeMatrix.rowLow + i][column];
                }
                (* result)[row][column] = sum;
            }
        }
        return * result;
    }

    friend std::ostream & operator<<(std::ostream & l_ostream, const SafeMatrix<T> & l_SafeMatrix) {
        int size = l_SafeMatrix.rowHigh - l_SafeMatrix.rowLow + 1;
        for (int row = 0; row < size; row++) {
            l_ostream << * l_SafeMatrix.matrix[row] << std::endl;
        }
        return l_ostream;
    }

};

#endif //SAFEARRAY_SAFEMATRIX_H
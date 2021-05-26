/*
 * VNT(Very Neat Table) class is a m by n matrix such that the entries of each row are in sorted order from left to right
 * entries of each column are in sorted order from top(max) to bottom(min)
 */

#ifndef SAFEARRAY_VNT_H
#define SAFEARRAY_VNT_H
#define SAFEARRAY_VNT_DEBUG true

#include <cmath>
#include "SafeMatrix.h"

template <typename T>
class VNT {
public:
    T _MAX;
    SafeMatrix<T> * table;

public:

    VNT() {
        table = nullptr;
    }

    // creates a "n" by "n" table
    VNT(const int & n, const T & max) : _MAX(max) {
        table = new SafeMatrix<T>(n - 1);
        table->fillMatrix(_MAX);
    }

    // creates a "m" by "n" table
    VNT(const int & m, const int & n, const T & max) : _MAX(max) {
        table = new SafeMatrix<T>(m - 1, n - 1);
        table->fillMatrix(_MAX);
    }

    VNT(const VNT<T> & l_VNT) {
        _MAX = l_VNT._MAX;
        table = new SafeMatrix<T>(* l_VNT.table);
    }

    ~VNT() { delete table; }

    // adds an "element" to the matrix
    void add(T element) {
        int row = table->rowHigh;
        int col = table->colHigh;
        if ((* table)[row][col] != _MAX) {
            if (SAFEARRAY_VNT_DEBUG) {
                std::cout << "insert error: table full " << element << std::endl;
            }
            return;
        } else if (element > _MAX) {
            if (SAFEARRAY_VNT_DEBUG) {
                std::cout << "insert error: value limit " << element << std::endl;
            }
            return;
        } else (* table)[row][col] = element;

        while(row > 0 || col > 0) {
            if (row > 0 && (* table)[row - 1][col] == _MAX) {
                (* table)[row - 1][col] = element;
                (* table)[row][col] = _MAX;
                row--;
            } else if (col > 0 && (* table)[row][col - 1] == _MAX) {
                (* table)[row][col - 1] = element;
                (* table)[row][col] = _MAX;
                col--;
            } else {
                int left = 0;
                int top = 0;
                if (col > 0) {
                    left += (* table)[row][col - 1];
                }
                if (row > 0) {
                    top += (* table)[row - 1][col];
                }
                if (top >= left && (* table)[row - 1][col] > element) {
                    T temp = (* table)[row - 1][col];
                    (* table)[row - 1][col] = element;
                    (* table)[row][col] = temp;
                    row--;
                    continue;
                } else if (left > top && (* table)[row][col - 1] > element) {
                    T temp = (* table)[row][col - 1];
                    (* table)[row][col - 1] = element;
                    (* table)[row][col] = temp;
                    col--;
                    continue;
                }
                break;
            }
        }
    }

    T getMin() {
        return (* table)[0][0];
    }

    VNT<T> sort(T squareMatrix[], int size) {
        int n = sqrt(size);
        delete table;
        table = new SafeMatrix<T>(n - 1);
        table->fillMatrix(_MAX);
        for (int index = 0; index < size; index++) {
            add(squareMatrix[index]);
        }
        return * this;
    }

    bool find(T element) {
        int lRow = table->rowHigh;
        int lCol = 0;
        int rRow = 0;
        int rCol = table->colHigh;
        bool run = true;
        while (run) {
            if ((* table)[lRow][lCol] == element) return true;
            if ((* table)[rRow][rCol] == element) return true;
            if (lCol < table->colHigh && (* table)[lRow][lCol + 1] <= element) {
                lCol++;
                run = !run;
            } else if (lRow > 0) {
                lRow--;
                run = !run;
            }
            if (rRow < table->rowHigh && (* table)[rRow + 1][rCol] <= element) {
                rRow++;
                run = !run;
            } else if (rCol > 0) {
                rCol--;
                run = !run;
            }
        }
        return false;
    }

    VNT<T> & operator=(const VNT<T> & l_t) {
        if (this == & l_t) return * this;
        _MAX = l_t._MAX;
        delete table;
        table = new SafeMatrix<T>(* l_t.table);
        return * this;
    }

    friend std::ostream & operator<<(std::ostream & l_ostream, const VNT<T> & l_vnt) {
        for (int row = 0; row <= l_vnt.table->rowHigh; row++) {
            for (int col = 0; col <= l_vnt.table->colHigh; col++) {
                if ((* l_vnt.table)[row][col] == l_vnt._MAX) l_ostream << "--\t";
                else l_ostream << (* l_vnt.table)[row][col] << "\t";
            }
            l_ostream << std::endl;
        }
        return l_ostream;
    }

};

#endif //SAFEARRAY_VNT_H

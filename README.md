# SafeMatrix

Project is for CSCI381 Advanced OOP in C++

Additional functionality added to project for a more robust class

## Highlights

## Requirements for SafeMatrix
* Implement a templated 2 dimentional safe array class (i.e. matrix class)
* Upper and lower bounds specifiable on each dimention
* Test SafeMatrix class by using the class for matrix multiplication

## Requirements for Block
* Implement a custom memory allocator for 1 dimential safe array class
* Details for custom memory allocator
  1.   Contiguous allocation
  2.   Next Fit
  3.   Variable partition
  4.   Immediate coalescing
  5.   Explicit free list
  6.   LIFO (last in first out)
  7.   Static memory pool


## Requirements for VNT (Very Neat Table)

Test your Matrix class by using it to implement a VNT class

VNT class is a m by n matrix such that
* Entries of each row are in sorted order from left(min) to right(max)
* Entries of each column are in sorted order from top(min) to bottom(max)
* Some of the entries of a VNT may be INT_MAX, which we treat as nonexistent elements

Thus, a VNT can be used to hold r ≤ mn integers

If A is a VNT object then A[ m ][ n ] is the m, n th element of the underlying 2 dimensional Matrix

So if A is empty then A[ 0 ][ 0 ] = INT_MAX and if A is full then A[ m - 1 ][ n - 1 ] < INT_MAX

1. VNT(int m, int n) will create an m x n VNT object 

    e.g. `VNT A(5,7);`

2. `A.add(25);` will add 25 to a non-full VNT

3. `A.getMin();` will extract the smallest element from A and leave A a VNT

4. `A.sort(int k[], int size)` sort the n x n numbers in k using A and not calling any sort routine as a subroutine

5. `A.find(int i)` will return true if i is in A and false otherwise

#### Constraints

* function `add()` should work in time proportional to m + n
* function `getMin()` should work in time proportional to m + n
* function `sort()` should work in time proportional to n^3
* function `find()` should work in time proportional to m + n

## Sample Output

## Sameple Debug Logs

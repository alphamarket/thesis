//#include "inc/stdafx.hpp"
#include "inc/matrix.hpp"
#include <iostream>
#include <typeinfo>
using namespace std;


/**
 * @brief main The main entry of program
 * @return The exit flag
 */
int main(int, char**) {
    // Create a 3D array that is 3 x 4 x 2
    matrix1D_t<size_t> A({2});
    A[0] = 3;
    A[1] = 4;
    auto B = A;
    B[0] = 13;
    B[1] = 14;
    cout << "MIN: " << A.min() << endl;
    cout << "MAX: " << A.max() << endl;
    cout << "NORM: " << A.norm2() << endl;
    cout << "NORMALIZE: "<< A.normalize() << endl;
    cout << typeid(A[indices[A.range(0, 2)]]).name() << endl;
    matrix<size_t, 2> K = A[indices[A.range(0, 2)]];
    for(auto x : A[indices[A.range(0, 2)]]) {
        cout << x << " ";
    }
    cout << endl;
    cout << B.as<double>() / 2 + A;

    cout <<"DONE"<<endl;
    exit(EXIT_SUCCESS);
}

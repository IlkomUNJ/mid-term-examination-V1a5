#ifndef CUSTOMMATRIX_H
#define CUSTOMMATRIX_H

#include <iostream>

using namespace std;

class CustomMatrix{
public:
    bool mat[3][3]={0};
    CustomMatrix(){

    }

    //alternate way to instantiate the class
    CustomMatrix(bool m[3][3]){
        // Menggunakan loop eksplisit untuk mencegah masalah ukuran array
        for (int i=0;i< 3;i++) {
            for(int j=0; j < 3;j++){
                mat[i][j] = m[i][j];
            }
        }
    }

    void fillMatrix(bool m[3][3]){
        // Menggunakan loop eksplisit untuk mencegah masalah ukuran array
        for (int i=0;i< 3;i++) {
            for(int j=0; j < 3;j++){
                mat[i][j] = m[i][j];
            }
        }
    }

    bool isEmpty() const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (mat[i][j]) {
                    return false; // Ada piksel bukan-putih
                }
            }
        }
        return true; // Semua piksel putih
    }

    void printMatrix() const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cout << (mat[i][j] ? "1" : "0") << " ";
            }
            cout << endl;
        }
    }
};

#endif // CUSTOMMATRIX_H

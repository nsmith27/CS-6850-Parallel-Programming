#include "immintrin.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <random>
#include <string>
#include <vector>
#include <chrono>

using namespace std;



template <typename T>
void bubbleSort(int size, T * A, bool rev);

template <typename T> 
void aShuffle(int size, T * A );

template <typename T>
void aprint(int size, T * A);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Primary Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Bubble Sort 
template <typename T>
void bubbleSort(int size, T * A, bool rev){
    for (int i = 0; i < size; i++){
        int swaps = 0;
        for (int k =  0; k < size-1; k++){
            if ( (A[k] > A[k+1]) != (rev) ){
                T temp = A[k];
                A[k] = A[k+1];
                A[k+1] = temp;
                swaps += 1;  
            }
        }
        if (swaps == 0){
            return;  
        }
    }
    return;
}


// Takes in a pointer to 32 ints (32bit each) 
void bitonicSort(int * A, int * B) {
    bubbleSort(16, A, 0);
    bubbleSort(16, B, 1);

    aprint(16, A);
    aprint(16, B);


    // __m512i reverse = _mm512_set_epi32(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);

    // __m512i idx_L1= _mm512_set_epi32(23, 22, 21, 20, 19, 18, 17, 16, 7, 6, 5, 4, 3, 2, 1, 0);
    // __m512i idx_H2= _mm512_set_epi32(31, 30, 29, 28, 27, 26, 25, 24, 15, 14, 13, 12, 11, 10, 9, 8);
    // __m512i idx_L3= _mm512_set_epi32(27, 26, 25, 24, 11, 10, 9, 8, 19, 18, 17, 16, 3, 2, 1, 0);
    // __m512i idx_H4= _mm512_set_epi32(31, 30, 29, 28, 15, 14, 13, 12, 23, 22, 21, 20, 7, 6, 5, 4);
    // __m512i idx_L5= _mm512_set_epi32(29, 28, 13, 12, 25, 24, 9, 8, 21, 20, 5, 4, 17, 16, 1, 0);
    // __m512i idx_H6= _mm512_set_epi32(31, 30, 15, 14, 27, 26, 11, 10, 23, 22, 7, 6, 19, 18, 3, 2);
    // __m512i idx_L7= _mm512_set_epi32(30, 14, 28, 12, 26, 10, 24, 8, 22, 6, 20, 4, 18, 2, 16, 0);
    // __m512i idx_H8= _mm512_set_epi32(31, 15, 29, 13, 27, 11, 25, 9, 23, 7, 21, 5, 19, 3, 17, 1);
    // __m512i idx_L9= _mm512_set_epi32(23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0);
    // __m512i idx_H10= _mm512_set_epi32(31, 15, 30, 14, 29, 13, 28, 12, 27, 11, 26, 10, 25, 9, 24, 8);


}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //  Helper Functions
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////

// // // Code to help print an __m512 vector:
// void printVectorInt(__m512i v, string name) {
//     #if defined (__GNUC__)
//     int* temp = (int*)aligned_alloc(64, sizeof(int) * 16);
//     #elif defined (_MSC_VER)
//     int* temp = (int*)_aligned_malloc(sizeof(int) * 16, 64);
//     #endif

//     _mm512_store_si512(temp, v);
//     printf("The vector called %s contains: ", name.c_str());
   
//     for (int i = 0; i < 16; i++) {
//         printf("%02d ", temp[i]);   
//     }
//     printf("\n");
   
//     #if defined (__GNUC__)
//     free(temp);
//     #elif defined (_MSC_VER)
//     _aligned_free(temp);
//     #endif
   
// }
// void printVectorFloat(__m512 v, string name) {
//     #if defined (__GNUC__)
//     float* temp = (float*)aligned_alloc(64, sizeof(float) * 16);
//     #elif defined (_MSC_VER)
//     float* temp = (float*)_aligned_malloc(sizeof(float) * 16, 64);
//     #endif

//     _mm512_store_ps(temp, v);
//     printf("The vector called %s contains: ", name.c_str());
    
//     for (int i = 0; i < 16; i++) {
//         printf("%3f ", temp[i]);
//     }
//     printf("\n");

//     #if defined (__GNUC__)
//     free(temp);
//     #elif defined (_MSC_VER)
//     _aligned_free(temp);
//     #endif
// }

// Print array
template <typename T>
void aprint(int size, T * A){
    cout << size << "\t";  
    for (int i = 0; i < size; i++) {
        cout << A[i];
        if (i+1 != size ){
            cout << " - " ;
        }
    }
    cout << endl; 
    return;  
}

// Randomize elements of a vector
template <typename T> 
void aShuffle(int size, T * A ){
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

    for (int i = 0; i < size; i++){
        std::uniform_int_distribution<int> rand_elem(0, i);
        int i2 = rand_elem(gen);
        T temp = A[i];
        A[i] = A[i2];
        A[i2] = temp;  
    }
    return;  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){


    cout << "Hi there! " << endl << endl;  

    int n = 32;
    int * test = (int*)aligned_alloc(64, sizeof(int) * n);
    // int * test = new int[n];
    for (int i = 0; i < n; i++){
        test[i] = i; 
    }


    // vector<int> test{2,4,15,6,4,3};
    aShuffle(n, test);
    aprint(n, test); 

    bitonicSort(test, test+16);  
    // bubbleSort(n, test, 1);  
    // aprint(n, test); 
    // bubbleSort(n, test, 0);  
    // aprint(n, test);


    return 0;
}







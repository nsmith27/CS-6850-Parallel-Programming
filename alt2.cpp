#include "immintrin.h"
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <queue>  

using namespace std;



template <typename T>
bool isSorted(int size, T * A);

template <typename T>
void sort16(int size, T * A);

template <typename T>
void selectionSort(int size, T * A);

template <typename T>
void bubbleSort(int size, T * A);

template <typename T>
void getRandomArray(int size, T * A);

template <typename T> 
void aShuffle(int size, T * A );

template <typename T>
void aprint(int size, T * A);

int * getCopyArr(int n, int * start);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Code to help print an __m512 vector:
void printVectorInt(__m512i v, string name) {
    #if defined (__GNUC__)
    int* temp = (int*)aligned_alloc(64, sizeof(int) * 16);
    #elif defined (_MSC_VER)
    int* temp = (int*)_aligned_malloc(sizeof(int) * 16, 64);
    #endif

    _mm512_store_si512(temp, v);
    printf("The vector called %s contains: ", name.c_str());

    for (int i = 0; i < 16; i++) {
        printf("%02d ", temp[i]);   
    }
    printf("\n");

    #if defined (__GNUC__)
    free(temp);
    #elif defined (_MSC_VER)
    _aligned_free(temp);
    #endif

}

// Print array
template <typename T>
void aprint(int size, T * A){
    cout << size << "\t";  
    for (int i = 0; i < size; i++) {
        if (A[i] < 10){
            cout << "0"; 
        }
        cout << A[i];
        if (i+1 != size ){
            if (i % 16 == 15){
                cout << " +|+";  
        }                }
        cout << " ";  
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

int * getCopyArr(int n, int * start){
    int * result = (int*)aligned_alloc(64, sizeof(int) * n);
    for (int i = 0; i < n; i++){
        result[i] = start[i];
    }
    return result;  
}

template <typename T>
void getRandomArray(int size, T * A){
    for (int i = 0; i < size; i++){
        A[i] = i; 
    }
    aShuffle(size, A);
    if (size <= 256){
        aprint(size, A); 
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Primary Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isSorted
template <typename T>
bool isSorted(int size, T * A){
    for (int i = 0; i < size-1; i++){
        if (A[i] > A[i+1])
        return false;
    }
    return true;  
}

// Sort 16
template <typename T>
void sort16(int size, T * A){
    for (int i = 0; i < size; i+=16){
        selectionSort(16,A+i);
        cout << endl; 
        aprint(16, A+i);
        if (!isSorted(16, A+i)){
            cout << "Not Sorted!!!" << endl;  
        }
    }
    return;
}

// Selection Sort
template <typename T>
void selectionSort(int size, T * A){
    int swaps;
    for (int i = 0; i < size-1; i++){
        int min = i;
        for (int j = i+1; j < size; j++){
            if (A[j] < A[min]){
                min = j;
            }
        }
        T temp = A[i];
        A[i] = A[min];
        A[min] = temp;   
    }
    return;  
}

// Bubble Sort 
template <typename T>
void bubbleSort(int size, T * A){
    int swaps;
    do {
        swaps = 0;
        for (int k = 0; k < size-1; k++){
            if ( A[k] > A[k+1] ) {
                T temp = A[k];
                A[k] = A[k+1];
                A[k+1] = temp;
                swaps += 1;  
            }
        }
    } while (swaps);

    return;
}


// Takes in two 16 bit pointers
void bitonicSort(__m512i &A1, __m512i &A2, __m512i &A1out, __m512i &A2out) {
    cout << endl << "bitonicSort() CALLED! " << endl << endl;  

    printVectorInt(A1, "A1");
    printVectorInt(A2, "A2");


    __m512i LA;
    __m512i HA;

    __m512i reverse = _mm512_set_epi32(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    __m512i idx_L1 = _mm512_set_epi32(23, 22, 21, 20, 19, 18, 17, 16, 7, 6, 5, 4, 3, 2, 1, 0);
    __m512i idx_H1 = _mm512_set_epi32(31, 30, 29, 28, 27, 26, 25, 24, 15, 14, 13, 12, 11, 10, 9, 8);
    __m512i idx_L2 = _mm512_set_epi32(27, 26, 25, 24, 11, 10, 9, 8, 19, 18, 17, 16, 3, 2, 1, 0);
    __m512i idx_H2 = _mm512_set_epi32(31, 30, 29, 28, 15, 14, 13, 12, 23, 22, 21, 20, 7, 6, 5, 4);
    __m512i idx_L3 = _mm512_set_epi32(29, 28, 13, 12, 25, 24, 9, 8, 21, 20, 5, 4, 17, 16, 1, 0);
    __m512i idx_H3 = _mm512_set_epi32(31, 30, 15, 14, 27, 26, 11, 10, 23, 22, 7, 6, 19, 18, 3, 2);
    __m512i idx_L4 = _mm512_set_epi32(30, 14, 28, 12, 26, 10, 24, 8, 22, 6, 20, 4, 18, 2, 16, 0);
    __m512i idx_H4 = _mm512_set_epi32(31, 15, 29, 13, 27, 11, 25, 9, 23, 7, 21, 5, 19, 3, 17, 1);
    __m512i idx_L5 = _mm512_set_epi32(23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0);
    __m512i idx_H5 = _mm512_set_epi32(31, 15, 30, 14, 29, 13, 28, 12, 27, 11, 26, 10, 25, 9, 24, 8);

    // Reverse Step
    A2 = _mm512_permutexvar_epi32(reverse, A2);

    // L1
    LA = _mm512_min_epi32(A1, A2);
    HA = _mm512_max_epi32(A1, A2);
    A1 = _mm512_permutex2var_epi32(LA, idx_L1, HA);
    A2 = _mm512_permutex2var_epi32(LA, idx_H1, HA);

    // L2
    LA = _mm512_min_epi32(A1, A2);
    HA = _mm512_max_epi32(A1, A2);
    A1 = _mm512_permutex2var_epi32(LA, idx_L2, HA);
    A2 = _mm512_permutex2var_epi32(LA, idx_H2, HA);

    // L3
    LA = _mm512_min_epi32(A1, A2);
    HA = _mm512_max_epi32(A1, A2);
    A1 = _mm512_permutex2var_epi32(LA, idx_L3, HA);
    A2 = _mm512_permutex2var_epi32(LA, idx_H3, HA);

    // L4
    LA = _mm512_min_epi32(A1, A2);
    HA = _mm512_max_epi32(A1, A2);
    A1 = _mm512_permutex2var_epi32(LA, idx_L4, HA);
    A2 = _mm512_permutex2var_epi32(LA, idx_H4, HA);

    // L5
    LA = _mm512_min_epi32(A1, A2);
    HA = _mm512_max_epi32(A1, A2);
    A1out = _mm512_permutex2var_epi32(LA, idx_L5, HA);
    A2out = _mm512_permutex2var_epi32(LA, idx_H5, HA);

    printVectorInt(A1out, "A1out");
    printVectorInt(A2out, "A2out");


    return;  
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){


    //      |..SEG...SEG...SEG...SEG..|..........BLOCK..........|..........BLOCK..........|..........BLOCK..........|
    //       <--------           THREAD WORK SIZE               -------->
    //
    // Several segments make a block, and 1 or 4 blocks make a thread_work_size, until the whole thing is sorted.  
    // Segments are 2xsorted_block_size long, and are spaced at regular intervals 
    cout << "Program Start..... " << endl << endl;  
    int n = 256;
    int * Arr = (int*)aligned_alloc(64, sizeof(int) * n);
    getRandomArray(n, Arr);
    aprint(n, Arr);   



    // Each thread works on a thread_work_size amount of data
    int thread_work_size = n;
    int end_block_size = thread_work_size / 1;
    sort16(n, Arr);  
    int sorted_block_size = 16;
    
    int thread_work_index = 0;
    int thread_end_index = thread_work_index + n;  

    int * Arr_read = &Arr[thread_work_index];
    int * Arr_write = (int*)aligned_alloc(64, sizeof(int) * n);

    cout << endl << "-------------------------------------------" << endl << endl;  

    // while (sortedBlockSize < endBlockSize){


        for (int segment = 0; segment < thread_work_size; segment += sorted_block_size * 2){
            cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << segment << endl;  
            int startA1 = segment;
            int endA1 = startA1 + sorted_block_size;
            int startA2 = endA1;
            int endA2 = startA2 + sorted_block_size;

            __m512i A1 = _mm512_load_si512(&Arr_read[startA1]); 
            __m512i A2 = _mm512_load_si512(&Arr_read[startA2]);
            __m512i A1out;  
            __m512i A2out;
            for (int i = 0; i < (sorted_block_size/8)-1; i++){
                cout << i+1 << " -- ";  
                bitonicSort(A1, A2, A1out, A2out);
                cout << startA1 << ", " << startA2 << endl;  
                _mm512_store_si512(&Arr_write[segment + i*16], A1out);
                A1 = A2out; 
                if (i == ((sorted_block_size/8)-2) ){
                    cout << "Last Iteration" << endl;  
                    _mm512_store_si512(&Arr_write[segment + i*16+16], A2out);
                }
                else if (startA1+16 == endA1) {
                    cout << "If 1" << endl;  
                    // Finished 1’s side but not 2’s side
                    startA2 += 16;
                    A2 = _mm512_load_si512(&Arr_read[startA2]);
                } 
                else if (startA2+16 == endA2) {
                    cout << "If 2" << endl;  
                    // Finished 2's side but not 1’s side
                    startA1 += 16;
                    A2 = _mm512_load_si512(&Arr_read[startA1]);
                } 
                else if (Arr_read[startA1 + 16] < Arr_read[startA2 + 16] ){
                    cout << "If 3" << endl;  
                    // use A1’s value
                    startA1 += 16;
                    A2 = _mm512_load_si512(&Arr_read[startA1]); 
                } 
                else if (Arr_read[startA1 + 16] >= Arr_read[startA2 + 16] ){
                    cout << "If 4" << endl;  
                    // Store A2's source and swap A1's source with A2's source 
                    startA2 += 16;
                    A2 = _mm512_load_si512(&Arr_read[startA2]); 
                }
                cout << startA1 << ", " << startA2 << endl;  
                aprint(128, Arr_write);
                aprint(128, Arr_write+128);
            }
        }
    // }

    return 0;
}
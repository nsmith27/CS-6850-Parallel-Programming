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

#include <unistd.h>
#include <omp.h>
#include <cstdio>
#define N 33'554'432 // 2^25

using namespace std;



template <typename T>
bool isSorted(int size, T * A);

template <typename T>
bool areBlocksSorted(int size, T * A, int ILP);

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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
    std::random_device rd;  // Required to obtain seed from random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

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
        // aprint(size, A); 
    }
    return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Primary Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// isSorted
template <typename T>
bool isSorted(int size, T * A){
    for (int i = 0; i < size-1; i++){
        if (A[i] > A[i+1]){
            return false;
        }
    }
    return true;  
}

// areBlocksSorted
template <typename T>
bool areBlocksSorted(int size, T * A, int ILP){
    static int thread_work_chunk = 1;
    bool sorted = true; 
    int block_size = size / ILP;
    for (int i = 0; i < ILP; i++){
        if ( !isSorted(block_size, &A[i * block_size]) ){
            cout << "Failed to sort Block " << i << " in work group " << thread_work_chunk << "!" << endl;  
            // aprint(block_size, &A[i * block_size]);  
            sorted = false;
        }
    }
    if (sorted){
        // cout << "Success! \t Work Group: " << thread_work_chunk << endl;  
    }
    thread_work_chunk += 1;  
    return sorted;  
}


// Sort 16
template <typename T>
void sort16(int size, T * A){
    for (int i = 0; i < size; i+=16){
        selectionSort(16,A+i);
        // cout << endl; 
        // aprint(16, A+i);
        // if (!isSorted(16, A+i)){
        //     cout << "16 Elements Not Sorted!!!" << endl;  
        // }
    }
    return;
}

// Selection Sort
template <typename T>
void selectionSort(int size, T * A){
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
void bitonicSort(__m512i &A1, __m512i &A2, __m512i &A1out, __m512i &A2out,
                 __m512i &B1, __m512i &B2, __m512i &B1out, __m512i &B2out,
                 __m512i &C1, __m512i &C2, __m512i &C1out, __m512i &C2out,
                 __m512i &D1, __m512i &D2, __m512i &D1out, __m512i &D2out) {

    __m512i LA;
    __m512i LB;
    __m512i LC;
    __m512i LD;

    __m512i HA;
    __m512i HB;
    __m512i HC;
    __m512i HD;

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
    B2 = _mm512_permutexvar_epi32(reverse, B2);
    C2 = _mm512_permutexvar_epi32(reverse, C2);
    D2 = _mm512_permutexvar_epi32(reverse, D2);

    // L1
    LA = _mm512_min_epi32(A1, A2);
    LB = _mm512_min_epi32(B1, B2);
    LC = _mm512_min_epi32(C1, C2);
    LD = _mm512_min_epi32(D1, D2);
    
    HA = _mm512_max_epi32(A1, A2);
    HB = _mm512_max_epi32(B1, B2);
    HC = _mm512_max_epi32(C1, C2);
    HD = _mm512_max_epi32(D1, D2);
    
    A1 = _mm512_permutex2var_epi32(LA, idx_L1, HA);
    B1 = _mm512_permutex2var_epi32(LB, idx_L1, HB);
    C1 = _mm512_permutex2var_epi32(LC, idx_L1, HC);
    D1 = _mm512_permutex2var_epi32(LD, idx_L1, HD);
    
    A2 = _mm512_permutex2var_epi32(LA, idx_H1, HA);
    B2 = _mm512_permutex2var_epi32(LB, idx_H1, HB);
    C2 = _mm512_permutex2var_epi32(LC, idx_H1, HC);
    D2 = _mm512_permutex2var_epi32(LD, idx_H1, HD);

    // L2
    LA = _mm512_min_epi32(A1, A2);
    LB = _mm512_min_epi32(B1, B2);
    LC = _mm512_min_epi32(C1, C2);
    LD = _mm512_min_epi32(D1, D2);
    
    HA = _mm512_max_epi32(A1, A2);
    HB = _mm512_max_epi32(B1, B2);
    HC = _mm512_max_epi32(C1, C2);
    HD = _mm512_max_epi32(D1, D2);
    
    A1 = _mm512_permutex2var_epi32(LA, idx_L2, HA);
    B1 = _mm512_permutex2var_epi32(LB, idx_L2, HB);
    C1 = _mm512_permutex2var_epi32(LC, idx_L2, HC);
    D1 = _mm512_permutex2var_epi32(LD, idx_L2, HD);
    
    A2 = _mm512_permutex2var_epi32(LA, idx_H2, HA);
    B2 = _mm512_permutex2var_epi32(LB, idx_H2, HB);
    C2 = _mm512_permutex2var_epi32(LC, idx_H2, HC);
    D2 = _mm512_permutex2var_epi32(LD, idx_H2, HD);

    // L3
    LA = _mm512_min_epi32(A1, A2);
    LB = _mm512_min_epi32(B1, B2);
    LC = _mm512_min_epi32(C1, C2);
    LD = _mm512_min_epi32(D1, D2);
    
    HA = _mm512_max_epi32(A1, A2);
    HB = _mm512_max_epi32(B1, B2);
    HC = _mm512_max_epi32(C1, C2);
    HD = _mm512_max_epi32(D1, D2);
    
    A1 = _mm512_permutex2var_epi32(LA, idx_L3, HA);
    B1 = _mm512_permutex2var_epi32(LB, idx_L3, HB);
    C1 = _mm512_permutex2var_epi32(LC, idx_L3, HC);
    D1 = _mm512_permutex2var_epi32(LD, idx_L3, HD);
    
    A2 = _mm512_permutex2var_epi32(LA, idx_H3, HA);
    B2 = _mm512_permutex2var_epi32(LB, idx_H3, HB);
    C2 = _mm512_permutex2var_epi32(LC, idx_H3, HC);
    D2 = _mm512_permutex2var_epi32(LD, idx_H3, HD);

    // L4
    LA = _mm512_min_epi32(A1, A2);
    LB = _mm512_min_epi32(B1, B2);
    LC = _mm512_min_epi32(C1, C2);
    LD = _mm512_min_epi32(D1, D2);
    
    HA = _mm512_max_epi32(A1, A2);
    HB = _mm512_max_epi32(B1, B2);
    HC = _mm512_max_epi32(C1, C2);
    HD = _mm512_max_epi32(D1, D2);
    
    A1 = _mm512_permutex2var_epi32(LA, idx_L4, HA);
    B1 = _mm512_permutex2var_epi32(LB, idx_L4, HB);
    C1 = _mm512_permutex2var_epi32(LC, idx_L4, HC);
    D1 = _mm512_permutex2var_epi32(LD, idx_L4, HD);
    
    A2 = _mm512_permutex2var_epi32(LA, idx_H4, HA);
    B2 = _mm512_permutex2var_epi32(LB, idx_H4, HB);
    C2 = _mm512_permutex2var_epi32(LC, idx_H4, HC);
    D2 = _mm512_permutex2var_epi32(LD, idx_H4, HD);

    // L5
    LA = _mm512_min_epi32(A1, A2);
    LB = _mm512_min_epi32(B1, B2);
    LC = _mm512_min_epi32(C1, C2);
    LD = _mm512_min_epi32(D1, D2);
    
    HA = _mm512_max_epi32(A1, A2);
    HB = _mm512_max_epi32(B1, B2);
    HC = _mm512_max_epi32(C1, C2);
    HD = _mm512_max_epi32(D1, D2);

    A1out = _mm512_permutex2var_epi32(LA, idx_L5, HA);
    B1out = _mm512_permutex2var_epi32(LB, idx_L5, HB);
    C1out = _mm512_permutex2var_epi32(LC, idx_L5, HC);
    D1out = _mm512_permutex2var_epi32(LD, idx_L5, HD);
    
    A2out = _mm512_permutex2var_epi32(LA, idx_H5, HA);
    B2out = _mm512_permutex2var_epi32(LB, idx_H5, HB);
    C2out = _mm512_permutex2var_epi32(LC, idx_H5, HC);
    D2out = _mm512_permutex2var_epi32(LD, idx_H5, HD);

    return;  
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //      |..SEG...SEG...SEG...SEG..|..........BLOCK..........|..........BLOCK..........|..........BLOCK..........|
    //      <-------------------------------------<      THREAD WORK SIZE      >------------------------------------>
    //
    //
    // Each segment length = 2 * sorted_block_size 
    // variables segentA + segmentB + segmentC + segmentD = Block
    // When the while loop has finished, 4 blocks make a thread_work_size.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    cout << endl << "-------------------------------------------" << endl;  
    cout << "Program Start..... " << endl;  
    cout << "-------------------------------------------" << endl << endl;  
    int ILP = 4;  
    int TOTAL_SIZE = pow(2, 25);
    // TOTAL_SIZE = 256*4;
    int thread_work_size = 65536;
    int * Arr = (int*)aligned_alloc(64, sizeof(int) * TOTAL_SIZE);
    getRandomArray(TOTAL_SIZE, Arr);

    auto t0 = std::chrono::steady_clock::now();
    // Each thread works on a thread_work_size amount of data
    #pragma omp parallel for schedule(dynamic) num_threads(64)
    for (unsigned int thread_work_index = 0; thread_work_index < TOTAL_SIZE; thread_work_index += thread_work_size) {
        sort16(thread_work_size, &Arr[thread_work_index]);  
        int sorted_block_size = 16;
        
        int * Arr_read = &Arr[thread_work_index];
        int * Arr_write = (int*)aligned_alloc(64, sizeof(int) * thread_work_size);

        while (sorted_block_size < thread_work_size/4){
            for (int segmentA = 0; segmentA < thread_work_size; segmentA += sorted_block_size * 8){
                int segmentB = segmentA + sorted_block_size * 2;
                int segmentC = segmentB + sorted_block_size * 2;
                int segmentD = segmentC + sorted_block_size * 2;
                int startA1 = segmentA;
                int startB1 = segmentB;
                int startC1 = segmentC;
                int startD1 = segmentD;
                int endA1 = startA1 + sorted_block_size;
                int endB1 = startB1 + sorted_block_size;
                int endC1 = startC1 + sorted_block_size;
                int endD1 = startD1 + sorted_block_size;
                int startA2 = endA1;
                int startB2 = endB1;
                int startC2 = endC1;
                int startD2 = endD1;
                int endA2 = startA2 + sorted_block_size;
                int endB2 = startB2 + sorted_block_size;
                int endC2 = startC2 + sorted_block_size;
                int endD2 = startD2 + sorted_block_size;

                __m512i A1 = _mm512_load_si512(&Arr_read[startA1]); 
                __m512i B1 = _mm512_load_si512(&Arr_read[startB1]); 
                __m512i C1 = _mm512_load_si512(&Arr_read[startC1]); 
                __m512i D1 = _mm512_load_si512(&Arr_read[startD1]); 
                __m512i A2 = _mm512_load_si512(&Arr_read[startA2]);
                __m512i B2 = _mm512_load_si512(&Arr_read[startB2]);
                __m512i C2 = _mm512_load_si512(&Arr_read[startC2]);
                __m512i D2 = _mm512_load_si512(&Arr_read[startD2]);
                __m512i A1out;  
                __m512i B1out;  
                __m512i C1out;  
                __m512i D1out;  
                __m512i A2out;
                __m512i B2out;
                __m512i C2out;
                __m512i D2out;
                
                for (int i = 0; i < (sorted_block_size/8)-1; i++){
                    bitonicSort(A1, A2, A1out, A2out,
                                B1, B2, B1out, B2out,
                                C1, C2, C1out, C2out,
                                D1, D2, D1out, D2out);
                    _mm512_store_si512(&Arr_write[segmentA + i*16], A1out);
                    _mm512_store_si512(&Arr_write[segmentB + i*16], B1out);
                    _mm512_store_si512(&Arr_write[segmentC + i*16], C1out);
                    _mm512_store_si512(&Arr_write[segmentD + i*16], D1out);
                    A1 = A2out; 
                    B1 = B2out; 
                    C1 = C2out; 
                    D1 = D2out; 
                    //////////////////////////////// -- A -- ////////////////////////////////
                    if (i == ((sorted_block_size/8)-2) ){
                        _mm512_store_si512(&Arr_write[segmentA + i*16+16], A2out);
                        _mm512_store_si512(&Arr_write[segmentB + i*16+16], B2out);
                        _mm512_store_si512(&Arr_write[segmentC + i*16+16], C2out);
                        _mm512_store_si512(&Arr_write[segmentD + i*16+16], D2out);
                    }
                    if (startA1+16 == endA1) {
                        // Finished 1’s side but not 2’s side
                        startA2 += 16;
                        A2 = _mm512_load_si512(&Arr_read[startA2]);
                    } 
                    else if (startA2+16 == endA2) {
                        // Finished 2's side but not 1’s side
                        startA1 += 16;
                        A2 = _mm512_load_si512(&Arr_read[startA1]);
                    } 
                    else if (Arr_read[startA1 + 16] < Arr_read[startA2 + 16] ){
                        // use A1’s value
                        startA1 += 16;
                        A2 = _mm512_load_si512(&Arr_read[startA1]); 
                    } 
                    else if (Arr_read[startA1 + 16] >= Arr_read[startA2 + 16] ){
                        // Store A2's source and swap A1's source with A2's source 
                        startA2 += 16;
                        A2 = _mm512_load_si512(&Arr_read[startA2]); 
                    }
                    //////////////////////////////// -- B -- ////////////////////////////////
                    if (startB1+16 == endB1) {
                        // Finished 1’s side but not 2’s side
                        startB2 += 16;
                        B2 = _mm512_load_si512(&Arr_read[startB2]);
                    } 
                    else if (startB2+16 == endB2) {
                        // Finished 2's side but not 1’s side
                        startB1 += 16;
                        B2 = _mm512_load_si512(&Arr_read[startB1]);
                    } 
                    else if (Arr_read[startB1 + 16] < Arr_read[startB2 + 16] ){
                        // use A1’s value
                        startB1 += 16;
                        B2 = _mm512_load_si512(&Arr_read[startB1]); 
                    } 
                    else if (Arr_read[startB1 + 16] >= Arr_read[startB2 + 16] ){
                        // Store A2's source and swap A1's source with A2's source 
                        startB2 += 16;
                        B2 = _mm512_load_si512(&Arr_read[startB2]); 
                    }
                    //////////////////////////////// -- C -- ////////////////////////////////
                    if (startC1+16 == endC1) {
                        // Finished 1’s side but not 2’s side
                        startC2 += 16;
                        C2 = _mm512_load_si512(&Arr_read[startC2]);
                    } 
                    else if (startC2+16 == endC2) {
                        // Finished 2's side but not 1’s side
                        startC1 += 16;
                        C2 = _mm512_load_si512(&Arr_read[startC1]);
                    } 
                    else if (Arr_read[startC1 + 16] < Arr_read[startC2 + 16] ){
                        // use A1’s value
                        startC1 += 16;
                        C2 = _mm512_load_si512(&Arr_read[startC1]); 
                    } 
                    else if (Arr_read[startC1 + 16] >= Arr_read[startC2 + 16] ){
                        // Store A2's source and swap A1's source with A2's source 
                        startC2 += 16;
                        C2 = _mm512_load_si512(&Arr_read[startC2]); 
                    }
                    //////////////////////////////// -- D -- ////////////////////////////////
                    if (startD1+16 == endD1) {
                        // Finished 1’s side but not 2’s side
                        startD2 += 16;
                        D2 = _mm512_load_si512(&Arr_read[startD2]);
                    } 
                    else if (startD2+16 == endD2) {
                        // Finished 2's side but not 1’s side
                        startD1 += 16;
                        D2 = _mm512_load_si512(&Arr_read[startD1]);
                    } 
                    else if (Arr_read[startD1 + 16] < Arr_read[startD2 + 16] ){
                        // use A1’s value
                        startD1 += 16;
                        D2 = _mm512_load_si512(&Arr_read[startD1]); 
                    } 
                    else if (Arr_read[startD1 + 16] >= Arr_read[startD2 + 16] ){
                        // Store A2's source and swap A1's source with A2's source 
                        startD2 += 16;
                        D2 = _mm512_load_si512(&Arr_read[startD2]); 
                    }
                }
            }
            sorted_block_size *= 2; 
            int * temp = Arr_write;
            Arr_write = Arr_read;
            Arr_read = temp;  
        }
        delete [] Arr_write;  
        areBlocksSorted(thread_work_size, Arr_read, ILP);  
    }
    double elapsed_time = (std::chrono::steady_clock::now() - t0).count() / 1e9;
    cout << "Total Elapsed Time: " << elapsed_time << endl << endl;  
    // aprint(TOTAL_SIZE, Arr);

    return 0;
}





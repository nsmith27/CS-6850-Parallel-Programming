#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>


using namespace std;



template <typename T>
void bubbleSort(vector<T> &v);
template <typename T>
void vprint(vector<T> &v);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void bubbleSort(vector<T> &v){
    int size = v.size();
    for (auto i : v){
        for (int k =  0; k < size-1; k++){
            if ( v[k] > v[k+1] ){
                T temp = v[k];
                v[k] = v[k+1];
                v[k+1] = temp;  
            }
        }
    }
    return;
}

template <typename T>
void vprint(vector<T> &v){
    cout << v.size() << "\t";  
    for (auto it=v.begin(); it != v.end(); it++){
        cout << *it;
        if (it+1 != v.end() ){
            cout << " - " ;
        }
    }
    cout << endl; 
    return;  
}

template <typename T> 
void vShuffle( vector<T> &V ){
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

    vector<T> vec( V.begin(), V.end() );
    std::shuffle( vec.begin(), vec.end(), gen );
    V.assign( vec.begin(), vec.end() );
    return;  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){


    cout << "Hi there! " << endl << endl;  

    vector<int> test(32);
    for (int i = 0; i < test.size(); i++){
        test[i] = i; 
    }


    // vector<int> test{2,4,15,6,4,3};
    vShuffle(test);
    vprint(test); 
    bubbleSort(test);  
    vprint(test); 
    
    // Here are the final changes
    // Here are the final changes 2222!
    // 3
    // 4
    




    return 0;
}

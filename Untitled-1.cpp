#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <unistd.h>
#include <cstdio>


using namespace std;


int main(){

    cout << "hi!"<< endl;  
    // while (true){
        char bs = 8;
        char d = 127;
        string a = "+";
        string b = "x";
        // printf("%s", a.c_str());
        // printf("%c", bs);
        // printf("%s", b.c_str());
        cout << a;
        sleep(1);
        cout << bs;
        cout << d;
        cout << b;
        sleep(1);
        cout << bs;
        cout << d;

        // cout << bs << b;
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // cout << bs;
    // }


    printf("\n");

    return 0;
}
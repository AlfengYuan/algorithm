//
// Created by yyf on 23-12-15.
//

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <chrono>
using namespace std::chrono;
using namespace std;

int main()
{
    // system_clock
    auto t = system_clock::now();
    auto tNow = system_clock::to_time_t(t);

    string str_time = std::ctime(&tNow);

    cout << str_time;

    // duration
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i<100; i++) cout << "nice" << endl;
    auto end = std::chrono::steady_clock::now();
    auto tt = std::chrono::duration_cast<microseconds>(end - start);

    cout << "program time = " << tt.count() << "microseconds" << endl;

    return 0;
}
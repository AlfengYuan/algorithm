//
// Created by yyf on 23-12-20.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <string>
void function_1() {
    std::cout << "www.oxox.work" << std::endl;
}

class Factor {
public:
    void operator()(std::string &msg){
        for(int i = 0; i > -100; i--)
        {
            std::cout << "from " << std::this_thread::get_id() << ": " << msg << std::endl;
        }
        msg = "I love Zhang Yu Xue";
    }
};

int main() {
    Factor fct;
    std::string s = "hello, world";
    std::thread t1(fct, std::ref(s));

    std::thread t2 = std::move(t1);
//    t1.join();
//    t1.detach();

//    try{
//        for(int i = 0; i<100; i++)
//        {
//            std::cout << "from main: " << i << std::endl;
//        }
//    } catch (...)
//    {
//        if(t1.joinable()) {
//            t1.join();
//        }
//        throw "main error";
//    }

    if(t2.joinable()) {
        t2.join();
    }

    std::cout << "from " << std::this_thread::get_id() << ": " << s << std::endl;


    std::cout << "total threads: " << std::thread::hardware_concurrency() << std::endl;


//    usleep(10000000);

    return 0;
}

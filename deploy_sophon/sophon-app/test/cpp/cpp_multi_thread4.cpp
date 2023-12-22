//
// Created by yyf on 23-12-20.
//
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
class LockFile{
public:
    LockFile(){
        //f.open("log.txt");
    }

    void shared_print(std::string id, int value)
    {
        std::unique_lock<std::mutex> locker(m_mutex, std::defer_lock);
        //...
        locker.lock();
        std::cout << "From " << id << ": " << value << std::endl;
        locker.unlock();
        //...

    }

    // std::ofstream &GetStream() {return f;}
//    void processf(void fun(std::ofstream &))
//    {
//        fun(f);
//    }
protected:
private:
    std::mutex m_mutex;
    std::ofstream f;
};

//std::mutex mu;
//
//void shared_print(std::string msg, int id)
//{
////    mu.lock();
//    std::lock_guard<std::mutex> guard(mu);
//    std::cout << msg << id << std::endl;
////    mu.unlock();
//}

void function_1(LockFile &log){
    for(int i = 0; i > -100; i--)
    {
        log.shared_print("From t1: ", i);
    }
}

int main()
{
    LockFile log;
    std::thread t1(function_1, std::ref(log));
    for(int i = 0; i<100; i++)
    {
        log.shared_print("From main: ", i);
    }

    t1.join();

    return 0;
}

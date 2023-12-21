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
        f.open("log.txt");
    }

    void shared_print(std::string id, int value)
    {
        std::lock(m_mutex, m_mutex2);
        std::lock_guard<std::mutex> locker(m_mutex, std::adopt_lock);
        std::lock_guard<std::mutex> locker2(m_mutex2, std::adopt_lock);
        std::cout << "From " << id << ": " << value << std::endl;
    }

    void shared_print2(std::string id, int value)
    {
        std::lock(m_mutex, m_mutex2);
        std::lock_guard<std::mutex> locker2(m_mutex2, std::adopt_lock);
        std::lock_guard<std::mutex> locker(m_mutex, std::adopt_lock);

        std::cout << "From " << id << ": " << value << std::endl;
    }

    // std::ofstream &GetStream() {return f;}
//    void processf(void fun(std::ofstream &))
//    {
//        fun(f);
//    }
protected:
private:
    std::mutex m_mutex;
    std::mutex m_mutex2;
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
        log.shared_print2("From main: ", i);
    }

    t1.join();

    return 0;
}

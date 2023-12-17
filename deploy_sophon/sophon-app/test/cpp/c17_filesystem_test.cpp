//
// Created by yyf on 23-12-15.
//


#include <filesystem>
#include <iostream>

using namespace std::filesystem;

int main()
{
    path p1 = "./test1/";
    path p2 = "test2/test3";
    path p3 = p1 / p2;
    std::cout << p3 << std::endl;

    p1 += p2;
    std::cout << p1 << std::endl;

    if(p1 == p3){
        std::cout << "The paths are equal " << std::endl;
    } else {
        std::cout << "The paths are not equal " << std::endl;
    }

    path p = "C:/Users/John/Documents/report.txt";
    path filename = p.stem();
    path extension = p.extension();
    std::cout << filename << std::endl;
    std::cout << extension << std::endl;

    p = "../../test";
    for(auto &entry : directory_iterator(p))
    {
        std::cout << entry.path() << std::endl;
    }

    p = "./";
    for(auto &entry : directory_iterator(p))
    {
        if(is_regular_file(entry))
        {
            std::cout << entry.path() << std::endl;
        }
    }

    path currentPath = current_path();
    std::cout << "Current working directory is: " << currentPath << std::endl;



    return 0;
}
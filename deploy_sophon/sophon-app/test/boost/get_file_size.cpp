//
// Created by yyf on 23-12-19.
//

#include <iostream>
#include <boost/filesystem.hpp>


int main()
{
    int file_size = boost::filesystem::file_size("/data/se50_data/resnet50/20231115112043liuyu.jpg");
    std::cout << "文件大小为: " << file_size << "字节\n";

    return 0;
}

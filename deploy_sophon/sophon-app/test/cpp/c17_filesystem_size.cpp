//
// Created by yyf on 23-12-19.
//

#include <filesystem>
#include <iostream>

int main()
{
    int file_size = std::filesystem::file_size("/data/se50_data/resnet50/20231115112043liuyu.jpg");
    std::cout << "文件大小为: " << file_size << "字节\n";
    return 0;
}
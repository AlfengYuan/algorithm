#ifndef _BASE64_HPP_
#define _BASE64_HPP_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
 
static inline bool is_base64(const char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

//加密
std::string base64_encode(const char * bytes_to_encode, unsigned int in_len);


//解密
std::string base64_decode(std::string const & encoded_string);


#endif 
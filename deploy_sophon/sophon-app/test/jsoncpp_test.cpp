#include <iostream>
#include <fstream>

#include "json/json.h"


#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__MSC_VER)
#pragma warning(disable: 4996)
#endif

using namespace std;

int createjson()
{
    Json::Value root;
    Json::Value language;
    Json::Value mail;
    // Json::StyledWriter writer;
    Json::FastWriter writer;

    root["Name"] = "pikashu";
    root["Age"] = 18;

    language[0] = "C++";
    language[1] = "Python";
    root["Language"] = language;

    mail["QQ"] = "789123456@qq.com";
    mail["Google"] = "789123456354@gmail.com";
    root["E-mail"] = mail;

    string json_file = writer.write(root);

    ofstream ofs;
    ofs.open("test1.json");
    if(!ofs.is_open())
    {
        cout << "open file error. " << endl;
        return -1;
    }
    ofs << json_file;
    ofs.close();
    return 0;
}

int createjson2()
{
    Json::Value root;
    Json::Value cameras;
    Json::Value wuyu;
    Json::Value liuyu;
    Json::StyledWriter writer;
    wuyu["rtsp"] = "rtsp://admin:yz123456@192.168.150.14:554/h264/ch1/main/av_stream";
    wuyu["state"] = 0;
    liuyu["rtsp"] = "rtsp://admin:yz123456@192.168.150.14:554/h264/ch1/sub/av_stream";
    liuyu["state"] = 0;
    cameras["wuyu"] = wuyu;
    cameras["liuyu"] = liuyu;

    root["cameras"] = cameras;

    string json_file = writer.write(root);

    ofstream ofs;
    ofs.open("test1.json");
    if(!ofs.is_open())
    {
        cout << "open file error. " << endl;
        return -1;
    }
    ofs << json_file;
    ofs.close();
    return 0;



}

int readjson()
{
    Json::Reader reader;
    Json::Value root;
    Json::Value language;
    Json::Value mail;

    ifstream ifs;
    ifs.open("test1.json");
    if(!ifs.is_open())
    {
        cout << "open file error." << endl;
        return -1;
    }

    if(!reader.parse(ifs, root))
    {
        cout << "parser error" << endl;
        return -1;
    }

    string Name = root["Name"].asString();
    int Age = root["Age"].asInt();
    cout << "Name: " << Name << endl;
    cout << "Age: " << Age << endl;

    if(root["Language"].isArray())
    {
        Json::Value array_1 = root["Language"];
        cout << "Language: ";
        for(int i = 0; i<array_1.size(); i++)
        {
            cout << array_1[i] << " ";
        }
        cout << endl;
    }

    cout << "Google: " << root["E-mail"].get("Google", "").asString() << endl;
    cout << "QQ: " << root["E-mail"].get("QQ", "").asString() << endl;

    return 0;

}

int readjson2()
{
    Json::Reader reader;
    Json::Value root;
    Json::Value language;
    Json::Value mail;
    Json::Value::Members members;
    ifstream ifs;
    ifs.open("test1.json");
    if(!ifs.is_open())
    {
        cout << "open file error." << endl;
        return -1;
    }

    if(!reader.parse(ifs, root))
    {
        cout << "parser error" << endl;
        return -1;
    }

    if(root.isMember("cameras"))
    {
        members = root["cameras"].getMemberNames();
    }

    for(Json::Value::Members::iterator it = members.begin(); it != members.end(); it++)
    {
        string key = *it;
        cout << key << endl;
    }

    return 0;

}

int main()
{
    std::cout << "createjson2==========" << std::endl;
    createjson2();
    // cout << "readjson============" << endl;
    // readjson();
    std::cout << "readjson2===========" << std::endl;
    readjson2();
    return 0;
}
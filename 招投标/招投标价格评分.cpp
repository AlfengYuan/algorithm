// cppProject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;


class Product
{
public:
    std::string name;
    double originPrice;
    double lastScore;
};

bool comparePrice(Product first, Product second)
{
    return first.originPrice < second.originPrice;
}

bool compareScore(Product first, Product second)
{
    return first.lastScore > second.lastScore;
}



int main(int argc, char* argv[])
{
    //cout << setiosflags(ios::fixed) << setprecision(2);
    int N;
    std::vector<Product> PriceVec;
    std::cout << "请输入有效投标文件个数： " << std::endl;
    std::cin >> N;
    for (int i = 0; i < N; i++)
    {
        Product product;
        string name;
        std::cout << "请输入第 " << i + 1 << "家投标名称" << std::endl;
        std::cin >> name;
        product.name = name;
        std::cout << "请输入第 " << i + 1 << "家投标报价" << std::endl;
        double price;
        std::cin >> price;
        product.originPrice = price;
        product.lastScore = 95;

        PriceVec.push_back(product);
    }

    sort(PriceVec.begin(), PriceVec.end(), comparePrice);

    int method;
    cout << "请选择计算方法： 1 或者 2" << endl;
    cin >> method;

    if (method == 1)
    {
        double C = 0.0;
        double K = 0.0;
        double A = 0.0;

        if (N >= 7)
        {
            double top20 = 0.2 * N;
            int left = top20 - int(top20) >= 0.5 ? (int(top20) + 1) : (int(top20));
            int right = N - left - 1;
            while (left > 0 && PriceVec[left - 1].originPrice == PriceVec[left].originPrice) left--;
            while (right + 1 < N && PriceVec[right + 1].originPrice == PriceVec[right].originPrice) right++;

            double sum = 0;
            for (int i = left; i <= right; i++)
            {
                sum += PriceVec[i].originPrice;
            }

            A = sum / (right - left + 1);
        }
        else if (N >= 4)
        {
            int left = 0;
            int right = N - 2;
            while (right > 0 && PriceVec[right - 1].originPrice == PriceVec[right].originPrice) right--;

            double sum = 0;
            for (int i = left; i <= right; i++)
            {
                sum += PriceVec[i].originPrice;
            }

            A = sum / (right - left + 1);
        }
        else
        {
            A = PriceVec[N - 2].originPrice;
        }

        cout << "请输入K值，K值在开标前由招标人推选的代表随机抽取确定, 请输入95， 96， 97 或 98即可" << endl;

        cin >> K;

        C = A * K / 100;

        for (size_t i = 0; i < PriceVec.size(); i++)
        {
            if (PriceVec[i].originPrice - C == 0)
            {
                PriceVec[i].lastScore = 95;
            }
            else if (PriceVec[i].originPrice - C > 0)
            {
                double diff = (PriceVec[i].originPrice - C) / C * 150;
                PriceVec[i].lastScore = 95 - diff;
            }
            else
            {
                double diff = (C - PriceVec[i].originPrice) / C * 100;
                PriceVec[i].lastScore = 95 - diff;
            }
        }


    }
    else if (method == 2)
    {
        double C = 0.0;
        double K1 = 0.0;
        double K2 = 0.0;
        double Q1 = 0.0;
        double Q2 = 0.0;
        double A = 0.0;
        double B = 0.0;
        
        cout << "方法二需要输入招标控制价B， 请输入招标控制价B： " << endl;
        cin >> B;

        if (N >= 7)
        {
            double top20 = 0.2 * N;
            int left = top20 - int(top20) >= 0.5 ? (int(top20) + 1) : (int(top20));
            int right = N - left - 1;
            while (left > 0 && PriceVec[left - 1].originPrice == PriceVec[left].originPrice) left--;
            while (right + 1 < N && PriceVec[right + 1].originPrice == PriceVec[right].originPrice) right++;

            double sum = 0;
            for (int i = left; i <= right; i++)
            {
                sum += PriceVec[i].originPrice;
            }

            A = sum / (right - left + 1);
        }
        else if (N >= 4)
        {
            int left = 0;
            int right = N - 2;
            while (right > 0 && PriceVec[right - 1].originPrice == PriceVec[right].originPrice) right--;

            double sum = 0;
            for (int i = left; i <= right; i++)
            {
                sum += PriceVec[i].originPrice;
            }

            A = sum / (right - left + 1);
        }
        else
        {
            A = PriceVec[N - 2].originPrice;
        }

        cout << "请输入K1值，K1值在开标前由招标人推选的代表随机抽取确定, 请输入95， 96， 97 或 98即可" << endl;

        cin >> K1;
        K2 = (100 - K1) / 100;
        K1 /= 100;

        cout << "请输入Q1值，Q1值在开标前由招标人推选的代表随机抽取确定，请输入65， 70， 75， 80，或 85 即可" << endl;
        cin >> Q1;
        Q2 = (100 - Q1) / 100;
        Q1 /= 100;
        

        C = A * K1 * Q1 + B * K2 * Q2;

        for (size_t i = 0; i < PriceVec.size(); i++)
        {
            if (PriceVec[i].originPrice - C == 0)
            {
                PriceVec[i].lastScore = 95;
            }
            else if (PriceVec[i].originPrice - C > 0)
            {
                double diff = (PriceVec[i].originPrice - C) / C * 150;
                PriceVec[i].lastScore = 95 - diff;
            }
            else
            {
                double diff = (C - PriceVec[i].originPrice) / C * 100;
                PriceVec[i].lastScore = 95 - diff;
            }
        }


    }
    else
    {
        cout << "只有方法1或者2" << endl;
    }

    sort(PriceVec.begin(), PriceVec.end(), compareScore);

    cout << "最终得分排序结果： " << endl;

    for (size_t i = 0; i < PriceVec.size(); i++)
    {
        cout << "公司: " << PriceVec[i].name << ", 报价: " << PriceVec[i].originPrice << ", 得分: " << PriceVec[i].lastScore << endl;
    }

    system("pause");

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

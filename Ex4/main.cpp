#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <ctime>
#include <tuple>

int main()
{
    auto t = std::make_tuple("String",5.2, 1);
    int a = std::get<3>(t);
    std::cout << std::get<0>(t) << ' '
              << std::get<1>(t) << ' '
              << std::get<2>(t) << '\n';

    return 0;
}
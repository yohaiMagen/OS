
#include <iostream>
#include <sstream>
#include <vector>

#define SPACE_CHAR ' '

int main()
{

    std::string str  = "name niv a a   a a   a a a\n";
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
//    if (num_seg < 0) {
//        while (std::getline(ss, item, SPACE_CHAR))
//        {
//            result.push_back(item);
//        }
//    }
//    else
//    {
        int j = 0;
        for (unsigned int i = 0; i < 3; ++i)
        {
            int next = str.find(' ', j);
            int len = 0;
            if ( i == (3 - 1))
                len = str.length() - j;
            else
            {
                len = next - j;
            }
            result.push_back(str.substr(j, len));
            j = next + 1;
//            std::getline(ss, item, SPACE_CHAR);
//            result.push_back(item);
        }
//    }


//    std::cout << SOMAXCONN << std::endl;
    for (int i = 0; i < result.size(); ++i)
    {
        std::cout << "-" << result[i] << "-" << std::endl;
    }
    return 0;
}
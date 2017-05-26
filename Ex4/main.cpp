#include <iostream>
#include <sys/stat.h>

int main()
{
    struct stat fi;
    stat("/tmp", &fi);
    int blksize = fi.st_blksize;
    std::cout << blksize <<std::endl;
    if(0)
    {
        std::cout << "Blaldld" << std::endl;
    }
    return 0;
}
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <ctime>

int main()
{
//    int fd1 = open("/home/nivkeren/Documents/file", O_RDONLY);
//    int fd2= open("/home/nivkeren/Documents/file", O_RDONLY | O_DIRECT | O_SYNC);
//    char* temp = (char*)malloc(100000);
//    off_t a;
//    struct stat st;
//    stat("/home/nivkeren/Documents/file", &st);
//    a = st.st_size;
//    int read = pread(fd1, temp, 1000, 0);
//    int read2 = pread(fd1, temp, 1000, read);
//    char b[4096*4];
//    char* c = &b[4096];
//    char* d = &b[4096*2];
    int fd1 = open("/home/yohai/CLionProjects/OS/Ex4/test.txt", O_RDONLY);
    char a[2];
     int x = pread(fd1, a , 10 , 0);
     char* b = a + x;
      x = pread(fd1, b , 10 , x);
    std::cout << x << std::endl;

    return 0;
}
//
// Created by dsgissin on 6/5/17.
//

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <cstring>
#include "CacheFS.h"

void sanityCheck()
{

    bool ok = true;

    // get the block size:
    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    std::ofstream eraser;
    eraser.open("/tmp/sanity_test.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // create the files for the test:
    std::ofstream outfile ("/tmp/sanity_test.txt");
    for (unsigned int i=0; i<10*blockSize; i++)
    {
        outfile << "TEST ";
    }
    outfile.close();

    // run basic read tests:
    CacheFS_init(10, LRU, 0.1, 0.1);
    int fd = CacheFS_open("/tmp/sanity_test.txt");

    char data[11] = "\0";
    CacheFS_pread(fd, &data, 10, 0); // read from beginning of block
    if (strcmp(data, "TEST TEST ")) {ok = false;}
    CacheFS_pread(fd, &data, 10, 5*blockSize + 1); // read from middle of block
    if (strcmp(data, "EST TEST T")) {ok = false;}
    CacheFS_pread(fd, &data, 10, 5*blockSize + 2); // read from middle of block
    if (strcmp(data, "ST TEST TE")) {ok = false;}
    CacheFS_pread(fd, &data, 10, 5*blockSize + 3); // read from middle of block
    if (strcmp(data, "T TEST TES")) {ok = false;}
    CacheFS_pread(fd, &data, 10, 5*blockSize + 4); // read from middle of block
    if (strcmp(data, " TEST TEST")) {ok = false;}

    eraser.open("/tmp/sanity_test_cache.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // review cache:
    CacheFS_print_cache("/tmp/sanity_test_cache.txt");
    std::ifstream resultsFileInput;
    resultsFileInput.open("/tmp/sanity_test_cache.txt");
    char cacheResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(cacheResults, 10000);

        if (strcmp(cacheResults, "/tmp/sanity_test.txt 5\n/tmp/sanity_test.txt 0\n")) {ok = false;}
    }
    resultsFileInput.close();

    eraser.open("/tmp/sanity_test_stats.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // review stats:
    CacheFS_print_stat("/tmp/sanity_test_stats.txt");
    resultsFileInput.open("/tmp/sanity_test_stats.txt");
    char statsResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 3.\nMisses number: 2.\n") || !strcmp(statsResults, "Hits number: 3\nMisses number: 2\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    CacheFS_close(fd);
    CacheFS_destroy();

    if (ok)
    {
        std::cout << "Sanity Check Passed!\n";
    }
    else
    {
        std::cout << "Sanity Check Failed!\n";
    }

}

void doubleOpenClose()
{
    bool ok = true;

    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    // create the files for the test:
    std::ofstream outfile ("/tmp/double_test.txt");
    for (unsigned int i=0; i<10*blockSize; i++)
    {
        outfile << "TEST ";
    }
    outfile.close();

    // open and read from the file using two file descriptors:
    CacheFS_init(10, LRU, 0.1, 0.1);
    int fd1 = CacheFS_open("/tmp/double_test.txt");
    int fd2 = CacheFS_open("/tmp/double_test.txt");

    char data[11];
    data[10] = '\0';
    CacheFS_pread(fd1, &data, 10, 0); // read from first fd
    CacheFS_pread(fd2, &data, 10, 0); // read from second fd

    // check that we have one hit:
    std::ofstream eraser;
    eraser.open("/tmp/test_double_open.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // review cache:
    char results[10000] = "\0";
    CacheFS_print_stat("/tmp/test_double_open.txt");
    std::ifstream resultsFileInput;
    resultsFileInput.open("/tmp/test_double_open.txt");
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(results, 10000);
        if (!(!strcmp(results, "Hits number: 1.\nMisses number: 1.\n") || !strcmp(results, "Hits number: 1\nMisses number: 1\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();

    if (ok)
    {
        std::cout << "Double Open Check Passed!\n";
    }
    else
    {
        std::cout << "Double Open Check Failed!\n";
    }
}

void offsetTooBig()
{
    bool ok = true;

    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    // create the files for the test:
    std::ofstream eraser;
    eraser.open("/tmp/offset_test.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();
    std::ofstream outfile ("/tmp/offset_test.txt");
    for (unsigned int i=0; i<blockSize/2; i++)
    {
        outfile << "A";
    }
    outfile.close();

    // open and read from the file with an offset that is too big:
    CacheFS_init(10, LRU, 0.1, 0.1);
    int fd = CacheFS_open("/tmp/offset_test.txt");

    char data[11] = "\0";
    int ret = CacheFS_pread(fd, &data, 10, blockSize); // the offset is larger than the file size
    if (ret != 0) {ok = false;}

    if (data[0] != '\0') {ok = false;}

    // open and read from the file, where we ask for more data than is available in the file:
    ret = CacheFS_pread(fd, &data, 10, (blockSize/2)-5); // the offset is larger than the file size
    if (ret != 5) {ok = false;}
    if (strcmp(data, "AAAAA")) {ok = false;}

    CacheFS_close(fd);
    CacheFS_destroy();

    if (ok)
    {
        std::cout << "Offset Check Passed!\n";
    }
    else
    {
        std::cout << "Offset Check Failed!\n";
    }

}

void basicLRU()
{
    bool ok = true;

    // get the block size:
    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    // create the files for the test:
    std::ofstream outfile1 ("/tmp/LRU1.txt");
    for (unsigned int i=0; i<5*blockSize; i++)
    {
        outfile1 << "A";
    }
    outfile1.close();
    std::ofstream outfile2 ("/tmp/LRU2.txt");
    for (unsigned int i=0; i<5*blockSize; i++)
    {
        outfile2 << "B";
    }
    outfile2.close();

    std::ofstream eraser;
    eraser.open("/tmp/LRU_cache.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    CacheFS_init(5, LRU, 0.1, 0.1);
    int fd1 = CacheFS_open("/tmp/LRU1.txt");
    int fd2 = CacheFS_open("/tmp/LRU2.txt");

    char data[11];
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 3*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_print_cache("/tmp/LRU_cache.txt");
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_print_cache("/tmp/LRU_cache.txt");
    CacheFS_pread(fd2, &data, 10, 3*blockSize);
    CacheFS_print_cache("/tmp/LRU_cache.txt");

    // review cache:
    std::ifstream resultsFileInput;
    resultsFileInput.open("/tmp/LRU_cache.txt");
    char cacheResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(cacheResults, 10000);

        char cacheCorrect[] = "/tmp/LRU1.txt 4\n/tmp/LRU1.txt 3\n/tmp/LRU1.txt 2\n/tmp/LRU1.txt 1\n/tmp/LRU1.txt 0\n"
                "/tmp/LRU1.txt 0\n/tmp/LRU1.txt 4\n/tmp/LRU1.txt 3\n/tmp/LRU1.txt 2\n/tmp/LRU1.txt 1\n"
                "/tmp/LRU2.txt 3\n/tmp/LRU1.txt 0\n/tmp/LRU1.txt 4\n/tmp/LRU1.txt 3\n/tmp/LRU1.txt 2\n";

        if (strcmp(cacheResults, cacheCorrect)) {ok = false;}
    }
    resultsFileInput.close();

    eraser.open("/tmp/LRU_stats.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // review stats:
    CacheFS_print_stat("/tmp/LRU_stats.txt");
    resultsFileInput.open("/tmp/LRU_stats.txt");
    char statsResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 1.\nMisses number: 6.\n") || !strcmp(statsResults, "Hits number: 1\nMisses number: 6\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();

    if (ok)
    {
        std::cout << "Basic LRU Check Passed!\n";
    }
    else
    {
        std::cout << "Basic LRU Check Failed!\n";
    }

}

void basicLFU()
{
    bool ok = true;

    // get the block size:
    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    // create the files for the test:
    std::ofstream outfile1 ("/tmp/LFU1.txt");
    for (unsigned int i=0; i<5*blockSize; i++)
    {
        outfile1 << "A";
    }
    outfile1.close();
    std::ofstream outfile2 ("/tmp/LFU2.txt");
    for (unsigned int i=0; i<5*blockSize; i++)
    {
        outfile2 << "B";
    }
    outfile2.close();

    std::ofstream eraser;
    eraser.open("/tmp/LFU_cache.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();
    eraser.open("/tmp/LFU_stats.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    CacheFS_init(5, LFU, 0.1, 0.1);
    int fd1 = CacheFS_open("/tmp/LFU1.txt");
    int fd2 = CacheFS_open("/tmp/LFU2.txt");

    char data[11];

    // ramp up the frequency of these ones:
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 3*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 3*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 3*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_print_cache("/tmp/LFU_cache.txt");

    // these should all be misses:
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_print_cache("/tmp/LFU_cache.txt");
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_print_cache("/tmp/LFU_cache.txt");
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);

    // get the results:
    CacheFS_print_stat("/tmp/LFU_stats.txt");

    // review cache:
    std::ifstream resultsFileInput;
    resultsFileInput.open("/tmp/LFU_cache.txt");
    char cacheResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(cacheResults, 10000);

        char cacheCorrect[] = "/tmp/LFU1.txt 4\n/tmp/LFU1.txt 3\n/tmp/LFU1.txt 2\n/tmp/LFU1.txt 1\n/tmp/LFU1.txt 0\n"
                "/tmp/LFU1.txt 4\n/tmp/LFU1.txt 3\n/tmp/LFU1.txt 2\n/tmp/LFU1.txt 1\n/tmp/LFU2.txt 0\n"
                "/tmp/LFU1.txt 4\n/tmp/LFU1.txt 3\n/tmp/LFU1.txt 2\n/tmp/LFU1.txt 1\n/tmp/LFU2.txt 1\n";

        if (strcmp(cacheResults, cacheCorrect)) {ok = false;}
    }
    resultsFileInput.close();

    // review stats:
    resultsFileInput.open("/tmp/LFU_stats.txt");
    char statsResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 10.\nMisses number: 15.\n") || !strcmp(statsResults, "Hits number: 10\nMisses number: 15\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();

    if (ok)
    {
        std::cout << "Basic LFU Check Passed!\n";
    }
    else
    {
        std::cout << "Basic LFU Check Failed!\n";
    }
}

void basicFBR()
{
    bool ok = true;

    // get the block size:
    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    // create the files for the test:
    std::ofstream outfile1 ("/tmp/FBR1.txt");
    for (unsigned int i=0; i<10*blockSize; i++)
    {
        outfile1 << "A";
    }
    outfile1.close();
    std::ofstream outfile2 ("/tmp/FBR2.txt");
    for (unsigned int i=0; i<10*blockSize; i++)
    {
        outfile2 << "B";
    }
    outfile2.close();

    std::ofstream eraser;
    eraser.open("/tmp/FBR_cache0.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();
    eraser.open("/tmp/FBR_cache1.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();
    eraser.open("/tmp/FBR_stats0.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();
    eraser.open("/tmp/FBR_stats1.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // test that reading a block from "new" doens't increase the frequency:
    CacheFS_init(10, FBR, 0.3, 0.3);
    int fd1 = CacheFS_open("/tmp/FBR1.txt");
    int fd2 = CacheFS_open("/tmp/FBR2.txt");
    char data[11];

    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize); // This should not help the 0 block and it should be removed in the end!
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize);
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 3*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_pread(fd1, &data, 10, 5*blockSize);
    CacheFS_pread(fd1, &data, 10, 6*blockSize);
    CacheFS_pread(fd1, &data, 10, 7*blockSize);
    CacheFS_pread(fd1, &data, 10, 8*blockSize);
    CacheFS_pread(fd1, &data, 10, 9*blockSize);
    CacheFS_pread(fd2, &data, 10, 5*blockSize);
    CacheFS_print_cache("/tmp/FBR_cache0.txt");
    CacheFS_print_stat("/tmp/FBR_stats0.txt");

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();


    // test that the lowest frequency from "old" is removed, and not the oldest one in "old":
    CacheFS_init(10, FBR, 0.3, 0.3);
    fd1 = CacheFS_open("/tmp/FBR1.txt");
    fd2 = CacheFS_open("/tmp/FBR2.txt");

    CacheFS_pread(fd1, &data, 10, 0*blockSize); // read #1
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_pread(fd1, &data, 10, 3*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize); // read #2
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_pread(fd1, &data, 10, 5*blockSize);
    CacheFS_pread(fd1, &data, 10, 6*blockSize);
    CacheFS_pread(fd1, &data, 10, 0*blockSize); // read #3 - This one should still be in the cache after all the reads!
    CacheFS_pread(fd1, &data, 10, 7*blockSize);
    CacheFS_pread(fd1, &data, 10, 8*blockSize);
    CacheFS_pread(fd1, &data, 10, 9*blockSize);
    CacheFS_pread(fd1, &data, 10, 1*blockSize);
    CacheFS_pread(fd1, &data, 10, 2*blockSize);
    CacheFS_print_cache("/tmp/FBR_cache1.txt");
    CacheFS_pread(fd1, &data, 10, 3*blockSize);
    CacheFS_pread(fd1, &data, 10, 4*blockSize);
    CacheFS_pread(fd1, &data, 10, 5*blockSize);
    CacheFS_pread(fd1, &data, 10, 6*blockSize);

    CacheFS_pread(fd2, &data, 10, 0*blockSize);
    CacheFS_pread(fd2, &data, 10, 1*blockSize);
    CacheFS_pread(fd2, &data, 10, 2*blockSize);
    CacheFS_print_cache("/tmp/FBR_cache1.txt");
    CacheFS_print_stat("/tmp/FBR_stats1.txt");

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();


    // review cache:
    std::ifstream resultsFileInput;
    resultsFileInput.open("/tmp/FBR_cache0.txt");
    char cacheResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(cacheResults, 10000);

        char cacheCorrect[] = "/tmp/FBR2.txt 5\n/tmp/FBR1.txt 9\n/tmp/FBR1.txt 8\n/tmp/FBR1.txt 7\n/tmp/FBR1.txt 6\n"
                "/tmp/FBR1.txt 5\n/tmp/FBR1.txt 4\n/tmp/FBR1.txt 3\n/tmp/FBR1.txt 2\n/tmp/FBR1.txt 1\n";

        if (strcmp(cacheResults, cacheCorrect)) {ok = false;}
    }
    resultsFileInput.close();

    resultsFileInput.open("/tmp/FBR_cache1.txt");
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(cacheResults, 10000);

        char cacheCorrect[] = "/tmp/FBR1.txt 2\n/tmp/FBR1.txt 1\n/tmp/FBR1.txt 9\n/tmp/FBR1.txt 8\n/tmp/FBR1.txt 7\n"
                "/tmp/FBR1.txt 0\n/tmp/FBR1.txt 6\n/tmp/FBR1.txt 5\n/tmp/FBR1.txt 4\n/tmp/FBR1.txt 3\n"
                "/tmp/FBR2.txt 2\n/tmp/FBR2.txt 1\n/tmp/FBR2.txt 0\n/tmp/FBR1.txt 6\n/tmp/FBR1.txt 5\n"
                "/tmp/FBR1.txt 4\n/tmp/FBR1.txt 3\n/tmp/FBR1.txt 2\n/tmp/FBR1.txt 1\n/tmp/FBR1.txt 0\n";

        if (strcmp(cacheResults, cacheCorrect)) {ok = false;}
    }
    resultsFileInput.close();

    // review stats:
    resultsFileInput.open("/tmp/FBR_stats0.txt");
    char statsResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 4.\nMisses number: 11.\n") || !strcmp(statsResults, "Hits number: 4\nMisses number: 11\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    resultsFileInput.open("/tmp/FBR_stats1.txt");
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 8.\nMisses number: 13.\n") || !strcmp(statsResults, "Hits number: 8\nMisses number: 13\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    if (ok)
    {
        std::cout << "Basic FBR Check Passed!\n";
    }
    else
    {
        std::cout << "Basic FBR Check Failed!\n";
    }
}

void readSeveralBlocksAtOnce()
{
    bool ok = true;

    // get the block size:
    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    // create the files for the test:
    std::ofstream outfile1 ("/tmp/Several1.txt");
    for (unsigned int i=0; i<10*blockSize; i++)
    {
        outfile1 << "A";
    }
    outfile1.close();
    std::ofstream outfile2 ("/tmp/Several2.txt");
    for (unsigned int i=0; i<10*blockSize; i++)
    {
        outfile2 << "B";
    }
    outfile2.close();

    std::ofstream eraser;
    eraser.open("/tmp/Several_cache.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    CacheFS_init(3, LRU, 0.1, 0.1);
    int fd1 = CacheFS_open("/tmp/Several1.txt");
    int fd2 = CacheFS_open("/tmp/Several2.txt");

    char data[3*blockSize] = "\0";
    CacheFS_pread(fd1, &data, 2*blockSize + 10, 0*blockSize); // read three blocks at once
    CacheFS_print_cache("/tmp/Several_cache.txt");
    CacheFS_pread(fd1, &data, 2*blockSize + 10, 2*blockSize); // read three blocks at once (one already in cache)
    CacheFS_print_cache("/tmp/Several_cache.txt");
    CacheFS_pread(fd1, &data, 2*blockSize + 10, 0*blockSize); // read three blocks at once (one should be in cache but evicted because we read the blocks one by one)
    CacheFS_print_cache("/tmp/Several_cache.txt");

    // review cache:
    std::ifstream resultsFileInput;
    resultsFileInput.open("/tmp/Several_cache.txt");
    char cacheResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(cacheResults, 10000);

        char cacheCorrect[] = "/tmp/Several1.txt 2\n/tmp/Several1.txt 1\n/tmp/Several1.txt 0\n/tmp/Several1.txt 4\n"
                "/tmp/Several1.txt 3\n/tmp/Several1.txt 2\n/tmp/Several1.txt 2\n/tmp/Several1.txt 1\n/tmp/Several1.txt 0\n";

        if (strcmp(cacheResults, cacheCorrect)) {ok = false;}
    }
    resultsFileInput.close();

    eraser.open("/tmp/Several_stats.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // review stats:
    CacheFS_print_stat("/tmp/Several_stats.txt");
    resultsFileInput.open("/tmp/Several_stats.txt");
    char statsResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 1.\nMisses number: 8.\n") || !strcmp(statsResults, "Hits number: 1\nMisses number: 8\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();

    if (ok)
    {
        std::cout << "Several Blocks Check Passed!\n";
    }
    else
    {
        std::cout << "Several Blocks Check Failed!\n";
    }

}

void stressTest()
{
    bool ok = true;

    // get the block size:
    struct stat fi;
    stat("/tmp", &fi);
    size_t blockSize = (size_t)fi.st_blksize;

    // create the files for the test:
    std::ofstream outfile1 ("/tmp/stress1.txt");
    for (unsigned int i=0; i<10*blockSize; i++)
    {
        outfile1 << "A";
    }
    outfile1.close();
    std::ofstream outfile2 ("/tmp/stress2.txt");
    for (unsigned int i=0; i<10*blockSize; i++)
    {
        outfile2 << "B";
    }
    outfile2.close();

    std::ofstream eraser;

    CacheFS_init(5, LRU, 0.1, 0.1);
    int fd1 = CacheFS_open("/tmp/Several1.txt");
    int fd2 = CacheFS_open("/tmp/Several2.txt");

    char data[10*blockSize] = "\0";
    for (int i = 0; i<10000; i++)
    {
        CacheFS_pread(fd1, &data, 6*blockSize, 0*blockSize); // read 6 blocks again and again
    }

    eraser.open("/tmp/stress_stats.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // review stats:
    std::ifstream resultsFileInput;
    CacheFS_print_stat("/tmp/stress_stats.txt");
    resultsFileInput.open("/tmp/stress_stats.txt");
    char statsResults[10000] = "\0";
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 0.\nMisses number: 60000.\n") || !strcmp(statsResults, "Hits number: 0\nMisses number: 60000\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();

    CacheFS_init(6, LRU, 0.1, 0.1);
    fd1 = CacheFS_open("/tmp/Several1.txt");
    fd2 = CacheFS_open("/tmp/Several2.txt");

    for (int i = 0; i<10000; i++)
    {
        CacheFS_pread(fd1, &data, 6*blockSize, 0*blockSize); // read 6 blocks again and again
    }

    eraser.open("/tmp/stress_stats_2.txt", std::ofstream::out | std::ofstream::trunc);
    eraser.close();

    // review stats:
    CacheFS_print_stat("/tmp/stress_stats_2.txt");
    resultsFileInput.open("/tmp/stress_stats_2.txt");
    if (resultsFileInput.is_open()) {
        resultsFileInput.read(statsResults, 10000);

        if (!(!strcmp(statsResults, "Hits number: 59994.\nMisses number: 6.\n") || !strcmp(statsResults, "Hits number: 59994\nMisses number: 6\n")))
        {
            ok = false;
        }
    }
    resultsFileInput.close();

    CacheFS_close(fd1);
    CacheFS_close(fd2);
    CacheFS_destroy();


    if (ok)
    {
        std::cout << "Stress Test Passed!\n";
    }
    else
    {
        std::cout << "Stress Test Failed!\n";
    }

}

int main()
{
    sanityCheck();
    doubleOpenClose();
    offsetTooBig();
    basicLRU();
    basicLFU();
    basicFBR();
    readSeveralBlocksAtOnce();
    stressTest();

    return 0;
}
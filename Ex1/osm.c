#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/time.h>
#include <string.h>
#include "osm.h"


#define OP_LOOP_SIZE 50
#define SETVALUE 1000
#define SEC_TO_NANO 1000000000
#define MICRO_TO_NANO 1000
#define MACHINE_NAME_LENGTH 1024
#define ADERESS "/home/yohai/CLionProjects/untitled/qqq"
#define BLOOK_SIZE 4096

char* machineName;



/* Initialization function that the user must call
 * before running any other library function.
 * The function may, for example, allocate memory or
 * create/open files.
 * Pay attention: this function may be empty for some desings. It's fine.
 * Returns 0 uppon success and -1 on failure
 */
int osm_init()
{
    machineName = (char*)malloc(sizeof(char)*MACHINE_NAME_LENGTH);
}


/* finalizer function that the user must call
 * after running any other library function.
 * The function may, for example, free memory or
 * close/delete files.
 * Returns 0 uppon success and -1 on failure
 */
int osm_finalizer()
{
    free(machineName);
}

double osmSetTime(unsigned int iterations)
{
    if(iterations == 0)
    {
        iterations = SETVALUE;
    }
    if(iterations % OP_LOOP_SIZE != 0)
    {
        iterations += OP_LOOP_SIZE;
    }
    struct timeval time1, time2;
    struct timezone tz;
    unsigned long i = 0;
    volatile int a = 0;
    if(gettimeofday(&time1, &tz) == -1)
    {
        return -1;
    }
    for(; i < iterations ; i += OP_LOOP_SIZE)
    {
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
        a = 0;
    }
    if(gettimeofday(&time2, &tz) == -1)
    {
        return -1;
    }
    return ((double )(time2.tv_sec - time1.tv_sec) * SEC_TO_NANO +(time2.tv_usec - time1.tv_usec) * MICRO_TO_NANO) / (i * OP_LOOP_SIZE);
}


/* Time measurement function for a simple arithmetic operation.
   returns time in nano-seconds upon success,
   and -1 upon failure.
   */
double osm_operation_time(unsigned int iterations)
{
    if(iterations == 0)
    {
        iterations = SETVALUE;
    }
    if(iterations % OP_LOOP_SIZE != 0)
    {
        iterations += OP_LOOP_SIZE;
    }
    struct timeval time1, time2;
    struct timezone tz;
    unsigned long i = 0;
    volatile int a = 0;
    if(gettimeofday(&time1, &tz) == -1)
    {
        return -1;
    }
    for(; i < iterations ; i += OP_LOOP_SIZE)
    {
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
        a = 1+1;
    }
    if(gettimeofday(&time2, &tz) == -1)
    {
        return -1;
    }
    return ((double )(time2.tv_sec - time1.tv_sec) * SEC_TO_NANO +(time2.tv_usec - time1.tv_usec) * MICRO_TO_NANO) / (i * OP_LOOP_SIZE);
}

void dumyFunc(){}


/* Time measurement function for an empty function call.
   returns time in nano-seconds upon success,
   and -1 upon failure.
   */
double osm_function_time(unsigned int iterations)
{
    if(iterations == 0)
    {
        iterations = SETVALUE;
    }
    if(iterations % OP_LOOP_SIZE != 0)
    {
        iterations += OP_LOOP_SIZE;
    }
    struct timeval time1, time2;
    struct timezone tz;
    unsigned long i = 0;
    if(gettimeofday(&time1, &tz) == -1)
    {
        return -1;
    }
    for(; i < iterations ; i += OP_LOOP_SIZE)
    {
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();
        dumyFunc();

    }
    if(gettimeofday(&time2, &tz) == -1)
    {
        return -1;
    }
    return ((double )(time2.tv_sec - time1.tv_sec) * SEC_TO_NANO +(time2.tv_usec - time1.tv_usec) * MICRO_TO_NANO) / (i * OP_LOOP_SIZE);
}


/* Time measurement function for an empty trap into the operating system.
   returns time in nano-seconds upon success,
   and -1 upon failure.
   */
double osm_syscall_time(unsigned int iterations)
{
    if(iterations == 0)
    {
        iterations = SETVALUE;
    }
    if(iterations % OP_LOOP_SIZE != 0)
    {
        iterations += OP_LOOP_SIZE;
    }
    struct timeval time1, time2;
    struct timezone tz;
    unsigned long i = 0;
    if(gettimeofday(&time1, &tz) == -1)
    {
        return -1;
    }
    for(; i < iterations ; i += OP_LOOP_SIZE)
    {
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;

    }
    if(gettimeofday(&time2, &tz) == -1)
    {
        return -1;
    }
    return ((double )(time2.tv_sec - time1.tv_sec) * SEC_TO_NANO +(time2.tv_usec - time1.tv_usec) * MICRO_TO_NANO) / (i * OP_LOOP_SIZE);
}

/* Time measurement function for accessing the disk.
   returns time in nano-seconds upon success,
   and -1 upon failure.
   */
double osm_disk_time(unsigned int iterations)
{
    char address[200] = ADERESS;
    char block[BLOOK_SIZE +1] ;
    for (int k = 0; k <  BLOOK_SIZE; ++k)
    {
        block[k] = 'a';

    }
    block[BLOOK_SIZE-1] = '\0';
    int testMe = open(address, O_CREAT | O_RDWR | O_SYNC, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH );
    block[BLOOK_SIZE- strlen(address)] = '\0';
    int a = write(testMe, block, BLOOK_SIZE - strlen(address - 1));
    block[BLOOK_SIZE- strlen(address)] = 'b';
    if(iterations == 0)
    {
        iterations = SETVALUE;
    }
    if(iterations % OP_LOOP_SIZE != 0)
    {
        iterations += OP_LOOP_SIZE;
    }
    struct timeval time1, time2;
    struct timezone tz;
    unsigned long i = 0;
    if(gettimeofday(&time1, &tz) == -1)
    {
        return -1;
    }
    for(; i < iterations ; i += OP_LOOP_SIZE)
    {
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);
        write(testMe, block, BLOOK_SIZE);


    }
    if(gettimeofday(&time2, &tz) == -1)
    {
        return -1;
    }
    return ((double )(time2.tv_sec - time1.tv_sec) * SEC_TO_NANO +(time2.tv_usec - time1.tv_usec) * MICRO_TO_NANO) / (i * OP_LOOP_SIZE);



}




timeMeasurmentStructure measureTimes (unsigned int operation_iterations,
                                      unsigned int function_iterations,
                                      unsigned int syscall_iterations,
                                      unsigned int disk_iterations)
{
    timeMeasurmentStructure tms;
    if(gethostname(machineName, MACHINE_NAME_LENGTH-1) == -1)
    {
        machineName[0] = '\0';
    }
    machineName[MACHINE_NAME_LENGTH-1] = '\0';
    tms.machineName = machineName;
    tms.instructionTimeNanoSecond = osm_operation_time(operation_iterations);
    tms.functionTimeNanoSecond = osm_function_time(function_iterations);
    tms.trapTimeNanoSecond = osm_syscall_time(syscall_iterations);
    tms.diskTimeNanoSecond = osm_disk_time(disk_iterations);
    tms.functionInstructionRatio = tms.functionTimeNanoSecond / tms.instructionTimeNanoSecond;
    tms.trapInstructionRatio = tms.trapTimeNanoSecond / tms.instructionTimeNanoSecond;
    tms.diskInstructionRatio = tms.diskTimeNanoSecond / tms.instructionTimeNanoSecond;
    return tms;

}



#include <stdio.h>
#include <time.h>
#include <sys/time.h>

/* 
 * timestamp - read and print the current timestamp
 * Wade Ryan 2020-09-27
 * compile using: g++ timestamp.cpp -o timestamp
 */ 

int main(int argc, char **argv) {
    char   timestamp[24];
    struct timeval currentTime;
//  struct tm      ts;

    gettimeofday(&currentTime, NULL);

    long long epoch  =  (unsigned long long)(currentTime.tv_sec) * 1000 +
                        (unsigned long long)(currentTime.tv_usec) / 1000;
        
    strftime(timestamp, sizeof(timestamp), "%F %T", localtime(&currentTime.tv_sec));

    printf("epoch %lld ms :: %s.%03ld\n", epoch, timestamp, currentTime.tv_usec/1000);

}

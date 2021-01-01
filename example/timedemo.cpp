#include <log4pi.h>

/*
 * time demo
 *
 * wade ryan
 * 2021-01-01
 *
 * Complie using:
 *
 * $ g++ -llog4pi timedemo.cpp -o timedemo
 */

using namespace common::utility;

Logger logger{"main"};

int main(int argc, char **argv) {

    int64_t start=currentTimeMillis();
    logger.info("start:   %13lld", start);

    usleep(250*1000);

    int64_t end=currentTimeMillis();
    logger.info("end:     %13lld", end);

    long elapsed = end-start;
    logger.info("elapsed: %13ld",  elapsed);
}

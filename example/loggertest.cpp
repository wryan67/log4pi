#include <stdio.h>
#include <uuid/uuid.h>
#include <iostream>
#include <vector>
#include <thread>

// use angle brackets for normal use
#include <log4pi.h>

// use these for log4pi development only
// #include "Logger.h"
// #include "Common.h"

using namespace common;
using namespace common::utility;
using namespace common::synchronized;

void txop(int id);

int main(int argc, char **argv) {
    Logger logger("main");

    initRandom();

    if (argc<2) {
        logger.error("usage: LogLevel [threads]");
        logger.info("Log levels: %s", join(&Logger::logLevelNames,",").c_str());
        return EXIT_FAILURE;
    }

    logger.info("default global level: %s", logger.globalLevel().c_str());
  
    LogLevel level = Logger::str2level(argv[1]);
    
    logger.info("setting global level to %d (%s)", level, Logger::toString(level).c_str());

    logger.setGlobalLevel(level);

    logger.info("global level: %s", logger.globalLevel().c_str());

    logger.warn("----warn test----" ); fflush(stdout);
    logger.error("----error test----" ); fflush(stdout);
    logger.debug("----debug test----" ); fflush(stdout);
    logger.info("----thread test----" ); fflush(stdout);

    logger.info("%s","setting rolling log file masq: logger.out.%Y-%m-%d-%H-%M-%S");
    fflush(stderr);

    Logger::useRollingFile("logger.out.%Y-%m-%d-%H-%M-%S");

    try {
        int threadCount=10;
        if (argc>2) {
            threadCount=atoi(argv[2]);
        }
        vector<thread*> threads;
        for (int i=0;i<threadCount;++i) {
            thread *bgThread = new thread(txop,i);
            threads.push_back(bgThread);
            if (i%(threadCount/10)==0) usleep(randomInclusive(1000,1000000));
        }

        for (auto bgThread: threads) {
            bgThread->join();
        }
        exit(0);    
    } catch (system_error &e) {
        logger.error("threading issue: %s", e.what());
        exit(2);    
    }
}

void txop(int id) {   
    Logger logger("txop");

    usleep(randomInclusive(1000,1000000));

    string txid = getUUID();

    logger.setTransactionId(txid);

    logger.info("id=%04d txop=%s", id, txid.c_str());

    logger.clearTransactionId();
}

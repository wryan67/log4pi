#include <stdio.h>
#include <uuid/uuid.h>
#include <iostream>
#include <vector>
#include <thread>

#include <log4pi.h>

//#include "Common.h"
//#include "Logger.h"

using namespace common;
using namespace common::utility;
using namespace common::synchronized;

void txop(int id);

int main(int argc, char **argv) {
    Logger logger("main");
  
    if (argc<2) {
        logger.error("usage: test Level [threads]");
        logger.info("levels: %s", join(&Logger::logLevelNames,",").c_str());
        return EXIT_FAILURE;
    }

   
    printf("default global level: %s\n", logger.globalLevel().c_str());
  
    LogLevel level = logger.str2level(argv[1]);
    
    printf("setting global level to %d (%s)\n", level, Logger::toString(level).c_str());

    logger.setGlobalLevel(level);

    printf("global level: %s\n", logger.globalLevel().c_str());

    printf("----thread test----\n" ); fflush(stdout);

    try {
        int threadCount=10;
        if (argc>2) {
            threadCount=atoi(argv[2]);
        }
        vector<thread*> threads;
        for (int i=0;i<threadCount;++i) {
            thread *bgThread = new thread(txop,i);
            threads.push_back(bgThread);
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

    string txid = getUUID();

    logger.setTransactionId(txid);

    logger.info("id=%04d txop=%s", id, txid.c_str());

    logger.clearTransactionId();
}


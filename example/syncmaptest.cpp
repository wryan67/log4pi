/* SynchronizedMap
** Functional Test
** g++ -O2 -Wall -std=c++11 test.cpp -o test
*/

#include <log4pi.h>

//#include "Logger.h"
//#include "SynchronizedMap.h"

using namespace std;
using namespace common::utility;
using namespace common::synchronized;

int main(int argc, char **argv) {
    Logger logger("synctest");

    SynchronizedMap<int, string> activeAssociations;

    activeAssociations.put({101, "red"});
    activeAssociations.put({102, "blue"});
    activeAssociations.put({102, "green"});
    activeAssociations.put({104, "purple"});
    activeAssociations.put({105, "yellow"});
    activeAssociations.remove(104);

    logger.info(".getOrDefault(102)= %s", activeAssociations.getOrDefault(102, "unknown").c_str());
    logger.info(".getOrDefault(112)= %s", activeAssociations.getOrDefault(112, "unknown").c_str());

    if (!activeAssociations.contains(104)) {
        logger.info("%d does not exist",104);
    }
    if (activeAssociations.contains(101)) {
        logger.info("%d exists",101);
    }

    logger.info("--- associations: ---");
    for (auto e: activeAssociations.associations()) {
        logger.info("%d=%s", e.first, e.second.c_str());
    }
}

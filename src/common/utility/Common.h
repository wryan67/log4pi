#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>

#include "Logger.h"
#include "StringUtil.h"
#include "RuntimeException.h"
#include "Join.h"

using namespace std;

// #define vectorContainsObjectEle(v,e) (std::find(v.begin(), v.end(), e)!=v.end())
#define vectorContainsElement(v,e) (std::find((v).begin(), (v).end(), e)!=(v).end())
#define vectorCopyAllElements(s,t)  {for(auto e:s){t.push_back(e);}}
#define vectorCopyElements(s,t,f,v) {for(auto e:s){if (t->f == v){t.push_back(e);}}}
#define vectorExtract(vTarget, vSource, eField) \
            for (auto e: vSource) { vTarget.push_back(eField); } 

namespace common { namespace utility {

    uint64_t currentTimeMillis();
   
    void bomb(int exitCode, const char *format, ...);

    void initRandom();
    int  randomInclusive(int low, int high);

    string getUUID();

}}

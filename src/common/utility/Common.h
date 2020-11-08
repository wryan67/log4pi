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

namespace common { namespace utility {

    uint64_t currentTimeMillis();
   
    void bomb(int exitCode, const char *format, ...);

    string getUUID();

}}

#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include "StringBuilder.h"

using namespace std;

namespace common { namespace utility {

    string join(vector<string> *input, string separator);


}}

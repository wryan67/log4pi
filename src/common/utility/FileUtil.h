#pragma once

#include <stdio.h>
#include <vector>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

using namespace std;

namespace common { namespace utility {

    struct strLine {
        int   len;
        char *data;
    };


    class FileUtil {


    private:
        long maxBytes   = LONG_MAX;
        long maxRecords = LONG_MAX;
        int  maxLineSize= 32767;

    public:
        FileUtil(long maxBytes, long maxRecords, int maxLineSize);

        long read2vector(vector<strLine*> &myVector, FILE *inputFile, bool closeFile);


    };
}}

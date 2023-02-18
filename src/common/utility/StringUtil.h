#pragma once

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <string>
#include <vector>

using namespace std;


namespace common { namespace utility {

    struct splitFieldType {
        char *field;
        unsigned int   maxLength;
    };

    bool isBlankOrNull(const char *);

    string& toUpper(string &x);
    string& toLower(string &x);


    int xstrcmp(const void *a, const void *b);

    int strnchr(char *s, char c);

    int strsplit(splitFieldType *fields, int expected, const char *input, const char *fieldSeparator, void (*softError)(int fieldNumber,int expected,int actual));

    void ignoreSplitSoftError(int fieldNumber, int expected, int actual);

    int strsplit(const char *input, int expected, const char *fieldSeparator, ...);

    string strprintf(const char *format, ...);

    string strvsprintf(const char *format, va_list args);    


    vector<string> split(const string &s, char delim);

}}

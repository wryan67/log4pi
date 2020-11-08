#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include <mutex>

using namespace std;

namespace common { namespace utility {

    class StringBuilder {
    protected:
        vector<string> userCache;
        StringBuilder(vector<string> cache);
        int indentLevel=0;
        char *myBytes=nullptr;
        void freeCstr();
        mutex lockCstr;
        static const string emptyStr;
        static const char*  emptyCStr;
        
    public:
        StringBuilder();
        ~StringBuilder();

        string endl="\n";
        string toString();
        int write(FILE *file);

        virtual StringBuilder& nl();
        virtual StringBuilder& indent();
        virtual StringBuilder& indent(int level);
        virtual StringBuilder& setIndentLevel(int value);

        StringBuilder& append(int value);
        StringBuilder& append(long value);
        StringBuilder& append(char value);
        StringBuilder& append(const char* value);
        StringBuilder& append(string value);
        StringBuilder& appendf(const char *format, ...);

        StringBuilder& undo();
        StringBuilder& pop();
        
        const char* c_str();

        operator const string();

    };

}}

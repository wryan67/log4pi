
#include "StringBuilder.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <StringUtil.h>
#include "Logger.h"

using namespace std;

namespace common { namespace utility {

    const string StringBuilder::emptyStr;
    const char  *StringBuilder::emptyCStr = "";

    StringBuilder::StringBuilder(){}
    StringBuilder::StringBuilder(vector<string> cache){
        this->userCache=cache;
    }

    StringBuilder::~StringBuilder() {
        freeCstr();
    }
    
    string StringBuilder::toString() {

        if (userCache.size()==0) {
            return emptyStr;
        }
        
        string out;

        for (string _s: userCache) {
            out+=_s;
        }
        return out;
    }
    
    const char* StringBuilder::c_str() {
        lockCstr.lock();

        if (myBytes!=nullptr) {
            return myBytes;
        }

        if (userCache.size()==0) {
            return emptyCStr;
        }


        string str=toString();

        if (str.length()==0) {
            return emptyCStr;
        }

        myBytes=(char*)malloc(str.size()+1);
        strcpy(myBytes,str.c_str());

        lockCstr.unlock();
        return myBytes;
    }


    StringBuilder::operator const string() {
        return toString();
    }

    int StringBuilder::write(FILE *stream) {
        int rs;
        int bytes=0;

        for (string _s: userCache) {
            rs=fputs(_s.c_str(), stream);            
            if (rs==EOF) {
                return EOF;
            }
            bytes+=rs;
        }
        return bytes;        
    }
    


    void StringBuilder::freeCstr() {
        lockCstr.lock();
        if (myBytes!=nullptr) {
            free(myBytes);
            myBytes=nullptr;
        }
        lockCstr.unlock();
    }

    StringBuilder& StringBuilder::pop() {
        if (myBytes!=nullptr) freeCstr();
        userCache.erase(userCache.begin());
        return *this;
    }


    StringBuilder& StringBuilder::undo() {
        if (myBytes!=nullptr) freeCstr();
        if (userCache.size()>0) {
            userCache.pop_back();
        }
        return *this;
    }

    StringBuilder& StringBuilder::append(int value) {
        if (myBytes!=nullptr) freeCstr();
        userCache.push_back(to_string(value));
        return *this;
    }
    StringBuilder& StringBuilder::append(long value) {
        if (myBytes!=nullptr) freeCstr();
        userCache.push_back(to_string(value));
        return *this;
    }
    StringBuilder& StringBuilder::append(char value) {
        if (myBytes!=nullptr) freeCstr();
        char tmpstr[2];
        sprintf(tmpstr,"%c",value);
        userCache.push_back(tmpstr);
        return *this;
    }
    StringBuilder& StringBuilder::append(const char *value) {
        if (myBytes!=nullptr) freeCstr();
        userCache.push_back(string(value));
        return *this;
    }
    StringBuilder& StringBuilder::append(string value) {
        if (myBytes!=nullptr) freeCstr();
        userCache.push_back(value);
        return *this;
    }
    StringBuilder& StringBuilder::appendf(const char *format, ...) {
        if (myBytes!=nullptr) freeCstr();
        va_list args;
        va_start(args, format);
        
        string s=strvsprintf(format, args);
        append(s);

        va_end(args);
        return *this;
    }

    StringBuilder& StringBuilder::indent() {
        if (myBytes!=nullptr) freeCstr();
        for (int i=0;i<indentLevel;++i) {
            append("    ");
        }
        return *this;
    }

    StringBuilder& StringBuilder::indent(int level) {
        if (myBytes!=nullptr) freeCstr();
        for (int i=0;i<level;++i) {
            append("    ");
        }
        return *this;
    }


    StringBuilder& StringBuilder::nl() {
        if (myBytes!=nullptr) freeCstr();
        userCache.push_back(endl);
        return *this;
    }

    StringBuilder& StringBuilder::setIndentLevel(int value) {
        this->indentLevel=value;
        return *this;
    }

}}

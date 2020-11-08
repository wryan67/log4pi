
#include "StringUtil.h"

#include <sstream>
#include <locale>

using namespace std;

#ifndef null
#define null 0
#endif

namespace common { namespace utility {

    bool isBlankOrNull(const char *str) {
        if (str==nullptr) {
            return true;
        }
        int len=strlen(str);

        if (len==0) {
            return true;
        }

        for (int i=0;i<len;++i) {
            if (!isspace(str[i])) {
                return false;
            }
        }
        return true;
    }   


    int xstrcmp(const void *a, const void *b) {
        const char *pa = *(const char**)a;
        const char *pb = *(const char**)b;

        return strcmp(pa,pb);
    }

    int strnchr(char *s, char c) {
        char *f=strchr(s,c);
        if (f==NULL) {
            return -1;
        }
        return f-s;
    }


    void ignoreSplitSoftError(int fieldNumber, int expected, int actual) {
    }
    

    int strsplit(splitFieldType *fields, int expected, const char *input, const char *fieldSeparator, void (*softError)(int fieldNumber,int expected,int actual))  {
        int i;
        int fieldSeparatorLen=strlen(fieldSeparator);
        const char *tNext, *tLast=input;

        for (i=0; i<expected && (tNext=strstr(tLast, fieldSeparator))!=NULL; ++i) {
            unsigned int len=tNext-tLast;
            if (len>=fields[i].maxLength) {
                softError(i,fields[i].maxLength-1,len);
                len=fields[i].maxLength-1;
            }
            fields[i].field[len]=0;
            strncpy(fields[i].field,tLast,len);
            tLast=tNext+fieldSeparatorLen;
        }
        if (i<expected) {
            if (strlen(tLast)>fields[i].maxLength) {
                softError(i,fields[i].maxLength,strlen(tLast));
            } else {
                strcpy(fields[i].field,tLast);
            }
            return i+1;
        } else {
            return i;
        }
    }

    vector<string> split(const string &s, char delim) {
        vector<string> result;
        stringstream ss (s);
        string item;

        while (getline (ss, item, delim)) {
            result.push_back (item);
        }

        return result;
    }

    string strvsprintf(const char *format, va_list args) {

        int maxlen= strlen(format)+16384;
        char *tmpstr=(char*)malloc(maxlen);

        const char *overflow="::message exceeds maximum allowed size";

        vsnprintf(tmpstr,maxlen,format,args);
        string rs=string(tmpstr);

        if (strlen(tmpstr)>(maxlen-strlen(overflow)-1)) {
            strcpy(&tmpstr[maxlen-strlen(overflow)-1],overflow);
            tmpstr[maxlen-1]=0;
        }

        free(tmpstr);

        return rs;
    }

    string strprintf(const char *format, ...) {
        va_list args;
        va_start(args, format);

        int maxlen= strlen(format)+16384;
        char *tmpstr=(char*)malloc(maxlen);

        vsnprintf(tmpstr,maxlen,format,args);
        string rs=string(tmpstr);

        free(tmpstr);
        va_end(args);

        return rs;
    }

    int strsplit(const char *input, int expected, const char *fieldSeparator, ...) {
    va_list args;
    va_start(args, fieldSeparator);

    const char *last=input;
    const char *next; 

    int ct=0;
    while (ct<expected && (next=strstr(last, fieldSeparator))!=NULL) {
        char *target=va_arg(args, char *);
        if (target!=NULL) {
        strncpy(target,last,next-last);
        target[next-last]=0;
        ++ct;
        }
        last=next+2;
    }
    if (ct<expected) {
        char *target=va_arg(args, char *);
        if (target!=NULL) {
        strcpy(target,last);
        }
    }
    
    va_end(args);
    return ct+1;
    }

    string& toUpper(string &str) {
        for (auto & c: str) c = toupper(c);
        return str;
    }
    string& toLower(string &str) {
        for (auto & c: str) c = tolower(c);
        return str;
    }

}}

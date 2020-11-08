#include "Logger.h"
#include "strings.h"
#include <thread>
#include <sstream>

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "StringUtil.h"

namespace common { namespace utility {

    LogLevel Logger::globalLogLevel=ALL;

    vector<string> Logger::logLevelNames = {
        "ALL", "TAG", "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
    };

    SynchronizedMap<thread::id, string> Logger::transactionId;


    Logger::Logger(const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        this->name = strvsprintf(format, valist); 

        va_end(valist);
    }

    Logger::Logger(string name) {
        this->name = name;
    }
    Logger::Logger(string name, LogLevel level) {
        this->name = name;
        this->logLevel = level;
    }

    void Logger::setGlobalLevel(LogLevel level) {
        Logger::globalLogLevel = level;
    }

    LogLevel Logger::getGlobalLevel() {
        return Logger::globalLogLevel;
    }

    string Logger::globalLevel() {
        return logLevelNames[Logger::globalLogLevel];
    }

    string Logger::toString(LogLevel level) {
        return logLevelNames[level];
    }

    LogLevel Logger::str2level(char *str) {
        thread::id threadId = this_thread::get_id();

        if (str==nullptr) {
            string txid=getTransactionId();
            Logger::logStream("common::str2level", threadId, stderr, ERROR, "str2level called with null string", nullptr);
            return ALL;
        }
        
        string s = string(str);
        string2upper(s);

        for (int i=0; i<(int)logLevelNames.size();++i) {
            if (s==logLevelNames.at(i)) return (LogLevel)i;            
        }
        
        string txid=getTransactionId();

        string msg = string("str2level called with unknown string: ") + s;
        Logger::logStream("common::str2level", threadId, stderr, ERROR, msg.c_str(), nullptr);
        return ALL;
    }


    void Logger::vtag(int number, const char *format, va_list *valist) {
        char *fmt = (char*)malloc(strlen(format)+16);
       
        sprintf(fmt,"%03d: %s", number, format);
        log(ERROR, fmt, valist);
        free(fmt);
    }
    void Logger::vtrace(const char *format, va_list *valist) {
        log(ERROR, format, valist);
    }
    void Logger::vdebug(const char *format, va_list *valist) {
        log(ERROR, format, valist);
    }
    void Logger::vinfo(const char *format, va_list *valist) {
        log(ERROR, format, valist);
    }
    void Logger::vwarn(const char *format, va_list *valist) {
        log(ERROR, format, valist);
    }
    void Logger::verror(const char *format, va_list *valist) {
        log(ERROR, format, valist);
    }
    void Logger::vfatal(const char *format, va_list *valist) {
        log(ERROR, format, valist);
    }

    void Logger::tagStream(int number, const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        char *fmt = (char*)malloc(strlen(format)+16);
        sprintf(fmt,"%03d: %s", number, format);

        thread::id threadId = this_thread::get_id();

        logStream("TAG", threadId, stderr, TAG, format, &valist); 
        fflush(stderr);

        free(fmt);
        va_end(valist);
    }

    void Logger::tag(int number, const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        char *fmt = (char*)malloc(strlen(format)+16);
        
        sprintf(fmt,"%03d: %s", number, format);

        log(TAG, fmt, &valist); fflush(stderr);
        free(fmt);
        va_end(valist);
    }

    void Logger::trace(const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        log(TRACE, format, &valist);
        va_end(valist);
    }

    void Logger::debug(const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        log(DEBUG, format, &valist);  fflush(stderr);
        va_end(valist);
    }

    void Logger::info(const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        log(INFO, format, &valist);
        va_end(valist);
    }

    void Logger::warn(const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        log(WARN, format, &valist);
        va_end(valist);
    }

    void Logger::error(const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        log(ERROR, format, &valist); fflush(stderr);
        va_end(valist);
    }

    void Logger::fatal(const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        log(FATAL, format, &valist); fflush(stderr);
        va_end(valist);
    }

    string Logger::getTransactionId() {
        thread::id threadId = this_thread::get_id();
 
        return Logger::transactionId.getOrDefault(threadId, "");
    }

    string Logger::getTransactionId(thread::id threadId) {
        return Logger::transactionId.getOrDefault(threadId, "");
    }

    void Logger::setTransactionId(string txid) {
        thread::id threadId = this_thread::get_id();
        
        Logger::transactionId.put({threadId, txid});
    }     

    void Logger::clearTransactionId() {
        thread::id threadId = this_thread::get_id();
        
        Logger::transactionId.remove(threadId);
    }     

    void Logger::flush() {
        fflush(stderr);
    }


    void Logger::logfd(string name, thread::id threadId, int fd, LogLevel level, const char *format, va_list *valist) {
        FILE  *pipe;
        pipe=fdopen(fd, "w");
        if (pipe==nullptr) {
            Logger::punt(name, threadId, strerror(errno), level, format, valist);
        } else {
            Logger::logStream(name, threadId, pipe, level, format, valist);
            fclose(pipe);
        }
    }

    void Logger::logStream(string name, thread::id threadId, FILE *pipe, LogLevel level, const char *format, va_list *valist) {
        char   timestamp[24];
        struct timeval currentTime;

        gettimeofday(&currentTime, nullptr);
        strftime(timestamp, sizeof(timestamp), "%F %T", localtime(&currentTime.tv_sec));

        stringstream threadSS;
        threadSS << threadId;
        string tid = threadSS.str();
        
        string txid = Logger::getTransactionId(threadId);
        if (Logger::globalLogLevel <= level) {
            fprintf(pipe, "[%s] %s.%03ld %-5s ", 
                        txid.c_str(), 
                        timestamp, currentTime.tv_usec/1000,
                        Logger::toString(level).c_str()
            );
            if (valist==nullptr) {
                fprintf(pipe, "%s", format);
            } else {
                vfprintf(pipe, format, *valist);
            }

            fprintf(pipe, " (%s)(%s)\n",name.c_str(),tid.c_str());
        }
    }

    void Logger::punt(string name, thread::id threadId, char *sysError, LogLevel level, const char *format, va_list *valist) {
        Logger::logStream(name, threadId, stderr, level, format, valist);
        fprintf(stderr, "ERROR: %s", sysError); 
        fflush(stderr);
    }

    void Logger::log(LogLevel level, const char *format, va_list *valist) {
        thread::id threadId = this_thread::get_id();

        int pipefds[2];
        int rs=pipe(pipefds);
        if (rs!=EXIT_SUCCESS) {
            Logger::punt(name, threadId, strerror(errno), level, format, valist);
            return;
        }

 
        FILE *pipe = fdopen(pipefds[0], "r");
        if (pipe==nullptr) {
            Logger::punt(name, threadId, strerror(errno), level, format, valist);
            return;
        } 

        thread(logfd, name, threadId, pipefds[1] ,level, format, valist).detach();
       

        vector<logLine*> lineio;

        int size = read2vector(lineio, pipe);
        fclose(pipe);
                
        char *out = (char *)malloc(size);

        int pos=0;
        for (logLine *line: lineio) {
            memcpy(&out[pos], line->data, line->len);
            pos+=line->len;           
            free(line->data);
            free(line);
        }        
        lineio.clear();
        fwrite(out, size, 1, stderr);
        if (level==TAG) {
            fflush(stderr);
        }
        free(out);
    }

    int Logger::read2vector(vector<logLine*> &myVector, FILE *inputFile) {
        if (feof(inputFile)) {
            fprintf(stderr,"pipe is closed before reading\n"); fflush(stderr);
        }

        int bytes=0;
        while (!feof(inputFile)) {
            char buf[maxLineSize];
            int bytesRead = fread(buf, 1, sizeof(buf), inputFile);

            if (bytesRead>0) {
                logLine *line = (logLine *) malloc(sizeof(logLine));
                line->len = bytesRead;
                line->data = (char*)malloc(bytesRead);
                memcpy(line->data,buf,bytesRead);
                myVector.push_back(line);
                bytes+=bytesRead;
            }
            if (bytes > maxBytes) {
                break;
            }
            if ((int)myVector.size() > maxRecords) {
                break;
            }
        }

        return bytes;
    }   
}}

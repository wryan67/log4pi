#pragma once

#include <string>
#include <vector>
#include <thread>
#include <iostream>
#include <mutex>
#include <atomic>

#include "SynchronizedMap.h"


using namespace std;
using namespace common::synchronized;

namespace common { namespace utility {

    enum LogLevel {
//      0    1    2      3      4     5     6      7        
        ALL, TAG, TRACE, DEBUG, INFO, WARN, ERROR, FATAL
    };

   
    struct logLine {
        int   len;
        char *data;
    };

    class Logger {

        public:
            Logger(const char *name, ...);
            Logger(string name);
            Logger(string name, LogLevel level);
            static vector<string> logLevelNames;

            static string   toString(LogLevel level);
            static LogLevel str2level(char *str);
            static LogLevel int2level(int level);
            static string   globalLevel();

            static void     setGlobalLevel(LogLevel level);
            static LogLevel getGlobalLevel();
            static bool     getGlobalAnalysis();
            static void     setGlobalAnalysis(bool level);
            static bool     getUsePunt();
            static void     setUsePunt(bool value);          
            static void     useRollingFile(string masq);
            static mutex    writeLock;


            void setOptions(LogLevel level, bool analysis);
            void vtag(int number, const char *format, va_list *valist);
            void vtrace(const char *format, va_list *valist);
            void vdebug(const char *format, va_list *valist);
            void vinfo(const char *format, va_list *valist);
            void vwarn(const char *format, va_list *valist);
            void verror(const char *format, va_list *valist);
            void vfatal(const char *format, va_list *valist);

            void tag(int number, const char *format, ...);
            void trace(const char *format, ...);
            void debug(const char *format, ...);
            void info(const char *format, ...);
            void warn(const char *format, ...);
            void error(const char *format, ...);
            void fatal(const char *format, ...);

            static string getTransactionId();
            static string getTransactionId(thread::id threadId);
            static void   setTransactionId(string id);
            static void   clearTransactionId();

            static void logStream(string name, thread::id threadId, FILE *stream, LogLevel level, const char *format, va_list *valist);
            static void tagStream(int number, const char *format, ...);

            void commonTiming(const char* method, uint64_t start);

        private:        
            int    maxBytes        = 2147483647/2;
            int    maxRecords      = 2147483647;
            int    maxLineSize     = 1024;
            LogLevel logLevel = ALL;

            static FILE         *output;
            static mutex        fileRollingLock;
            static thread      *fileRollingThread;
            static bool         time2roll;
            static string       logMasq;
            static atomic<long> logEpoch;
            
            static void     logRoller(string masq);
            static void     rollFile();
            static void     rollClose(FILE *out);

            static bool     outputAnalysis;
            static bool     usePunt;
            static LogLevel globalLogLevel;
            static SynchronizedMap<thread::id, string> transactionId;

            string name;
            void        log(LogLevel level, const char *format, va_list *valist);
            static void punt(string name, thread::id threadId, char *sysError, LogLevel level, const char *format, va_list *valist);
            static void logfd(string name, thread::id threadId, int fd, LogLevel level, const char *format, va_list *valist);
            int         read2vector(vector<logLine*> &myVector, FILE *inputFile);

    };
}}

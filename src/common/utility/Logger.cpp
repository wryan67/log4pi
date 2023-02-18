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
#include "Common.h"

#define classFQName "ccsi::common::utility::Logger"
#define string2upper(str)  for (auto & c: str) c = toupper(c);

namespace common { namespace utility {

    LogLevel Logger::globalLogLevel=ALL;
    bool     Logger::outputAnalysis=true;
    bool     Logger::usePunt=false;
    FILE*    Logger::output=stderr;
    mutex    Logger::fileRollingLock;
    thread  *Logger::fileRollingThread=nullptr;
    mutex    Logger::writeLock;
    bool     Logger::time2roll;
    string   Logger::logMasq;
    atomic<long> Logger::logEpoch{0};


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

    void Logger::setOptions(LogLevel level, bool analysis) {
        this->outputAnalysis = analysis;
        LogLevel newLevel=level;
        if (analysis && level>=INFO) {
            newLevel=INFO;
        }
        this->logLevel = newLevel;
    }

    bool Logger::getGlobalAnalysis() {
        return Logger::outputAnalysis;
    }
    void Logger::setGlobalAnalysis(bool value) {
        if (value && globalLogLevel>=INFO) {
            globalLogLevel=INFO;
        }
        Logger::outputAnalysis = value;
    }

    bool Logger::getUsePunt() {
        return Logger::usePunt;
    }
    void Logger::setUsePunt(bool value) {
        Logger::usePunt = value;
    }

    LogLevel Logger::getGlobalLevel() {
        return Logger::globalLogLevel;
    }
    void Logger::setGlobalLevel(LogLevel level) {
        Logger::globalLogLevel = level;
    }

    string Logger::globalLevel() {
        return logLevelNames[Logger::globalLogLevel];
    }

    string Logger::toString(LogLevel level) {
        return logLevelNames[level];
    }

    LogLevel Logger::int2level(int level) {
        switch (level) {
            case 0: return ALL;
            case 1: return TAG;
            case 2: return TRACE;
            case 3: return DEBUG;
            case 4: return INFO;
            case 5: return WARN;
            case 6: return ERROR;
            case 7: return FATAL;
            
            default:
                throw RuntimeException("invalid logging level %d",level);
                break;
        }
    }
    void Logger::rollClose(FILE *old) {
        if (old==stderr) {
            return;
        }
        usleep(5*1000*1000);
        fclose(old);
    }

    void Logger::rollFile() {
        char       filename[2048];
        time_t     now = logEpoch/1000;
        struct tm* tm_info = localtime(&now);

        strftime(filename, sizeof(filename), logMasq.c_str(), tm_info);
        FILE *out = fopen(filename, "a");
        FILE *save=out;

        if (out) {
            output = out;
        } else {
            thread::id threadId = this_thread::get_id();
            char message[4096];
            snprintf(message,sizeof(message)-2,"log rolling cannot open output file '%s'; caused by: %s", filename,  strerror( errno ));
            Logger::logStream("logger", threadId, stderr, ERROR, message, nullptr);           
        }
        thread(rollClose,save).detach();
    }

    void Logger::logRoller(string masq) {
        logMasq=masq;

        enum rolling {
//                0  1  2  3  4
            unknown,ss,mm,hh,dd
        };

        rolling rollingMethod=unknown;
        
        if (strstr(masq.c_str(),"%S")) {        rollingMethod=ss;
        } else if (strstr(masq.c_str(),"%M")) { rollingMethod=mm;
        } else if (strstr(masq.c_str(),"%H")) { rollingMethod=hh;
        } else if (strstr(masq.c_str(),"%d")) { rollingMethod=dd;
        }

        time_t epoch=currentTimeMillis();
        logEpoch=epoch;
        time2roll=true;

        while (true) {
            time_t     timer = epoch/1000;
            struct tm* tm_info = localtime(&timer);

            if (rollingMethod>ss) {
                --tm_info->tm_sec;

                switch (rollingMethod) {
                    case mm:  ++tm_info->tm_min;   break;
                    case hh:  ++tm_info->tm_hour;  break;
                    case dd:  ++tm_info->tm_mday;  break;
                    default: break;
                }
                long future = mktime(tm_info);
                long sleep = (future-timer)*1000000;
                usleep(sleep);
            }

            epoch = currentTimeMillis();

            usleep((998-(epoch%1000))*1000);

            do {
                usleep(333);
                epoch=currentTimeMillis();
            } while ((epoch%1000)>500);
            logEpoch=epoch;
            time2roll=true;
        }
    }

    void Logger::useRollingFile(string masq) {       
        fileRollingLock.lock();
        if (fileRollingThread==nullptr) {
            fileRollingThread = new thread(Logger::logRoller, masq);
        }
        fileRollingLock.unlock();
    }


    LogLevel Logger::str2level(char *str) {
        thread::id threadId = this_thread::get_id();

        if (str==nullptr) {
            string txid=getTransactionId();
            Logger::logStream(classFQName, threadId, output, ERROR, "str2level called with null string", nullptr);
            return ALL;
        }
        
        string s = string(str);
        string2upper(s);

        for (int i=0; i<(int)logLevelNames.size();++i) {
            if (s==logLevelNames.at(i)) return (LogLevel)i;            
        }
        
        string txid=getTransactionId();

        string msg = string("str2level called with unknown string: ") + s;
        Logger::logStream(classFQName, threadId, output, ERROR, msg.c_str(), nullptr);
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

        logStream("TAG", threadId, output, TAG, format, &valist); 

        free(fmt);
        va_end(valist);
    }

    void Logger::tag(int number, const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        char *fmt = (char*)malloc(strlen(format)+16);
        
        sprintf(fmt,"%03d: %s", number, format);

        log(TAG, fmt, &valist); 
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

        log(DEBUG, format, &valist);  
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

        log(ERROR, format, &valist); 
        va_end(valist);
    }

    void Logger::fatal(const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        log(FATAL, format, &valist);
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

        if (level >= Logger::globalLogLevel) {
            if (pipe==output) writeLock.lock();
            char   timestamp[24];
            struct timeval currentTime;

            gettimeofday(&currentTime, nullptr);
            strftime(timestamp, sizeof(timestamp), "%F %T", localtime(&currentTime.tv_sec));

            stringstream threadSS;
            threadSS << threadId;
            string tid  = threadSS.str();            
            string txid = Logger::getTransactionId(threadId);

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
            fflush(pipe);        
            if (pipe==output) writeLock.unlock();
        }
    }

    void Logger::punt(string name, thread::id threadId, char *sysError, LogLevel level, const char *format, va_list *valist) {

        if (level < Logger::globalLogLevel ) {
            return;
        }
        Logger::logStream(name, threadId, output, level, format, valist);
        writeLock.lock();
        fprintf(output, "ERROR: %s", sysError); 
        fflush(output);
        writeLock.unlock();
    }

    void Logger::log(LogLevel level, const char *format, va_list *valist) {

        if (level < Logger::globalLogLevel ) {
            return;
        }

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

        thread bgTask(logfd, name, threadId, pipefds[1] ,level, format, valist);
       

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
        writeLock.lock();
        if (time2roll) {
            rollFile();
            time2roll=false;
        }
        fwrite(out, size, 1, output);

        if (level==TAG || level>=ERROR) {
            fflush(output);
        }
        writeLock.unlock();
        free(out);
        bgTask.join();
    }

    int Logger::read2vector(vector<logLine*> &myVector, FILE *inputFile) {
        if (feof(inputFile)) {
            writeLock.lock();
            fprintf(output,"pipe is closed before reading\n"); 
            fflush(output);
            writeLock.unlock();
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

    void Logger::commonTiming(const char* method, uint64_t start) {
        if (outputAnalysis) {
            auto elapsed=currentTimeMillis()-start;
            info("CommonTiming %s() elapsed time %lu", method, elapsed);
        }
    }
}}

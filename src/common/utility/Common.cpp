
#include "Common.h"
#include <sys/time.h>
#include <uuid/uuid.h>
#include <sys/types.h>
#include "StringBuilder.h"

namespace common { namespace utility {

    uint64_t currentTimeMillis() {
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);

        return
            (uint64_t)(currentTime.tv_sec) * 1000 +
            (uint64_t)(currentTime.tv_usec) / 1000;
    }

    void bomb(int exitCode, const char *format, ...) {
        va_list valist;
        va_start(valist, format);

        vfprintf(stderr, format, valist);
        fprintf(stderr, "\n");
        fflush(stderr);
        va_end(valist);
        exit(exitCode);
    }

    string getUUID() {
        uuid_t uuid;

        char uuid_str[37];
        char uuid_nodash[37];

        uuid_generate_random(uuid);
        uuid_unparse_lower(uuid, uuid_str);

        int t=0;
        for (int k=0;uuid_str[k]>0;++k) {
            if (uuid_str[k]!='-') {
                uuid_nodash[t++]=uuid_str[k];
            }
        } uuid_nodash[t]=0;

        return string(uuid_nodash);
    }

}}

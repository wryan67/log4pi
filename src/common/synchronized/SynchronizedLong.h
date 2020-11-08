#pragma once

#include <mutex>

using namespace std;

namespace common { namespace synchronized {

    class SynchronizedLong {

        public:
            SynchronizedLong(long initialValue);

            long get();
            void set(long value);
            long increment();
            bool equals(long other);

        private:
            long _value=0;

            mutex sync;       
    };

}}

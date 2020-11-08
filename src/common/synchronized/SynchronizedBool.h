#pragma once

#include <mutex>

using namespace std;

namespace common { namespace synchronized {

    class SynchronizedBool {

        public:
            SynchronizedBool(bool initialValue);

            bool get();
            void set(bool value);

            bool commit(bool expected, bool newValue);
            bool equals(bool other);

        private:
            bool value=false;

            mutex sync;       
    };

}}

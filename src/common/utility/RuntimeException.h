#pragma once

#include <exception>
#include <string>

using namespace std;

namespace common { namespace utility {

    class RuntimeException: public exception
    {
        public:
            RuntimeException(const char *format, ...);

            virtual const char* what() const throw();
          
        private:
            string message;            

    };
}}

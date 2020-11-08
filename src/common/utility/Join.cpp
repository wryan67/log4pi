#include "Join.h"


namespace common { namespace utility {

    string join(vector<string> *input, string separator) {
        if (input==nullptr) {
            return "";
        }

        vector<string> out;

        for (string e: *input) {
            out.push_back(e);
            out.push_back(separator);
        }   out.pop_back();

        string rs;
        for (auto e: out) {
            rs+=e;
        }
        return rs;
    }

}}

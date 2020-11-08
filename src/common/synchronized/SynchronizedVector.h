#pragma once
/*
 * SynchronizedMap.h
 * Wade Ryan 20201022
 * c++11
 */

#include <mutex>
#include <vector>

#ifndef __SynchronizedVector__
#define __SynchronizedVector__
//#define ValueType string

using namespace std;

namespace common { namespace synchronized {

    template <typename ValueType>
    class SynchronizedVector {

    private:
        mutex sync;
        vector<ValueType> usermap;    

    public:
        ValueType get(int i) {
            sync.lock();
            return usermap.get[i];
            sync.unlock();
        }

        void push_back(ValueType value) {
            sync.lock();
            usermap.push_back(value);
            sync.unlock();
        }


        int size() {
            return usermap.size();
        }


    };       

}}

#endif


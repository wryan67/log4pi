#pragma once
/*
 * SynchronizedMap.h
 * Wade Ryan 20200926
 * c++11
 */

#include <unordered_map>
#include <mutex>
#include <vector>

#ifndef __SynchronizedMap__
#define __SynchronizedMap__

using namespace std;

namespace common { namespace synchronized {

    template <typename KeyType, typename ValueType>

    class SynchronizedMap {

    private:
        mutex sync;
        unordered_map<KeyType, ValueType> usermap;    

    public:
        ValueType getOrDefault(KeyType key, ValueType defaultValue) {
            sync.lock();

            ValueType rs;

            auto value=usermap.find(key);
            if (value == usermap.end()) {
                rs = defaultValue;
            } else {
                rs = value->second;
            }
            sync.unlock();
            return rs;
        }

        bool contains(KeyType key) {
            sync.lock();
            bool exists = (usermap.find(key) != usermap.end());
            sync.unlock();
            return exists;
        }


        void put(pair<KeyType, ValueType> nodePair) {
            sync.lock();

            if (usermap.find(nodePair.first) != usermap.end()) {
                usermap.erase(nodePair.first);
            }
            usermap.insert(nodePair);
            sync.unlock();
        }

        void put(KeyType key, ValueType value) {
            sync.lock();

            if (usermap.find(key) != usermap.end()) {
                usermap.erase(key);
            }
            usermap.insert({key, value});
            sync.unlock();
        }


        void remove(KeyType key) {
            sync.lock();

            if (usermap.find(key) != usermap.end()) {
                usermap.erase(key);
            }
            sync.unlock();
        }

        vector<pair<KeyType, ValueType>> associations() {
            sync.lock();
 
            vector<pair<KeyType, ValueType>> elements;

            for (auto it=usermap.begin(); it != usermap.end(); ++it) {
                pair<KeyType, ValueType> element (it->first, it->second);
                elements.push_back( element );
            }

            sync.unlock();
            return elements;
        }
    };       
}}

#endif


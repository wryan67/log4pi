
#include "SynchronizedLong.h"


namespace common { namespace synchronized {

    SynchronizedLong::SynchronizedLong(long initialValue) {
        this->_value = initialValue;    
    }

    long SynchronizedLong::get() {   
        return _value;
    }

    void SynchronizedLong::set(long newValue) {

        sync.lock();
        _value=newValue;
        sync.unlock();
        
        return;
    }

    long SynchronizedLong::increment() {
        long rs;

        sync.lock();
        rs = ++_value;
        sync.unlock();
        
        return rs;
    }

    bool SynchronizedLong::equals(long other){
        long rs;

        sync.lock();
        rs=(_value==other);
        sync.unlock();
        
        return rs;
    }
    
}}

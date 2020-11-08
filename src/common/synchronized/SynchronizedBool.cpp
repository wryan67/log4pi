
#include "SynchronizedBool.h"


namespace common { namespace synchronized {

    SynchronizedBool::SynchronizedBool(bool initialValue) {
        this->value = initialValue;    
    }

    bool SynchronizedBool::get() {   
        return value;
    }


    void SynchronizedBool::set(bool newValue) {

        sync.lock();
        value=newValue;
        sync.unlock();
        
        return;
    }

    bool SynchronizedBool::commit(bool expected, bool newValue) {
        bool rs = false;

        sync.lock();
        if (value==expected) {
            value=newValue;
            rs=true;
        }
        sync.unlock();
        
        return rs;
    }

    bool SynchronizedBool::equals(bool other){
        bool rs;

        sync.lock();
        rs=(value==other);
        sync.unlock();
        
        return rs;
    }
    
}}

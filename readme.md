log4pi
------

This software library provides a convient and standerdized way of logging information 
that your program is generating. Log entries are time stamped with the current time, 
including milliseconds. The log4pi library is thread safe, and provides a way to track log
entries such that one can easily separate log entries for different threads for analysis.

Different modes can be set such that one can turn debugging information on or off 
depending upon the current need or environment.  Logging levels are as follows:

    ALL,TAG,TRACE,DEBUG,INFO,WARN,ERROR,FATAL

## Dependencies
Please install the uuid library prior to compiling the source:

    $ sudo apt install uuid

## Download

    $ cd [your projects folder]
    $ git clone https://github.com/wryan67/log4pi.git

## Install
To compile this library, navigate into the src folder and use the make utility to compile 
and install the library.  Note you can also create the demo applicaions here a well.  
"make install" uses sudo to install the library in the /usr/local system area.

    $ cd [your projects folder]/log4pi
    $ make 
    $ make install
    $ make demos

## Compiling
Complie your applications using these command line arguments: -pthread -llog4pi
Compiling example:

    $ g++ -O2 -lpthread -llog4pi loggertest.cpp -o loggertest


## Examples
In the example folder, there is a sample C++ program called loggertest.cpp which 
will introduce you to using the logging methods in your programs.  The make demos 
command will place the binary executables for the demos programs into the bin folder.

    $ cd bin
    $ ./loggertest 
    [] 2020-11-09 00:02:19.924 ERROR usage: test Level [threads] (main)(3069311440)
    [] 2020-11-09 00:02:19.924 INFO  levels: ALL,TAG,TRACE,DEBUG,INFO,WARN,ERROR,FATAL (main)(3069311440)



## Copyright

Copyright (c) 2020 Wade Ryan

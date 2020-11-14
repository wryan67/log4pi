log4pi
------

This software library provides a convient and standardized way of logging information that your program is generating. Log entries are time stamped with the current time, including milliseconds. All logging methods use the printf() formatting convention.  The log4pi library is thread safe, and provides a way to track log entries such that one can easily separate log entries for different threads for analysis.  All log entries are written to stderr so that stdout can be used for normal file i/o.  

Code Snippet with sample output:

    int id=72;
    char *state="running";
    
    logger.info("id=%04d state=%s", id, state);
    
sample output:

    [] 2020-11-02 14:28:32.027 INFO  id=0072 state=running (main)(3069499856)
    
    

Different modes, sometimes referred to as logging level, can be set such that one can turn debugging information on or off depending upon the current need or environment.  Logging levels are as follows:

    ALL,TAG,TRACE,DEBUG,INFO,WARN,ERROR,FATAL

## TO-DO

* Write logs to disk instead of stderr
  * Custom mask for file naming convention
  * Roll logs daily or hourly
* Common-Timing: create standard classes for capturing elapsed times for functions. 
  * Hold logging in memory until transacton completes
  * Manual logging
  * Create start/stop triggers to enable aggregate computations for:
    * average elapsed time
    * standard deviation
    * 95th percentile
* Transactions Per Second (TPS) monitoring.  Compute:
  * Exact TPS
  * TPS: 20 per moving average
  * Average elapsed time per second
* Compute total number of arguments and adjust sprintf() format accordingly.

## Dependencies
Please install the uuid development library prior to compiling the source:

    $ sudo apt install uuid-dev

## Download

    $ cd [your projects folder]
    $ git clone https://github.com/wryan67/log4pi.git

## Install
To compile this library, navigate into the src folder and use the make utility to compile and install the library.  Note you can also create the demo applicaions here a well.  "make install" uses sudo to install the library in the /usr/local system area.

    $ cd [your projects folder]
    $ cd log4pi
    $ make 
    $ make install
    $ make demos

## Compiling
Complie your applications using these command line arguments: -pthread -llog4pi
Compiling example:

    $ g++ -O2 -lpthread -llog4pi loggertest.cpp -o loggertest


## Examples
In the example folder, there is a sample C++ program called loggertest.cpp which will introduce you to using the logging methods in your programs.  The make demos command will place the binary executables for the demos programs into the bin folder.

Notice that as the log level progresses through the defined types that the amount of messages being printed automaically decrease per the level being set on the command line:

    $ cd bin
    $ ./loggertest 
    [] 2020-11-09 00:02:19.924 ERROR usage: test Level [threads] (main)(3069311440)
    [] 2020-11-09 00:02:19.924 INFO  levels: ALL,TAG,TRACE,DEBUG,INFO,WARN,ERROR,FATAL (main)(3069311440)


    $ ./loggertest all 2>&1 | awk '{printf("    %s\n",$0)}'
    [] 2020-11-09 00:38:14.075 INFO  default global level: ALL (main)(3069606352)
    [] 2020-11-09 00:38:14.076 INFO  setting global level to 0 (ALL) (main)(3069606352)
    [] 2020-11-09 00:38:14.077 INFO  global level: ALL (main)(3069606352)
    [] 2020-11-09 00:38:14.077 WARN  ----warn test---- (main)(3069606352)
    [] 2020-11-09 00:38:14.077 ERROR ----error test---- (main)(3069606352)
    [] 2020-11-09 00:38:14.078 DEBUG ----debug test---- (main)(3069606352)
    [] 2020-11-09 00:38:14.078 INFO  ----thread test---- (main)(3069606352)
    [0fff0748ef014f339ac6a1bc7557f50b] 2020-11-09 00:38:14.080 INFO  id=0002 txop=0fff0748ef014f339ac6a1bc7557f50b (txop)(3038762000)
    [9769e8066ac8425abe10209a9600532b] 2020-11-09 00:38:14.080 INFO  id=0001 txop=9769e8066ac8425abe10209a9600532b (txop)(3047154704)
    [3e8aee70637b45cbb1e3ff34545ef900] 2020-11-09 00:38:14.080 INFO  id=0003 txop=3e8aee70637b45cbb1e3ff34545ef900 (txop)(3028284432)
    [a740d13c3d104cdc8e5d24f5400b52c4] 2020-11-09 00:38:14.081 INFO  id=0005 txop=a740d13c3d104cdc8e5d24f5400b52c4 (txop)(2998924304)
    [9d575d0802bc490ba97ba237d4edf07d] 2020-11-09 00:38:14.082 INFO  id=0007 txop=9d575d0802bc490ba97ba237d4edf07d (txop)(2984244240)
    [dcc23fa7c4094d8b829b3f32bc3d9387] 2020-11-09 00:38:14.082 INFO  id=0009 txop=dcc23fa7c4094d8b829b3f32bc3d9387 (txop)(2959066128)
    [de0a326bebf74df18438f737e4957120] 2020-11-09 00:38:14.082 INFO  id=0006 txop=de0a326bebf74df18438f737e4957120 (txop)(3055547408)
    [887c87278e7b487ca6924ee225cdebae] 2020-11-09 00:38:14.082 INFO  id=0004 txop=887c87278e7b487ca6924ee225cdebae (txop)(3009405968)
    [2a9f013a0fe5421a8f71c10139ea0126] 2020-11-09 00:38:14.083 INFO  id=0008 txop=2a9f013a0fe5421a8f71c10139ea0126 (txop)(2975851536)
    [b569c681b38c4d8dbf279f9c9ee9110b] 2020-11-09 00:38:14.083 INFO  id=0000 txop=b569c681b38c4d8dbf279f9c9ee9110b (txop)(3065431056)

    $ ./loggertest info 2>&1 | awk '{printf("    %s\n",$0)}'
    [] 2020-11-09 00:38:32.027 INFO  default global level: ALL (main)(3069499856)
    [] 2020-11-09 00:38:32.028 INFO  setting global level to 4 (INFO) (main)(3069499856)
    [] 2020-11-09 00:38:32.028 INFO  global level: INFO (main)(3069499856)
    [] 2020-11-09 00:38:32.029 WARN  ----warn test---- (main)(3069499856)
    [] 2020-11-09 00:38:32.029 ERROR ----error test---- (main)(3069499856)
    [] 2020-11-09 00:38:32.030 INFO  ----thread test---- (main)(3069499856)
    [9c9860c817cc40bbb64c0950828d62b4] 2020-11-09 00:38:32.032 INFO  id=0000 txop=9c9860c817cc40bbb64c0950828d62b4 (txop)(3065324560)
    [6b60608c5a6147ea8b17a68419aefe76] 2020-11-09 00:38:32.033 INFO  id=0001 txop=6b60608c5a6147ea8b17a68419aefe76 (txop)(3045061648)
    [07e3970dc3ff45f78bb1c3f9a20536d7] 2020-11-09 00:38:32.034 INFO  id=0002 txop=07e3970dc3ff45f78bb1c3f9a20536d7 (txop)(3036668944)
    [033378e1153b44bab4188f3b7bdd129d] 2020-11-09 00:38:32.034 INFO  id=0007 txop=033378e1153b44bab4188f3b7bdd129d (txop)(2975855632)
    [bc983a25cb2b4612a501186fa559f0f4] 2020-11-09 00:38:32.034 INFO  id=0003 txop=bc983a25cb2b4612a501186fa559f0f4 (txop)(3017786384)
    [1eec9b811dfc4ec29c716f8aa43d0efd] 2020-11-09 00:38:32.035 INFO  id=0006 txop=1eec9b811dfc4ec29c716f8aa43d0efd (txop)(3009393680)
    [eb2427adafa349bab10d2408d4226aaa] 2020-11-09 00:38:32.036 INFO  id=0005 txop=eb2427adafa349bab10d2408d4226aaa (txop)(2986337296)
    [56b6b986d1b7470baa081a4f9e4401f0] 2020-11-09 00:38:32.037 INFO  id=0008 txop=56b6b986d1b7470baa081a4f9e4401f0 (txop)(2959070224)
    [e202b1125a594b19882a80566635de1d] 2020-11-09 00:38:32.034 INFO  id=0004 txop=e202b1125a594b19882a80566635de1d (txop)(2998924304)
    [8aa91af0311e499c86c988af68497f09] 2020-11-09 00:38:32.038 INFO  id=0009 txop=8aa91af0311e499c86c988af68497f09 (txop)(2917106704)

    $ ./loggertest warn 2>&1 | awk '{printf("    %s\n",$0)}'
    [] 2020-11-09 00:38:45.792 INFO  default global level: ALL (main)(3070061008)
    [] 2020-11-09 00:38:45.793 INFO  setting global level to 5 (WARN) (main)(3070061008)
    [] 2020-11-09 00:38:45.794 WARN  ----warn test---- (main)(3070061008)
    [] 2020-11-09 00:38:45.795 ERROR ----error test---- (main)(3070061008)

    $ ./loggertest error 2>&1 | awk '{printf("    %s\n",$0)}'
    [] 2020-11-09 00:38:57.398 INFO  default global level: ALL (main)(3069929936)
    [] 2020-11-09 00:38:57.399 INFO  setting global level to 6 (ERROR) (main)(3069929936)
    [] 2020-11-09 00:38:57.400 ERROR ----error test---- (main)(3069929936)



## Copyright

Copyright (c) 2020 Wade Ryan

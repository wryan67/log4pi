#!/bin/bash
#:#####################################################:#
#:#  build.sh - source code compile & install tool    #:#
#:#  Written by Wade Ryan                             #:#
#:#  1/5/2020                                         #:#
#:#                                                   #:# 
#:#  Build commands (lifecycle):                      #:# 
#:#                                                   #:# 
#:#  clean    - remove all intermediate files         #:# 
#:#  compile  - build all sources & demo executable   #:# 
#:#  package  - create libraries                      #:# 
#:#  install  - install libraries to system folders   #:# 
#:#                                                   #:# 
#:#  * install will use sudo to access                #:# 
#:#    the system folders                             #:# 
#:#                                                   #:# 
#:#                                                   #:# 
#:#####################################################:#
set -a

DESTDIR=/usr
PREFIX=/local

LIBNAME=log4pi
EXECUTE=na
___MAIN=na

HEADER_NAME=$LIBNAME.h
HEADER_ORDER=header.order

DEMOSRC=`for i in example/*.cpp;do basename $i .cpp;done`

BASE="."
SRC=$BASE/src
BIN=$BASE/bin
LIB=$BASE/lib
OBJ=$BASE/obj
INC=$BASE/include

VERSION=`cat $BASE/version 2>/dev/null`
[ "$VERSION" = "" ] && VERSION=0.1

C="gcc"
CC="g++"
#DEBUG="-g"
DEBUG=""
CFLAGS="-O2 -std=c11 -Wall"
CCFLAGS="-O2 -std=c++11 -Wall -DbuildVersion=$VERSION $DEBUG"
DYNAMIC="-fPIC"
DBLIBS=""
LDFLAGS="-L$LIB"
SHLIBS="-pthread -lm -luuid"


## -------------- DB ---------------
##   Define these in your .profile
## ---------------------------------
## ubuntu:
#     export DB_INCLUDES << not needed
#     export DB_LIBS="/usr/lib/x86_64-linux-gnu"
#
## CentOS
#     export DB_INCLUDES=-I/home/wryan/apps/postgresql/include
#     export DB_LIBS=/home/wryan/apps/postgresql/lib
## 
RPATH="-Wl,-rpath-link,${DB_LIBS}"

SRC_INCLUDES=`find $SRC -type d | awk '{printf("-I%s ",$0);}'`
JNI_INCLUDES="-I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux"
INCLUDES="$SRC_INCLUDES $DB_INCLUDES $JNI_INCLUDES"
LIBS="$DBLIBS $SHLIBS"


#:###################:#
#:#    Die          #:#
#:###################:#
die() { 
  exit 2
}

#:###################:#
#:#    Clean        #:#
#:###################:#
clean() { 
  if [ "$1" = "objects" ];then
    rm -rf $OBJ || die
  else 
    rm -rf $BIN $OBJ $LIB $INC || die
  fi
}

#:###################:#
#:#    Compile      #:#
#:###################:#
compile() {
  LAST=$(ls -1tr "$OBJECT" "$SOURCE" "$HEADER" 2>/dev/null | tail -1)

  if [ "$LAST" = "$OBJECT" ];then
    return
  fi

  RELINK=1
  echo compiling "$SOURCE"
  eval \${$1} \${$2} -c $DYNAMIC $INCLUDES "$SOURCE" -o "$OBJECT"
# userEcho \${$1} \${$2} -c $DYNAMIC $INCLUDES "$SOURCE" -o "$OBJECT"
  RET=$?

  if [ $RET != 0 ];then
    die
  fi
}

#:#####################:#
#:#  Examine Objects  #:#
#:#####################:#
examineObjects() {
  for SOURCE in `find $SRC -type f | egrep -v $___MAIN`
  do
    DIRNAME=`dirname ${SOURCE}`
    FILNAME=`basename ${SOURCE}`
    SRCNAME="${FILNAME%.*}"
    EXT="${FILNAME##*.}"
    OBJECT="$OBJ/$DIRNAME/$SRCNAME".o 
    HEADER="$DIRNAME/$SRCNAME.h"

    mkdir -p $OBJ/$DIRNAME

    if [ $EXT = "c" ];then
      compile C CFLAGS
    elif [ $EXT = "cpp" ];then
      compile CC CCFLAGS
    fi
  done
}


#:###################:#
#:#  build objects  #:#
#:###################:#
build() {

  mkdir -p $BIN
  mkdir -p $OBJ

  examineObjects
}
#:##########################:#
#:#  echo what I'm doing   #:#
#:##########################:#
userEcho() {
  echo $*
  eval $* || die
}

#:######################:#
#:#  build executable  #:#
#:######################:#
buildExe() {

  APILIB=`echo lib$LIBNAME | sed 's/^lib//'`

  if [ $MAIN = 1 ];then
    echo linking $EXECUTE
    userEcho $CC $CCFLAGS $INCLUDES $LDFLAGS $RPATH $___MAIN.cpp -l$APILIB $LIBS -o $BIN/$EXECUTE 
  fi
  if [ $DEMOS = 1 ];then 
    echo linking demos
    for EXE in $DEMOSRC
    do
      userEcho $CC $CCFLAGS -I./include example/$EXE.cpp $LDFLAGS -l$APILIB $LIBS -o $BIN/$EXE
    done
  fi
}

#:###################:#
#:#  package        #:#
#:###################:#
package() {
  packageHeaders
  mkdir -p $LIB 

  if [ $STATICONLY = 0 ];then
    ls -tr $LIB/* 2>/dev/null | tail -1 | grep '\.a' > /dev/null
    A=$?

    DYNAMIC="-fPIC"
    [ $A = 0 ] && clean objects
    build

    echo Packaging lib$LIBNAME.so
    OBJECTS=$(find $OBJ -type f)
    userEcho ${CC} -shared -Wl,-soname,lib$LIBNAME.so -o $LIB/lib$LIBNAME.so.$VERSION ${LIBS} ${OBJECTS} || die
    ln -fs lib$LIBNAME.so.$VERSION $LIB/lib$LIBNAME.so
  fi

#  echo Buliding source for static library for sake of runtime speed
#ls -tr $LIB/* 2>/dev/null | tail -1 | grep '\.a' > /dev/null
#A=$?
#[ $A != 0 ] && clean objects
#  DYNAMIC=""
#  build

#  echo Packaging lib$LIBNAME.a
#  OBJECTS=$(find $OBJ -type f)
#  ar rcs $LIB/lib$LIBNAME.a $OBJECTS || die
#  ranlib $LIB/lib$LIBNAME.a  || die
}

#:###################:#
#:#  headers        #:#
#:###################:#
packageHeaders() {
  echo "[Package Headers]"

  mkdir -p include 

  echo "#pragma once" > ./include/$HEADER_NAME
  echo  "pragma once" > .headers.dat

  find $SRC -name "*.h" | while read FILENAME; do basename $FILENAME; done | awk '{printf("include.*%s\n",$0)}' >> .headers.dat
  
  (
    cat `ls src/common/synchronized/*.h` $(cat $HEADER_ORDER) 
  ) | egrep -avf .headers.dat > ./include/$HEADER_NAME
  rm -f .headers.dat
}

#:###################:#
#:#  install        #:#
#:###################:#
install() {
  packageHeaders
  echo "[Install Headers]"

  mkdir -p   ${DESTDIR}${PREFIX}/include || die
  chmod a+rx ${DESTDIR}${PREFIX}/include || die

  cp ./include/$HEADER_NAME ${DESTDIR}${PREFIX}/include/$HEADER_NAME

  chmod 0644 ${DESTDIR}${PREFIX}/include/$HEADER_NAME

  echo "[Install Libraries]"
  mkdir -p   ${DESTDIR}${PREFIX}/lib || die
  chmod a+rx ${DESTDIR}${PREFIX}/lib || die

  APILIB=lib$LIBNAME.so.$VERSION
  STATIC=lib$LIBNAME.a

  rm -f ${DESTDIR}/lib/lib$LIBNAME.so ${DESTDIR}${PREFIX}/lib/$APILIB ${DESTDIR}${PREFIX}/lib/$STATIC 
  if [ -s $LIB/$APILIB ];then 
    cp $LIB/$APILIB  ${DESTDIR}${PREFIX}/lib/$APILIB 
    ln -sf            ${DESTDIR}${PREFIX}/lib/$APILIB  ${DESTDIR}/lib/lib$LIBNAME.so
  fi

  if [ -s $LIB/$STATIC ];then  
    cp $LIB/$STATIC ${DESTDIR}${PREFIX}/lib/$STATIC 
  fi
  ldconfig
}

#:###################:#
#:#  un-install     #:#
#:###################:#
remove() {
  echo "[Un-install]"
  rm -f ${DESTDIR}${PREFIX}/include/$HEADER_NAME
  rm -f ${DESTDIR}${PREFIX}/lib/lib$LIBNAME.*
  ldconfig
}


#:###################:#
#:#    Main         #:#
#:###################:#

CLEAN=0
BUILD=0
INSTALL=0
PACKAGE=0
REMOVE=0
RELINK=0
LINK=0
MAIN=0
DEMOS=0
STATICONLY=0

if [ "$*" = "" ];then
  BUILD=1
fi

for COMMAND in $* 
do
  typeset -l $COMMAND
  case $COMMAND in
    help)      echo "phases:  clean compile package exe remove"
               exit 0
               ;;
    clean)     CLEAN=1
               ;;
    compile)   BUILD=1
               ;;
    build)     BUILD=1
               ;;
    package)   BUILD=1 
               PACKAGE=1
               ;;
    main)      BUILD=1
               PACKAGE=1
               MAIN=1
               ;;
    exe)       BUILD=1
               PACKAGE=1
               MAIN=1
               LINK=1
               ;;
    demos)     BUILD=1
               PACKAGE=1
               DEMOS=1
               LINK=1
               ;;
    install)   BUILD=1
               PACKAGE=1
               INSTALL=1
               ;;
    remove)    REMOVE=1
               ;;
    *)         BUILD=1
               ;;
  esac
done

[ $CLEAN = 1 ]       && clean
[ $BUILD = 1 ]       && build
[ $PACKAGE = 1 ]     && package
[ $LINK = 1 ]        && buildExe
[ $INSTALL = 1 ]     && install
[ $REMOVE = 1 ]      && remove

chown -R `logname`:`ls -lad . 2>/dev/null | awk '{print $4}'` .
exit 0

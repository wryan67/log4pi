##########################################################################
# Makefile
#
# This software is primarially a logging utility, but also includes 
# a few other basic tools, like StringBuilder and currentTimeMillis()
#
# Copyright (c) 2020 Wade Ryan
#
# If you have questions or improvements email me at
# wryan67@gmail.com
#
# This software is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The given code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You can view the contents of the licence at <http://www.gnu.org/licenses/>.
##########################################################################
# Sorry, I hate make and cmake, but at least I give you a 
# make file interface, so you can use make like you're used to
##########################################################################

all: compile

help:    
	./build.sh help

clean: chmod
	@./build.sh clean

compile: chmod
	@./build.sh compile
	
lib: chmod 
	@./build.sh package
	
package: chmod 
	@./build.sh package
	
install: chmod
	@sudo ./build.sh install

demos: chmod
	@./build.sh demos

exe: chmod
	@./build.sh demos

static: chmod
	@./build.sh static

remove: chmod
	@sudo ./build.sh remove

chmod: 
	@chmod 755 build.sh
     
jni:
	javac -cp ../core/target/core-2020neo.jar:../basetypes/target/basetypes-2020neo.jar:../messages/target/messages-2020neo.jar -h src/ccsi/idg/jni ../core/src/main/java/com/acxiom/ccsi/idmgmt/handler/JNISession.java

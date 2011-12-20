# Copyright 2011 Kevin Cox

################################################################################
#                                                                              #
#  Permission is hereby granted, free of charge, to any person obtaining a     #
#  copy of this software and associated documentation files (the "Software"),  #
#  to deal in the Software without restriction, including without limitation   #
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,    #
#  and/or sell copies of the Software, and to permit persons to whom the       #
#  Software is furnished to do so, subject to the following conditions:        #
#                                                                              #
#  The above copyright notice and this permission notice shall be included in  #
#  all copies or substantial portions of the Software.                         #
#                                                                              #
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  #
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    #
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL     #
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  #
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     #
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         #
#  DEALINGS IN THE SOFTWARE.                                                   #
#                                                                              #
################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(BuildGengen-makefile)

##### Files
FILE(GLOB sources *.cpp)
FILE(GLOB headers *.hpp)
SET(xml_sources
	../../buildgen-xml/common.cpp
	../../buildgen-xml/load.cpp
	../../buildgen-xml/target.cpp
)
SET(xml_headers
	../../buildgen-xml/common.hpp
	../../buildgen-xml/load.hpp
	../../buildgen-xml/target.hpp
)

##### Target Settings
#string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
#if (CMAKE_BUILD_TYPE STREQUAL "debug")
	SET(CMAKE_COLOR_MAKEFILE ON)
	SET(CMAKE_VERBOSE_MAKEFILE ON)

	ADD_DEFINITIONS(-ggdb -Wall -p -pg)
	#ADD_DEFINITIONS(-Weffc++) # If you feel brave. (You will get lots of output from the Qt libaries)
	ADD_DEFINITIONS(-DDEBUG)
#else ()
#	ADD_DEFINITIONS(-O4 -fexpensive-optimizations -s)
#	ADD_DEFINITIONS(-UDEBUG)
#endif ()

##### Build
INCLUDE_DIRECTORIES(../../)
ADD_EXECUTABLE(gen-makefile
	${sources}
	${headers}
	${xml_sources}
	${xml_headers}
	../../buildgen-exe/messages.hpp # Keep a uniform look and feel
	../../buildgen-exe/messages.cpp #
)

INSTALL(TARGETS gen-makefile DESTINATION bin)
#INSTALL(COPY resources DESTINATION .) # Get our resources file location

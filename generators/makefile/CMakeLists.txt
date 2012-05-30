# Copyright 2011-2012 Kevin Cox

################################################################################
#                                                                              #
#  This software is provided 'as-is', without any express or implied           #
#  warranty. In no event will the authors be held liable for any damages       #
#  arising from the use of this software.                                      #
#                                                                              #
#  Permission is granted to anyone to use this software for any purpose,       #
#  including commercial applications, and to alter it and redistribute it      #
#  freely, subject to the following restrictions:                              #
#                                                                              #
#  1. The origin of this software must not be misrepresented; you must not     #
#     claim that you wrote the original software. If you use this software in  #
#     a product, an acknowledgment in the product documentation would be       #
#     appreciated but is not required.                                         #
#                                                                              #
#  2. Altered source versions must be plainly marked as such, and must not be  #
#     misrepresented as being the original software.                           #
#                                                                              #
#  3. This notice may not be removed or altered from any source distribution.  #                                                           #
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

	ADD_DEFINITIONS(-ggdb -Wall)# -p -pg)
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

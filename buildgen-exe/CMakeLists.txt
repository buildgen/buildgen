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
#  3. This notice may not be removed or altered from any source distribution.  #
#                                                                              #
################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(BuildGen)

##### Files
FILE(GLOB sources *.cpp)
GET_FILENAME_COMPONENT(mainfile main.cpp ABSOLUTE)     # GLOB returns absolute
GET_FILENAME_COMPONENT(testmainfile test.cpp ABSOLUTE) # paths.
LIST(REMOVE_ITEM sources "${mainfile}" "${testmainfile}")
FILE(GLOB headers *.hpp)
SET(xml_sources
	../buildgen-xml/common.cpp
	../buildgen-xml/save.cpp
	../buildgen-xml/target.cpp
	../buildgen-xml/targetmanager.cpp
)
SET(xml_headers
	../buildgen-xml/common.hpp
	../buildgen-xml/save.hpp
	../buildgen-xml/target.hpp
	../buildgen-xml/itargetmanager.hpp
	../buildgen-xml/targetmanager.hpp
)

##### Target Settings
#string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
#if (CMAKE_BUILD_TYPE STREQUAL "debug")
	SET(CMAKE_COLOR_MAKEFILE ON)
	SET(CMAKE_VERBOSE_MAKEFILE ON)

	ADD_DEFINITIONS(-g -Wall)# -p -pg)
	#ADD_DEFINITIONS(-Weffc++) # If you feel brave. (You will get lots of output from the Qt libaries)
	ADD_DEFINITIONS(-DDEBUG)
#else ()
#	ADD_DEFINITIONS(-O4 -fexpensive-optimizations -s)
#	ADD_DEFINITIONS(-DNDEBUG)
#endif ()

SET(lualibs_root "lib/buildgen/lualibs/")
SET(default_generator "gen-makefile")
CONFIGURE_FILE("info.h.in" "${CMAKE_CURRENT_BINARY_DIR}/info.h") # Makes info.h which has some marcos in it.
CONFIGURE_FILE("info.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/info.cpp") # Makes info.h which has some marcos in it.
INSTALL(DIRECTORY "lualibs/" DESTINATION "${lualibs_root}"
	FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
	                 GROUP_READ GROUP_WRITE GROUP_EXECUTE
	                 WORLD_READ             WORLD_EXECUTE
)

##### Build
IF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	LINK_DIRECTORIES("/usr/local/lib/lua51/")
	INCLUDE_DIRECTORIES("/usr/local/include/lua51/")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")

INCLUDE_DIRECTORIES(..)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")
ADD_EXECUTABLE(gen
	"main.cpp"
	${sources}
	${headers}
	${xml_sources}
	${xml_headers}
	"${CMAKE_CURRENT_BINARY_DIR}/info.cpp"
	"info.h"
)

TARGET_LINK_LIBRARIES(gen gtest lua m)
INSTALL(TARGETS gen DESTINATION bin)

// Copyright 2011 Kevin Cox

/*******************************************************************************
*                                                                              *
*  Permission is hereby granted, free of charge, to any person obtaining a     *
*  copy of this software and associated documentation files (the "Software"),  *
*  to deal in the Software without restriction, including without limitation   *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
*  and/or sell copies of the Software, and to permit persons to whom the       *
*  Software is furnished to do so, subject to the following conditions:        *
*                                                                              *
*  The above copyright notice and this permission notice shall be included in  *
*  all copies or substantial portions of the Software.                         *
*                                                                              *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL     *
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
*  DEALINGS IN THE SOFTWARE.                                                   *
*                                                                              *
*******************************************************************************/

#ifndef XML_COMMON_HPP
#define XML_COMMON_HPP

#include <time.h>

namespace XML
{
	/**
	 * These are the names used in the xml file for tag and attribute names.
	 * Please use these because if they are ever changed there is a better
	 * chance that your program will jsut be a compile away from working.  (If
	 * these names are not changed).  Also this could help shrink the size of
	 * executables.
	 *
	 * The odd letters just before the suffix name are where they are used.
	 * They serve 2 purposes, most importantly prevent namespace clashes and
	 * secondly they serve as mnemonics.  The letters mean the following:
	 *	\li A attribute
	 *	\li N node
	 *	\li V value (usually for attributes but possibly for text nodes as well)
	 */
	extern const char *rootNName;
	extern const char *versionAName;
	extern const char *metaNName;
	extern const char *meta_projectRootNName;
	extern const char *meta_outRootNName;
	extern const char *meta_buildGenRootNName;
	extern const char *meta_timeNName;
	extern const char *targetNName;
	extern const char *targetsNName;
	extern const char *target_outNName;
	extern const char *target_dependsNName;
	extern const char *target_generatorNName;
	extern const char *target_generator_commandNName;
	extern const char *target_generator_command_argumentNName;
	extern const char *target_generator_command_argument_posAName;

	struct Meta {
		char *buildGenRoot;
		char *outRoot;
		char *projectRoot;

		time_t time;
	};
}

#endif

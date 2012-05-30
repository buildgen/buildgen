// Copyright 2011-2012 Kevin Cox

/*******************************************************************************
*                                                                              *
*  This software is provided 'as-is', without any express or implied           *
*  warranty. In no event will the authors be held liable for any damages       *
*  arising from the use of this software.                                      *
*                                                                              *
*  Permission is granted to anyone to use this software for any purpose,       *
*  including commercial applications, and to alter it and redistribute it      *
*  freely, subject to the following restrictions:                              *
*                                                                              *
*  1. The origin of this software must not be misrepresented; you must not     *
*     claim that you wrote the original software. If you use this software in  *
*     a product, an acknowledgment in the product documentation would be       *
*     appreciated but is not required.                                         *
*                                                                              *
*  2. Altered source versions must be plainly marked as such, and must not be  *
*     misrepresented as being the original software.                           *
*                                                                              *
*  3. This notice may not be removed or altered from any source distribution.  *                                                           *
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
	extern const char *filesNName;
	extern const char *files_rootNName;
	extern const char *files_infoNName;
	extern const char *targetNName;
	extern const char *targetsNName;
	extern const char *target_outNName;
	extern const char *target_out_magicAName;
	extern const char *target_dependsNName;
	extern const char *target_generatorNName;
	extern const char *target_generator_commandNName;
	extern const char *target_generator_command_argumentNName;
	extern const char *target_generator_command_argument_posAName;
	extern const char *target_generator_descriptionNName;

	struct Meta {
		char *outRoot;
		char *projectRoot;

		time_t time;
	};
}

#endif

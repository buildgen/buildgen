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

#include <string.h>

#include "globals.hpp"
#include "messages.hpp"

char *mstrdup ( const char *s )
{
	char *r = strdup(s);
	checkAlloc(r);
	return r;
}

char *myalloc ( size_t chars )
{
	void *r = malloc(chars*sizeof(char));
	checkAlloc(r);
	return (char*)r;
}

char *myrealloc ( char *s, size_t chars )
{
	s = (char*)realloc(s, chars*sizeof(char));
	checkAlloc(s);
	return s;
}

char *mstrcat ( const char *s1, const char *s2 )
{
	size_t l1 = strlen(s1);
	size_t l2 = strlen(s2);

	char *r = myalloc(l1+l2+1);

	strcpy(r, s1);
	strcpy(r+l1, s2);

	return r;
}

char *mstrcat ( char c, const char *s2 )
{
	size_t l2 = strlen(s2);

	char *r = myalloc(l2+2);

	*r = c;
	strcpy(r+1, s2);

	return r;
}

char *mstrcat ( const char *s1, const char *s2, const char *s3 )
{
	size_t l1 = strlen(s1);
	size_t l2 = strlen(s2);
	size_t l3 = strlen(s3);

	char *r = myalloc(l1+l2+l3+1);

	char *e1 = r+l1;
	char *e2 = e1+l2;

	strcpy(r, s1);
	strcpy(e1, s2);
	strcpy(e2, s3);

	return r;
}

char *mstrcat ( const char *s1, char c, const char *s3 )
{
	size_t l1 = strlen(s1);
	size_t l3 = strlen(s3);

	char *r = myalloc(l1+l3+2);

	char *e1 = r+l1;
	char *e2 = e1+1;

	strcpy(r, s1);
	*e1 = c;
	strcpy(e2, s3);

	return r;
}

char *mstrcat ( char c,const char *s2,  const char *s3 )
{
	size_t l2 = strlen(s2);
	size_t l3 = strlen(s3);

	char *r = myalloc(l2+l3+2);

	char *e2 = r+1;
	char *e3 = e2+l2;

	*r = c;
	strcpy(e2, s2);
	strcpy(e3, s3);

	return r;
}

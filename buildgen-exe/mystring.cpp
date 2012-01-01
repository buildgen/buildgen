// Copyright 2011-2012 Kevin Cox

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

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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "test.hpp"

#include "globals.hpp"
#include "messages.hpp"

char *mstrdup ( const char *s )
{
	assert( s != NULL );
	if ( s == NULL ) return NULL; // Safest runtime fallback;

	char *r = strdup(s);
	checkAlloc(r);
	return r;
}
#ifdef TEST
TEST(MyString, mstrdup)
{
#define T(s) { \
	char *r = mstrdup(s); \
	EXPECT_NE(s, r); \
	EXPECT_STREQ(s, r); \
	free(r); \
}

	T("string");
	T("stRing");
	T("test string");

#undef T
}
#endif

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
#ifdef TEST
TEST(MyString, mstrcat_stringstring)
{
#define T(s1, s2, e) { \
	char *r = mstrcat(s1, s2); \
	EXPECT_STREQ(e, r); \
	free(r); \
}

	T("string", "string", "stringstring");
	T("This is ", "a test.", "This is a test.");
	T("test string", "", "test string");
	T("", "test string", "test string");

#undef T
}
#endif

char *mstrcat ( char c, const char *s2 )
{
	size_t l2 = strlen(s2);

	char *r = myalloc(l2+2);

	*r = c;
	strcpy(r+1, s2);

	return r;
}
#ifdef TEST
TEST(MyString, mstrcat_charstring)
{
#define T(c, s, e) { \
	char *r = mstrcat(c, s); \
	EXPECT_STREQ(e, r); \
	free(r); \
}

	T('a', " string", "a string");
	T('s', "plat", "splat");
	T('!', "", "!");

#undef T
}
#endif

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
#ifdef TEST
TEST(MyString, mstrcat_stringstringstring)
{
#define T(s1, s2, s3, e) { \
	char *r = mstrcat(s1, s2, s3); \
	EXPECT_STREQ(e, r); \
	free(r); \
}

	T("string", "string", "string", "stringstringstring");
	T("This is ", "a test", ".", "This is a test.");
	T("", "Hello,", " goodbye.", "Hello, goodbye.");
	T("Hello,", "", " goodbye.", "Hello, goodbye.");
	T("Hello,", " goodbye.", "", "Hello, goodbye.");

#undef T
}
#endif

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
#ifdef TEST
TEST(MyString, mstrcat_stringcharstring)
{
#define T(s1, c, s2, e) { \
	char *r = mstrcat(s1, c, s2); \
	EXPECT_STREQ(e, r); \
	free(r); \
}

	T("/usr", '/', "share/", "/usr/share/");
	T("Hello", ' ', "Joe.", "Hello Joe.");
	T("Hi", '!', "", "Hi!");
	T("", '/', "bin/", "/bin/");

#undef T
}
#endif

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
#ifdef TEST
TEST(MyString, mstrcat_charstringstring)
{
#define T(c, s1, s2, e) { \
	char *r = mstrcat(c, s1, s2); \
	EXPECT_STREQ(e, r); \
	free(r); \
}

	T('H', "ello, ", "Joe.", "Hello, Joe.");
	T('H', "eidi.", "ello.", "Heidi.ello.");
	T('H', "", "ello.", "Hello.");
	T('H', "eidi.", "", "Heidi.");
#undef T
}
#endif

char *mstrapp ( char **sp, const char *n )
{
	char *s = *sp;
	size_t sl = strlen(s);
	size_t nl = strlen(n);

	s = myrealloc(s, sl+nl+1);
	strcpy(s+sl, n);

	*sp = s;
	return s;
}
#ifdef TEST
TEST(MyString, mstrapp)
{
	char *s1, *s2, *s3;

	s1 = s2 = strdup("Hello");
	s3 = mstrapp(&s1, " Mary.");
	EXPECT_EQ(s1, s3);
	EXPECT_STREQ(s1, "Hello Mary.");
	free(s1);
}
#endif

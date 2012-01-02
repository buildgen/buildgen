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

#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <stdio.h>
#include <stdarg.h>

/// Tools for outputing things to
namespace msg
{
	extern FILE *msg_out; ///< Handles everything but error
	extern FILE *err_out; ///< Handles error

	extern bool useColor;

	/// How much to say
	/**
	 * A value less than zero will print everthing.  A value of zero will print
	 * nothing. (errors will still printerd to \c stderr)  Higher values will
	 * print more and more.
	 */
	extern int verbosity;

	/// Print an info message
	/**
		A logging tool.  This is usually used for messages that don't fit into
		any other catagory.  Possibly telling the user that something strange
		has happened. (something non-fatal of course)
	*/
	inline void info(const char *msg, ...)
	{
		va_list args;
		va_start(args, msg);

		if ( msg::verbosity < 0 || msg::verbosity >= 1 )
		{
			fputs("Info:  ", msg_out);
			vfprintf(msg_out, msg, args);
			fputc('\n', msg_out);
		}

		va_end(args);
	}
	/// Print a log message
	/**
		A logging tool.  This is used to log the regular output of the program.
		There is a good chance that this might be parsed and a high chance the
		user won't see these messages (they can easily be turned off).

		These messages should log everything significant that the program does.
		Any variable part of the message should be double quotes for easy
		identificaton. (No need to escape quotes as the last quote is assumed to
		close it.
	*/
	inline void log(const char *msg, ...)
	{
		va_list args;
		va_start(args, msg);

		printf("verbosity: %d\n", msg::verbosity);
		if ( msg::verbosity < 0 || msg::verbosity >= 2 )
		{
			fputs("Log:   ", msg_out);
			vfprintf(msg_out, msg, args);
			fputc('\n', msg_out);
		}

		va_end(args);
	}
	/// Print a debug message
	/**
		A logging tool.  This is used to display messages to be used by devs.
		These won't display in release builds and will likely be optimized into
		oblivion.  It is safe to leave these in but please remove these after
		you finish working on a section so you don't spam the output.  If you
		think this would be helpful to anyone debuging there is a good chance
		you should be using log().

		There is no required format just use what you pefer.  The output stream
		is garenteed to be flushed.
	*/
	inline void debug(const char *msg, ...)
	{
		va_list args;
		va_start(args, msg);

#ifdef DEBUG //@help Is it safe to comment out the varg macros?
		vfprintf(msg_out, msg, args);
		fputc('\n', msg_out);
		fflush(msg_out);
#endif

		va_end(args);
	}
	/// Print an error message
	/**
		A logging tool.  This displays an error message to the user.  The
		error is usually fatal and the caller should quit the program or try to
		rack up more error messages.  In general, the program should not
		complete successfully after emiting an error message.  If you think the
		program can continue maybe you are looking for info().
	*/
	inline void error(const char *msg, ...)
	{
		va_list args;
		va_start(args, msg);

		fputs("Error: ", err_out);
		vfprintf(err_out, msg, args);
		fputc('\n', err_out);
		fflush(err_out);


		va_end(args);
	}
}


#endif

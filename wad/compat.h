/*
	compat.h

	Miscellaneous compability stuff

	Copyright (C) 1996-1997  Id Software, Inc.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

	$Id: compat.h,v 1.14 2002/06/26 22:20:11 despair Exp $
*/

#ifndef __compat_h
#define __compat_h

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#include <stdlib.h>


#ifndef max
# define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
# define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef bound
# define bound(a,b,c) (max(a, min(b, c)))
#endif
/* This fixes warnings when compiling with -pedantic */
#if defined(__GNUC__) && !defined(inline)
# define inline __inline__
#endif

/* These may be underscored... */
#if defined(HAVE__SNPRINTF) && !defined(HAVE_SNPRINTF)
# undef snprintf
# define snprintf Q_snprintfz
# define need_qstring_h
#endif
#if defined(HAVE__VSNPRINTF) && !defined(HAVE_VSNPRINTF)
# undef vsnprintf
# define vsnprintf Q_vsnprintfz
# define need_qstring_h
#endif
#if defined(_WIN32) && !defined(__BORLANDC__)
# define kbhit _kbhit
#endif

/* If we don't have them in the C-library we declare them to avoid warnings */
#if ! (defined(HAVE_SNPRINTF) || defined(HAVE__SNPRINTF))
extern int snprintf(char * s, size_t maxlen, const char *format, ...);
#endif
#if ! (defined(HAVE_VSNPRINTF) || defined(HAVE__VSNPRINTF))
extern int vsnprintf(char *s, size_t maxlen, const char *format, va_list arg);
#endif

/* String utility functions */
#if !defined(strequal)
# define strequal(a,b) (strcmp (a, b) == 0)
#endif
#if !defined(strcaseequal)
# define strcaseequal(a,b) (strcasecmp (a, b) == 0)
#endif
#if !defined(strnequal)
# define strnequal(a,b,c) (strncmp (a, b, c) == 0)
#endif
#if !defined(strncaseequal)
# define strncaseequal(a,b,c) (strncasecmp (a, b, c) == 0)
#endif
#ifdef HAVE_STRCASESTR
# ifndef HAVE_STRCASESTR_PROTO
extern char *strcasestr (const char *__haystack, const char *__needle);
# endif
#else
# define strcasestr Q_strcasestr
# define need_qstring_h
#endif
#ifdef HAVE_STRNLEN
# ifndef HAVE_STRNLEN_PROTO
size_t strnlen (const char *str, size_t len);
# endif
#else
# define strnlen Q_strnlen
# define need_qstring_h
#endif

#ifdef need_qstring_h
//# include "qstring.h"
#endif

#undef field_offset
#define field_offset(type,field) ((size_t)&(((type *)0)->field))

#endif // __compat_h

/*
	quakeio.c

	(description)

	Copyright (C) 1996-1997  Id Software, Inc.
	Copyright (C) 1999,2000  contributors of the QuakeForge project
	Please see the file "AUTHORS" for a list of contributors

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

*/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

static __attribute__ ((unused)) const char rcsid[] = 
	"$Id: quakeio.c,v 1.29 2004/02/29 07:12:05 taniwha Exp $";

#ifdef HAVE_ZLIB
# include <zlib.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _MSC_VER
# define _POSIX_
#endif

#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

//#include "dstring.h"
//#include "qendian.h"
#include "quakefs.h"
#include "quakeio.h"

#ifdef _WIN32
# ifndef __BORLANDC__
#  define setmode _setmode
#  define O_BINARY _O_BINARY
# endif
#endif

struct QFile_s {
	FILE *file;
#ifdef HAVE_ZLIB
	gzFile *gzfile;
#endif
	off_t size;
	off_t start;
	int   c;
};


int
Qrename (const char *old, const char *new)
{
	return rename (old, new);
}

int
Qremove (const char *path)
{
	return remove (path);
}

int
Qfilesize (QFile *file)
{
	return file->size;
}

static int
check_file (int fd, int offs, int len, int *zip)
{
	unsigned char id[2], len_bytes[4];

	if (offs < 0 || len < 0) {
		// normal file
		offs = 0;
		len = lseek (fd, 0, SEEK_END);
		lseek (fd, 0, SEEK_SET);
	}
	if (*zip) {
		int         r;

		lseek (fd, offs, SEEK_SET);
		r = read (fd, id, 2);
		if (r == 2 && id[0] == 0x1f && id[1] == 0x8b && len >= 6) {
			lseek (fd, offs + len - 4, SEEK_SET);
			read (fd, len_bytes, 4);
			len = ((len_bytes[3] << 24)
				   | (len_bytes[2] << 16)
				   | (len_bytes[1] << 8)
				   | (len_bytes[0]));
		} else {
			*zip = 0;
		}
	}
	lseek (fd, offs, SEEK_SET);
	return len;
}

QFile *
Qopen (const char *path, const char *mode)
{
	QFile      *file;
	char       *m, *p;
	int         reading = 0;
	int         zip = 0;
	int         size = -1;

	m = alloca (strlen (mode) + 1);
	for (p = m; *mode && p - m < ((int) sizeof (m) - 1); mode++) {
		if (*mode == 'z') {
			zip = 1;
			continue;
		}
		if (*mode == 'r') {
			reading = 1;
		}
#ifndef HAVE_ZLIB
		if (strchr ("0123456789fh", *mode)) {
			continue;
		}
#endif
		*p++ = *mode;
	}
	*p = 0;

	if (reading) {
		int         fd = open (path, O_RDONLY);
		if (fd != -1) {
			size = check_file (fd, -1, -1, &zip);
			close (fd);
		}
	}

	file = calloc (sizeof (*file), 1);
	if (!file)
		return 0;
	file->size = size;
#ifdef HAVE_ZLIB
	if (zip) {
		file->gzfile = gzopen (path, m);
		if (!file->gzfile) {
			free (file);
			return 0;
		}
	} else
#endif
	{
		file->file = fopen (path, m);
		if (!file->file) {
			free (file);
			return 0;
		}
	}
	file->c = -1;
	return file;
}

QFile *
Qdopen (int fd, const char *mode)
{
	QFile      *file;
	char       *m, *p;
	int         zip = 0;

	m = alloca (strlen (mode) + 1);
#ifdef _WIN32
	setmode (fd, O_BINARY);
#endif
	for (p = m; *mode && p - m < ((int) sizeof (m) - 1); mode++) {
		if (*mode == 'z') {
			zip = 1;
			continue;
		}
		*p++ = *mode;
	}

	*p = 0;

	file = calloc (sizeof (*file), 1);
	if (!file)
		return 0;
#ifdef HAVE_ZLIB
	if (zip) {
		file->gzfile = gzdopen (fd, m);
		if (!file->gzfile) {
			free (file);
			return 0;
		}
	} else
#endif
	{
		file->file = fdopen (fd, m);
		if (!file->file) {
			free (file);
			return 0;
		}
	}
	file->c = -1;
	return file;
}

QFile *
Qsubopen (const char *path, int offs, int len, int zip)
{
	int         fd = open (path, O_RDONLY);
	QFile      *file;

	if (fd == -1)
		return 0;
#ifdef _WIN32
	setmode (fd, O_BINARY);
#endif

	len = check_file (fd, offs, len, &zip);
	file = Qdopen (fd, zip ? "rbz" : "rb");
	file->size = len;
	file->start = offs;
	return file;
}

void
Qclose (QFile *file)
{
	if (file->file)
		fclose (file->file);
#ifdef HAVE_ZLIB
	else
		gzclose (file->gzfile);
#endif
	free (file);
}

int
Qread (QFile *file, void *buf, int count)
{
	int         offs = 0;
	int         ret;

	if (file->c != -1) {
		char       *b = buf;
		*b++ = file->c;
		buf = b;
		offs = 1;
		file->c = -1;
		count--;
	}
	if (file->file)
		ret = fread (buf, 1, count, file->file);
	else
#ifdef HAVE_ZLIB
		ret = gzread (file->gzfile, buf, count);
#else
		return -1;
#endif
	return ret == -1 ? ret : ret + offs;
}

int
Qwrite (QFile *file, const void *buf, int count)
{
	if (file->file)
		return fwrite (buf, 1, count, file->file);
#ifdef HAVE_ZLIB
	else
		return gzwrite (file->gzfile, (const voidp)buf, count);
#else
	return -1;
#endif
}

int
Qprintf (QFile *file, const char *fmt, ...)
{
	va_list     args;
	int         ret = -1;

	va_start (args, fmt);
	if (file->file)
		ret = vfprintf (file->file, fmt, args);
#ifdef HAVE_ZLIB
	else {
		static dstring_t *buf;

		if (!buf)
			buf = dstring_new ();

		va_start (args, fmt);
		dvsprintf (buf, fmt, args);
		va_end (args);
		ret = strlen (buf->str);
		if (ret > 0)
			ret = gzwrite (file->gzfile, buf, (unsigned) ret);
	}
#endif
	va_end (args);
	return ret;
}

int
Qputs (QFile *file, const char *buf)
{
	if (file->file)
		return fputs (buf, file->file);
#ifdef HAVE_ZLIB
	else
		return gzputs (file->gzfile, buf);
#else
	return 0;
#endif
}

char *
Qgets (QFile *file, char *buf, int count)
{
	char        *ret = buf;

	if (file->c != -1) {
		*buf++ = file->c;
		count--;
		file->c = -1;
		if (!count)
			return ret;
	}
	if (file->file)
		buf = fgets (buf, count, file->file);
	else {
#ifdef HAVE_ZLIB
		buf = gzgets (file->gzfile, buf, count);
#else
		return 0;
#endif
	}
	return buf ? ret : 0;
}

int
Qgetc (QFile *file)
{
	if (file->c != -1) {
		int         c = file->c;
		file->c = -1;
		return c;
	}
	if (file->file)
		return fgetc (file->file);
#ifdef HAVE_ZLIB
	else
		return gzgetc (file->gzfile);
#else
	return -1;
#endif
}

int
Qputc (QFile *file, int c)
{
	if (file->file)
		return fputc (c, file->file);
#ifdef HAVE_ZLIB
	else
		return gzputc (file->gzfile, c);
#else
	return -1;
#endif
}

int
Qungetc (QFile *file, int c)
{
	if (file->c == -1)
		file->c = (byte) c;
	return c;
}

int
Qseek (QFile *file, long offset, int whence)
{
	file->c = -1;
	if (file->file) {
		int         res;
		switch (whence) {
			case SEEK_SET:
				res = fseek (file->file, file->start + offset, whence);
				break;
			case SEEK_CUR:
				res = fseek (file->file, offset, whence);
				break;
			case SEEK_END:
				if (file->size == -1) {
					// we don't know the size (due to writing) so punt and
					// pass on the request as-is
					res = fseek (file->file, offset, SEEK_END);
				} else {
					res = fseek (file->file,
								 file->start + file->size - offset, SEEK_SET);
				}
				break;
			default:
				errno = EINVAL;
				return -1;
		}
		if (res != -1)
			res = ftell (file->file) - file->start;
		return res;
	}
#ifdef HAVE_ZLIB
	else {
		// libz seems to keep track of the true start position itself
		// doesn't support SEEK_END, though
		return gzseek (file->gzfile, offset, whence);
	}
#else
	return -1;
#endif
}

long
Qtell (QFile *file)
{
	int         offs;
	int         ret;

	offs =  (file->c != -1) ? 1 : 0;
	if (file->file)
		ret = ftell (file->file) - file->start;
	else
#ifdef HAVE_ZLIB
		ret = gztell (file->gzfile);	//FIXME does gztell do the right thing?
#else
		return -1;
#endif
	return ret == -1 ? ret : ret - offs;
}

int
Qflush (QFile *file)
{
	if (file->file)
		return fflush (file->file);
#ifdef HAVE_ZLIB
	else
		return gzflush (file->gzfile, Z_SYNC_FLUSH);
#else
	return -1;
#endif
}

int
Qeof (QFile *file)
{
	if (file->c != -1)
		return 0;
	if (file->file)
		return feof (file->file);
#ifdef HAVE_ZLIB
	else
		return gzeof (file->gzfile);
#else
	return -1;
#endif
}

/*
	Qgetline

	Dynamic length version of Qgets. DO NOT free the buffer.
*/
const char *
Qgetline (QFile *file)
{
	static int  size = 256;
	static char *buf = 0;
	int         len;

	if (!buf) {
		buf = malloc (size);
		if (!buf)
			return 0;
	}

	if (!Qgets (file, buf, size))
		return 0;

	len = strlen (buf);
	while (len && buf[len - 1] != '\n') {
		char       *t = realloc (buf, size + 256);

		if (!t)
			return 0;
		buf = t;
		size += 256;
		if (!Qgets (file, buf + len, size - len))
			break;
		len = strlen (buf);
	}
	return buf;
}

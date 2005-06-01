/* 
	wad.h

	wadfile tool (definitions)

	Copyright (C) 1996-1997 Id Software, Inc.
	Copyright (C) 2002 Bill Currie <bill@taniwha.org>
	Copyright (C) 2002 Jeff Teunissen <deek@quakeforge.net>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

	$Id: wad.h,v 1.1 2004/01/10 08:05:25 taniwha Exp $
*/

#ifndef __wad_h
#define __wad_h

#include "cmdlib.h"
#include "wadfile.h"

typedef enum {
	mo_none,
	mo_test,
	mo_create,
	mo_extract,
} wadmode_t;

typedef struct {
	wadmode_t	mode;			// see above
	int			verbosity;		// 0=silent
	qboolean	compress;		// for the future
	qboolean	pad;			// pad area of files to 4-byte boundary
	char		*wadfile;		// wad file to read/write/test
} options_t;

#endif	// __wad_h

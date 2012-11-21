/*____________________________________________________________________________
	
	Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)

	Portions Copyright (C) 1998-1999 EMusic.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	
	$Id: debug.h,v 1.5 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_DEBUG_H_
#define INCLUDED_DEBUG_H_

#include "config.h"
#include "i18n.h"

void debug_string(char*);

ZINF_EXPORT void DebugInit_v(void);
ZINF_EXPORT void Debug_v(const char * format, ...);

#endif /* _DEBUG_H_ */

/* arch-tag: fb0c42f0-d23b-46e0-a48a-f52aac045d0a
   (do not change this comment) */

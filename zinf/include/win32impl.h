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
	
	$Id: win32impl.h,v 1.4 2003/09/16 17:34:54 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_WIN32IMPL_H_
#define INCLUDED_WIN32IMPL_H_

#include <stdint.h>
#include <limits.h>

#include "config.h"

class FILETIME {
 public:
    int32_t dwLowDateTime;
    int32_t dwHighDateTime;
};

#define FILE_ATTRIBUTE_DIRECTORY        0x00000010L
#define FILE_ATTRIBUTE_NORMAL           0x00000080L
/* Not in Win32, but something is needed to indicate a symlink */
#define FILE_ATTRIBUTE_SYMLINK          0x00000040L

class WIN32_FIND_DATA {
 public:
    int32_t dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    int32_t nFileSizeHigh;
    int32_t nFileSizeLow;
    int32_t dwReserved0;
    int32_t dwReserved1;
#ifdef PATH_MAX
    char cFileName[ PATH_MAX ];
#else
    char cFileName[ 1024 ]; // arbitrary
#endif
    char cAlternateFileName[ 14 ];
};

typedef void *HANDLE;
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
typedef void *HMODULE;
#define HINSTANCE HMODULE

typedef void *FARPROC;

HANDLE FindFirstFile(char *lpFileName, WIN32_FIND_DATA *lpFindFileData);
bool FindNextFile(HANDLE hFindFile, WIN32_FIND_DATA *lpFindFileData);
bool FindClose(HANDLE hFindFile);
HINSTANCE LoadLibrary(char *lpLibFileName);
bool FreeLibrary(HMODULE hLibModule);
FARPROC GetProcAddress(HMODULE hModule, const char *lpProcName);


#endif

/* arch-tag: d9f367f6-cf3e-432c-9de7-63196b079913
   (do not change this comment) */

/*____________________________________________________________________________
	
	Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)
	Portions copyright (C) 1998-1999 EMusic.com

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
	
	$Id: errors.h,v 1.3 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_ERROR_H
#define INCLUDED_ERROR_H

#include "config.h"


typedef enum Error
{
    kError_NoErr                = 0,
    kError_UnknownErr           = 1,
    kError_InvalidParam         = 2,
    kError_LoadLibFailed        = 3,
    kError_FindFuncFailed       = 4,
    kError_NoFiles              = 5,
    kError_NoPrefs              = 6,
    kError_BufferTooSmall       = 7,
    kError_OutOfMemory          = 8,
    kError_FileNoAccess         = 9,
    kError_FileExists           = 10,
    kError_FileInvalidArg       = 11,
    kError_FileNotFound         = 12,
    kError_FileNoHandles        = 13,
    kError_NoMoreLibDirs        = 14,
    kError_NullValueInvalid     = 15,
    kError_InvalidError         = 16,
    kError_PluginNotInitialized = 17,
    kError_GotDefaultMethod     = 18,
    kError_InputUnsuccessful    = 19,
    kError_FileSeekNotSupported = 20,
    kError_OutputUnsuccessful   = 21,
    kError_InitFailed           = 22,
    kError_InitFailedSafely     = 23,
    kError_NoDataAvail          = 24,
    kError_SeekFailed           = 25,
    kError_CreateThreadFailed   = 26,
    kError_BufferingUp          = 27,
    kError_Interrupt            = 28,
    kError_EventPending         = 29,
    kError_YouScrewedUp         = 30,
    kError_NoPrefValue          = 31,
    kError_SyntaxError          = 32,
    kError_EndOfStream          = 33,
    kError_ConnectFailed        = 34,
    kError_CantCreateSocket     = 35,
    kError_CannotSetSocketOpts  = 36,
    kError_CannotBind           = 37,
    kError_DbaseItemNotStored   = 38,
    kError_ParseError           = 39,
    kError_NotFound             = 40,
    kError_NoMoreFormats        = 41,
    kError_FormatNotSupported   = 42,
    kError_NoMoreDevices        = 43,
    kError_DeviceNotFound       = 44,
    kError_FeatureNotSupported  = 45,
    kError_UserCancel           = 46,
    kError_LoadBitmapFailed     = 47,
    kError_ProtocolNotSupported = 48,
    kError_InvalidURL           = 49,
    kError_CantFindHost         = 50,
    kError_IOError              = 51,
    kError_UnknownServerError   = 52,
    kError_BadHTTPRequest       = 53,
    kError_AccessNotAuthorized  = 54,
    kError_AccessForbidden      = 55,  
    kError_RangeNotExceptable   = 56,
    kError_WriteFile            = 57,
    kError_ReadFile             = 58,
    kError_InvalidVersion       = 59,
    kError_AlreadyUpdating      = 60,
    kError_UnlinkFailed         = 61,
    kError_CopyFailed           = 62,
    kError_NoItemsUpdated       = 63,
    kError_DatabaseNotWorking   = 64,
    kError_DuplicateItem        = 65,
    kError_ItemNotFound         = 66,
    kError_HTTPFileNotFound     = 67,
    kError_CDInitFailed         = 68,
    kError_InvalidTrack         = 69,
    kError_DownloadDenied       = 70,
    kError_NoDiscInDrive        = 71,
    kError_LastError            = 9999
} _Errors;


#define IsError( err )		( (err) != kError_NoErr )
#define IsntError( err )	( (err) == kError_NoErr )

extern const char* ErrorString[];

#endif /* INCLUDED_ERROR_H */

/* arch-tag: eb7d9668-9669-484a-b381-bfbb8e328485
   (do not change this comment) */

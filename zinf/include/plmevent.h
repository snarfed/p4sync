/*____________________________________________________________________________
	
	Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)

	Portions Copyright (C) 1999 EMusic.com

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
	
	$Id: plmevent.h,v 1.5 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PLM_EVENT
#define INCLUDED_PLM_EVENT

#include <string>
#include <vector>


#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"


typedef enum {
    kPLMEvent_Status,
    kPLMEvent_Progress,
    kPLMEvent_Error,
    kPLMEvent_Done

} PLMEventType;

#define kPLMProgressInfinity -1

class PlaylistItem;

typedef struct PLMEventProgressData {
    uint32_t position;
    uint32_t total;
    PlaylistItem* item;
} PLMEventProgressData;

typedef struct PLMEventErrorData {
    uint32_t errorCode;
} PLMEventErrorData;

typedef union PLMEventData {
    PLMEventProgressData progressData;
    PLMEventErrorData    errorData;
} PLMEventData;

typedef struct PLMEvent {
    PLMEventType type;
    PLMEventData data;
    std::string eventString;
} PLMEvent;

typedef bool (*PLMCallBackFunction)(PLMEvent* event, void* userData);


#endif // INCLUDED_PLM_EVENT

/* arch-tag: 591620ee-ebb1-4996-b34a-f88232e70e8d
   (do not change this comment) */

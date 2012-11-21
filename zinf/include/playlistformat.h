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
	
	$Id: playlistformat.h,v 1.6 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PLAYLIST_FORMAT_H_
#define INCLUDED_PLAYLIST_FORMAT_H_

#include <string>
#include <vector>


#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"
#include "plmevent.h"

class PlaylistFormatInfo;
class PlaylistItem;

class PlaylistFormat {

 public:

    PlaylistFormat(FAContext *context) { m_context = context; }
    virtual ~PlaylistFormat() {}

    virtual Error GetSupportedFormats(PlaylistFormatInfo* info, uint32_t index) = 0;

    

    virtual Error readPlaylist(const std::string& url,
                               std::vector<std::string>& items)=0;

    virtual Error writePlaylist(const std::string& url,
                                std::vector<std::string>& items)=0;

    virtual Error ReadPlaylist( const char* url, 
                                std::vector<PlaylistItem*>* items,
                                PLMCallBackFunction function = NULL,
                                void* cookie = NULL) = 0;
    virtual Error WritePlaylist(const char* url, PlaylistFormatInfo* format, 
                                std::vector<PlaylistItem*>* items,
                                PLMCallBackFunction function = NULL,
                                void* cookie = NULL) = 0;

  protected:
    FAContext *m_context;
};

typedef PlaylistFormat* PlaylistFormatRef;

class PlaylistFormatInfo {

 public:

    PlaylistFormatInfo() {}
    
    virtual ~PlaylistFormatInfo() {}

    Error SetExtension(const char* extension)
    { m_extension = extension; return kError_NoErr; }
    const char* GetExtension() const { return m_extension.c_str(); }

    Error SetDescription(const char* description) 
    { m_description = description; return kError_NoErr; }
    const char* GetDescription() { return m_description.c_str(); }

    Error SetRef(PlaylistFormatRef ref) { m_ref = ref; return kError_NoErr; }
    const PlaylistFormatRef GetRef() const { return m_ref; }

 private:
  
    std::string m_extension;
    std::string m_description;
    PlaylistFormatRef m_ref;
};



#endif // _PLAYLIST_FORMAT_H_

/* arch-tag: 659cab35-821a-4f02-95df-30f3aa8882d4
   (do not change this comment) */

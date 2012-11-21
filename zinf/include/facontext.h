/*____________________________________________________________________________
	
	Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)

        Portions Copyright (C) 1999 EMusic.com
	Copyright (C) 1999 Mark H. Weaver <mhw@netris.org>

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
	
	$Id: facontext.h,v 1.5 2003/09/16 17:58:13 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_FACONTEXT_H_
#define INCLUDED_FACONTEXT_H_

#include <stdint.h>

#include "config.h"
#include "preferences.h"
#include "log.h"
#include "mutex.h"

static const int32_t c_majorVersion = 1;
static const int32_t c_minorVersion = 0;

// argFlags bits
#define FAC_ARGFLAGS_SAVE_STREAMS 1

class PlaylistManager;
class Properties;
class EventQueue;
class MusicCatalog;
class DownloadManager;
class UpdateManager;
class Player;
class TimerManager;
class APSInterface;
class MusicCollection;

class FAContext
{
 public:
    FAContext()
	: majorVersion(c_majorVersion),
	  minorVersion(c_minorVersion),
	  prefs(0),
	  log(0),
          props(0),
          plm(0),
          target(0),
	  catalog(0),
          downloadManager(0),
          aps(0),
          //updateManager(0),
          player(0),
#ifdef HAVE_GTK
	  gtkInitialized(false),
          gtkRunning(false),
#endif	  
#ifdef WIN32
      hInstance(0),
#endif      
	  argc(0),
          argv(0),	  
	  argFlags(0) { }
    
    ~FAContext()
    {
	    if (log)
	        delete log;
	    if (prefs)
	        delete prefs;
    }

    bool CompatibleVersion() { return majorVersion == c_majorVersion; }

    int32_t majorVersion, minorVersion;
    Preferences *prefs;
    LogFile *log;
    Properties *props;
    PlaylistManager *plm;
    EventQueue *target;
    MusicCatalog *catalog;
    MusicCollection *collection;
    DownloadManager *downloadManager;
    APSInterface *aps;
    //UpdateManager *updateManager;
    TimerManager *timerManager;
    Player *player;
#ifdef HAVE_GTK
    Mutex gtkLock;
    bool gtkInitialized;
    bool gtkRunning;
#endif    
#ifdef WIN32
    HINSTANCE hInstance;
#endif
    int32_t argc;
    char** argv;
    uint32_t argFlags;
};

#endif // _FACONTEXT_H_

/* arch-tag: c6ef6725-84c8-4d43-aae7-dfcb4b197370
   (do not change this comment) */

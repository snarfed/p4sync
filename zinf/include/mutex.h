/*____________________________________________________________________________
	
	Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)
	Portions copyright (C) 1998-2000 EMusic.com

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
	
	$Id: mutex.h,v 1.4 2003/09/16 17:34:54 kgk Exp $
____________________________________________________________________________*/

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include "config.h"
#define WAIT_FOREVER -1

#ifdef DEBUG_MUTEXES
#include <vector>
#include <string>

typedef struct TraceRef {
    std::string file;
    int    line;
};

typedef struct MutexInfo {
    void               *address;
    TraceRef           *owner;
    vector<TraceRef *> *waits;
};  
#endif

class Mutex {

public:
         Mutex(bool createOwned = false);
        ~Mutex();

#ifdef DEBUG_MUTEXES
    bool __Acquire(char *name, int line, long timeout = WAIT_FOREVER);
#define Acquire() __Acquire(__FILE__, __LINE__)
    void DumpAllMutexes(void);
#else
    bool Acquire(long timeout = WAIT_FOREVER);
#endif

    void Release(void);
    void DumpMutex(void);

 private:
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_tCond;
    int             m_iBusy;
    pthread_t       m_tOwner;

#ifdef DEBUG_MUTEXES
    MutexInfo      *m_info;
#endif
};

#endif /* MUTEX_H */

/* arch-tag: 054ce705-c1bd-484e-96d0-51ad5025d8dd
   (do not change this comment) */

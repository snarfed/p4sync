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
	
	$Id: timer.h,v 1.6 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_TIMER_H_
#define INCLUDED_TIMER_H_

#include <vector>


#include "config.h"
#include "thread.h"
#include "semaphore.h"
#include "mutex.h"


typedef void (*TimerFunction)(void* arg);

class Timer {
 public:
     uint32_t ticks;
     uint32_t duration;
     TimerFunction function;
     void* userValue;
     Thread* thread;
};

typedef Timer* TimerRef;

class TimerManager {
 public:
    TimerManager();
    virtual ~TimerManager();

    ZINF_EXPORT void StartTimer(TimerRef* timerRef, TimerFunction function,
					uint32_t seconds, void* userValue);

    ZINF_EXPORT void StopTimer(TimerRef timerRef);

    ZINF_EXPORT void SetTimer(TimerRef timerRef, uint32_t seconds);

    static void thread_function(void* arg);
    void ThreadFunction();

    static void timer_function(void* arg);

 private:
    Thread* m_thread;
    bool m_alive;
    Semaphore m_semaphore;
    Mutex m_mutex;
    std::vector<TimerRef> m_list;
};

#endif // INCLUDED_TIMER_H_

/* arch-tag: 3ceb2935-dfb1-4c60-a197-eec2839f8f43
   (do not change this comment) */

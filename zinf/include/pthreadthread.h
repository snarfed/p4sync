/*____________________________________________________________________________
	
	Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)

	Portions Copyright (C) 1998-2000 EMusic.com

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
	
	$Id: pthreadthread.h,v 1.7 2003/09/16 17:34:54 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PTHREAD_THREAD_H
#define INCLUDED_PTHREAD_THREAD_H
#include "config.h"

#include <pthread.h>
#include "thread.h"

#include "mutex.h"
#include "semaphore.h"

class pthreadThread : public Thread{

public:
	pthreadThread();
       ~pthreadThread();


	virtual bool Create(thread_function function, void* arg,
                            bool detach = false);
	virtual void Destroy();
	virtual void Suspend();
	virtual void Resume();
	virtual void Join();
     
	virtual uint32_t GetPriority() const;
	virtual uint32_t SetPriority(uint32_t priority);

	static void *internalThreadFunction(void *);
	void *InternalThreadFunction();

private:
	pthread_t       m_threadHandle;
	unsigned	m_threadId;
	bool            m_suspended;
	Mutex          *m_suspendMutex;
	thread_function m_function;
	void           *m_arg;
	Semaphore       m_semaphore;
};

#endif /* _LINUX_THREAD_H */




/* arch-tag: 5d9ae428-e738-4afd-9ac2-9d446d12019e
   (do not change this comment) */

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
        
        $Id: queue.h,v 1.7 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_QUEUE_H_
#define INCLUDED_QUEUE_H_

#include "config.h"
#include "mutex.h"

template<class T>
class Queue {

 public:
    Queue(bool dwd = true);
    ~Queue();
    
    T Read();
    T Peek();
    void  Clear();
    int32_t Write(T &);
    bool IsEmpty();

 private:
    class QueueMember {
        public:
            QueueMember *m_behindMe;
            T m_myMember;
    };

    void GetLock();
    void ReleaseLock();

    QueueMember *m_head;
    QueueMember *m_tail;
    bool m_deleteWhenDone;
    Mutex *m_myLock;
 
};


template<class T> Queue<T>::Queue(bool dwd) {
    m_deleteWhenDone = dwd;
    m_head = NULL;
    m_tail = NULL;
    m_myLock = new Mutex();
}

template<class T> Queue<T>::~Queue() {
    GetLock();
    QueueMember *pQM = m_head;
    QueueMember *next = NULL;
    m_head = NULL;
    m_tail = NULL;
    ReleaseLock();
    while (pQM != NULL) {
        next = pQM->m_behindMe;
        if (m_deleteWhenDone) {
            if (pQM->m_myMember) {
                delete pQM->m_myMember;
            }
        }
        delete pQM;
        pQM = next;
    }
    if (m_myLock) {
        delete m_myLock;
    }
}

template<class T> T Queue<T>::Read() {
    GetLock();
    //cout << "queue:: read" << endl;
    T rtnVal = NULL;
    if (m_head) {
        //cout << "queue:: read got m_head" << endl;
        QueueMember *tmp = m_head;
        rtnVal = tmp->m_myMember;
        m_head = tmp->m_behindMe;
        delete tmp;
    }
    if (m_head == NULL) {
        // no m_tail anymore
        m_tail = NULL;
    }
    //cout << "queue:: read ending" << endl;
    ReleaseLock();
    return rtnVal;
}

template<class T> T Queue<T>::Peek() 
{
    GetLock();
    T rtnVal = NULL;

    if (m_head) 
    {
        rtnVal = m_head->m_myMember;
    }
    ReleaseLock();

    return rtnVal;
}

template<class T> int32_t Queue<T>::Write(T &c) {
    GetLock();
    QueueMember *pQM = new QueueMember();
    pQM->m_myMember = c;
    pQM->m_behindMe = NULL;
    if (m_tail) {
        m_tail->m_behindMe = pQM;
    }
    m_tail = pQM;
    if (m_head == NULL) {
        // no m_head, this T we just pushed on is now the m_head
        m_head = pQM;
    }
    ReleaseLock();
    return 0;
}

template<class T> void Queue<T>::Clear() 
{
    GetLock();

    QueueMember *pQM = m_head;
    QueueMember *next = NULL;
    m_head = NULL;
    m_tail = NULL;
    ReleaseLock();

    while (pQM != NULL) {
        next = pQM->m_behindMe;
        if (m_deleteWhenDone) {
            if (pQM->m_myMember) {
                delete pQM->m_myMember;
            }
        }
        delete pQM;
        pQM = next;
    }
}


template<class T> bool Queue<T>::IsEmpty() {
    assert(m_myLock);
    return (m_head ? false : true);
}

template<class T> void Queue<T>::GetLock() {
    assert(m_myLock);
    m_myLock->Acquire();
    return;
}

template<class T> void Queue<T>::ReleaseLock() {
    assert(m_myLock);
    m_myLock->Release();
    return;
}


#endif //_QUEUE_H_

/* arch-tag: 55b7f1ba-cdfb-49d7-a0d6-98238be9f29d
   (do not change this comment) */

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
	
	$Id: undomanager.h,v 1.6 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_UNDO_MANAGER_H_
#define INCLUDED_UNDO_MANAGER_H_

#include <assert.h>
#include <string>
#include <vector>
#include <deque>
#include <functional>


#include "config.h"
#include "facontext.h"

#include "errors.h"
#include "mutex.h"
#include "semaphore.h"
#include "thread.h"

#define kRootIndex 0xFFFFFFFF

class UndoItem {
 public:
    virtual ~UndoItem() { }

    virtual void Undo() {};
    virtual void Redo() {};
};

class UndoMultiItem : public UndoItem {
 public:
            UndoMultiItem();
    virtual ~UndoMultiItem();

    virtual void Undo();
    virtual void Redo();

    void AddItem(UndoItem* item); 
 private:
    std::vector<UndoItem*> m_items;
};

class UndoManager {

 public:
    ZINF_EXPORT UndoManager();
    ZINF_EXPORT virtual ~UndoManager();
    
    ZINF_EXPORT void Undo();
    ZINF_EXPORT void Redo();

    ZINF_EXPORT bool CanUndo();
    ZINF_EXPORT bool CanRedo();

    void AddItem(UndoItem* item); 

    void Clear();

 protected:
    

 private:
    Mutex m_mutex;
    std::vector<UndoItem*> m_items;
    uint32_t m_current;
    bool m_doing;
   
};

#endif // INCLUDED_UNDO_MANAGER_H_










/* arch-tag: 08935082-4b79-4d22-a966-5fc29dfb4f5d
   (do not change this comment) */

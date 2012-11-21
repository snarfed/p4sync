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
	
	$Id: propimpl.h,v 1.5 2003/10/20 00:51:03 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PROPIMPL_H_
#define INCLUDED_PROPIMPL_H_

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#include "config.h"
#ifdef WIN32
#pragma warning(disable:4786)
#endif

// #include <stdlib.h>

#include <vector>
#include <map>
#include <string>



#include "mutex.h"
#include "properties.h"
#include "debug.h"

class PropElem 
{
   public:
       PropElem() 
         { m_val = NULL; }
      ~PropElem() 
         {  
            if (m_val) 
            {
               Debug_v("Propelem: delete %p", m_val);
               delete m_val; 
            }   
         }
         
      std::vector<PropertyWatcher *> m_propWatchers;
      PropValue *m_val;
};

class PropertiesImpl : public Properties {
 private:
    std::map<std::string, PropElem *> m_props;
    Mutex m_lock;
 public:
    PropertiesImpl();
    virtual ~PropertiesImpl();
    virtual Error GetProperty(const std::string& prop, PropValue **);
    virtual Error SetProperty(const std::string& prop, PropValue *);
    virtual Error RegisterPropertyWatcher(const std::string& prop, PropertyWatcher *);
    virtual Error RemovePropertyWatcher(const std::string& prop, PropertyWatcher *);
};

#endif // _PROPIMPL_H_


/* arch-tag: c986a442-413c-44b3-975c-533c24f0f131
   (do not change this comment) */

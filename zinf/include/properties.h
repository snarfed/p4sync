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
	
	$Id: properties.h,v 1.5 2003/10/20 00:51:03 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PROPERTIES_H_
#define INCLUDED_PROPERTIES_H_

#include <string.h>
#include <map>
#include "config.h"
#include "errors.h"
#include "mutex.h"

class PropValue {
 public:
    virtual ~PropValue() {}
};


class StringPropValue : public PropValue {
 public:
    StringPropValue(const std::string& str) 
      { m_str = str; };

    virtual ~StringPropValue()
      {};

    const char *GetString() { return m_str.c_str(); }
 private:
    std::string m_str;
};


class Int32PropValue : public PropValue {
 public:
    Int32PropValue(int32_t i) {
	m_int = i;
    }
    virtual ~Int32PropValue() {}
    int32_t GetInt32() { return m_int; }
 private:
    int32_t m_int;
};


class PropertyWatcher {
 public:
    virtual Error PropertyChange(const std::string& prop, PropValue *) = 0;
};



class PropElem;

class Properties {
 private:
    std::map<std::string, PropElem *> m_props;
    Mutex m_lock;
 public:
    Properties();
    virtual ~Properties();
    virtual Error GetProperty(const std::string& prop, PropValue **);
    virtual Error SetProperty(const std::string& prop, PropValue *);
    virtual Error RegisterPropertyWatcher(const std::string& prop, PropertyWatcher *);
    virtual Error RemovePropertyWatcher(const std::string& prop, PropertyWatcher *);
};


#endif // _PROPERTIES_H_

/* arch-tag: 478974fe-af9f-4637-9fc9-7a3b9ed1967a
   (do not change this comment) */

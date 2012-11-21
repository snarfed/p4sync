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
	
	$Id: registry.h,v 1.8 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_REGISTRY_H_
#define INCLUDED_REGISTRY_H_
#include "config.h"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#if _MSC_VER <= 1100
#define STRICT
#endif
#include <windows.h>
#include <stdint.h>
#endif

#include <vector>
#include <string>




class FAContext;

#ifdef WIN32
typedef HMODULE ModuleRef;
#else
typedef void* ModuleRef;
#endif

typedef void *(*InitializeFunction)(FAContext *);

class ZINF_EXPORT RegistryItem {
 public:
    RegistryItem();
    virtual ~RegistryItem();

    const std::string& getPath() {return m_path;}
    const std::string& getName() {return m_name;}
    const std::string& getDescription() {return m_description;}


    void SetPath(char* path);
    const char* Path() {return m_path.c_str();}

    void SetName(char* name);
    const char* Name() {return m_name.c_str();}

    void SetDescription(char* description);
    const char* Description() { return m_description.c_str();}

    void SetInitFunction(InitializeFunction function);
    const InitializeFunction InitFunction() const { return m_init;}

    void SetModule(ModuleRef module);
    const ModuleRef Module() const { return m_module;}

    bool operator < (const RegistryItem& x) const
    {
        return (x.m_path < m_path);
    }

    bool operator == (const RegistryItem& x) const
    {
        return (x.m_path == m_path);
    }

 private:
    std::string              m_path;
    std::string              m_name;
    std::string              m_description;
    InitializeFunction  m_init;
    ModuleRef           m_module;

};

class Registry {
 public:
    ZINF_EXPORT Registry();
    ZINF_EXPORT virtual ~Registry();

    ZINF_EXPORT void AddItem(RegistryItem* item);
    ZINF_EXPORT RegistryItem* GetItem(int32_t index);
    ZINF_EXPORT int32_t CountItems();

 private:
    std::vector<RegistryItem*> m_elements;
};


#endif // _REGISTRY_H_

/* arch-tag: c521226e-63a5-4a2a-b7e4-f8cd2010151d
   (do not change this comment) */

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
	
	$Id: registrar.h,v 1.5 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_REGISTRAR_H_
#define INCLUDED_REGISTRAR_H_

#include "config.h"
#include "errors.h"
#include "registry.h"
#include "preferences.h"


#ifndef WIN32
#include "win32impl.h"
#endif

class Registrar {
 public:
    Registrar(){m_subdir = NULL; m_search = NULL;}
    ~Registrar(){}

    ZINF_EXPORT Error InitializeRegistry(Registry* registry, Preferences* prefs);
    static Error CleanupRegistry(Registry* registry);

    void SetSubDir(const char* subdir){m_subdir = subdir;}
    const char* SubDir(void) const {return m_subdir;}
    void SetSearchString(const char* search){m_search = search;}
    const char* SearchString(void) const {return m_search;}

#ifdef WIN32
    void GetPluginDescription(char* file, RegistryItem* item);
#endif

 private:
    const char* m_subdir;
    const char* m_search;
    
};

#endif // _REGISTRAR_H_







/* arch-tag: 677cdc99-5c20-474e-9f3a-37574abaadc0
   (do not change this comment) */

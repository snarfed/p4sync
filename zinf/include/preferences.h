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
	
	$Id: preferences.h,v 1.14 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PREFERENCES_H
#define INCLUDED_PREFERENCES_H

#include <string>
#include <vector>
#include <stdint.h>

#include "config.h"
#include "errors.h"

#define ZINF_PREF_TAG(T) k##T##Pref

#define ZINF_PREF(T,V) extern const char* k##T##Pref;
#include "prefs.def"
#undef ZINF_PREF


class LibDirFindHandle;

class Preferences {
  public:
        virtual ~Preferences() { }

        virtual Error SetDefaults();
        virtual Error Save() = 0;

        virtual Preferences *ComponentPrefs(const char *componentName) = 0;

        // foundation preference functions
        virtual Error GetPrefString(const std::string& pref, 
				std::string& buffer) = 0;
        virtual Error SetPrefString(const std::string& pref, 
				const std::string& buf) = 0;

        Error GetPrefBoolean(const std::string& pref, 
				 bool& value);
        Error SetPrefBoolean(const std::string& pref, 
				 bool value);

        Error GetPrefInt32(const std::string& pref, 
			       int32_t& value);
        Error SetPrefInt32(const std::string& pref, 
			       int32_t value);

        void setDefault(const std::string&p, const char *v);
        void setDefault(const std::string&p, int v);
        void setDefault(const std::string&p, bool v);

        // <Deprecated>
        Error GetPrefString(const char *k, std::string *v) {
            return GetPrefString (std::string(k), *v);
        }
        Error SetPrefString(const char *k, std::string v) {
            return SetPrefString (std::string(k), v);
        }

        virtual Error GetPrefString(const char* pref, char* buf, uint32_t* len) = 0;
        virtual Error SetPrefString(const char* pref, const char* buf) = 0;

        Error GetPrefBoolean(const char* pref, bool* value);
        Error SetPrefBoolean(const char* pref, bool value);

        Error GetPrefInt32(const char* pref, int32_t* value);
        Error SetPrefInt32(const char* pref, int32_t value);
	// </Deprecated>


// 	// Iterators for the zinf library path
//         virtual LibDirFindHandle *GetFirstLibDir(char *path, uint32_t *len);
// 	virtual Error GetNextLibDir(LibDirFindHandle *hLibDirFind,
// 				char *path, uint32_t *len);
//         virtual Error GetLibDirClose(LibDirFindHandle *hLibDirFind);

	// XXX: This is a non-portable hack, and should only be used for user
	//      messages.  Normally, you should use the iterators above instead.
        virtual  std::string GetLibPath();


        const std::vector<std::string>& getLibDirs() {
            return m_libdirs;
        }
    protected:

        std::vector<std::string> m_libdirs;
};

#endif /* _PREFERENCES_H */

/* arch-tag: f27ee849-a8d1-4891-821c-b98ed217932b
   (do not change this comment) */

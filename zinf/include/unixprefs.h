/*____________________________________________________________________________
	
	Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)

	Portions Copyright (C) 1998-1999 EMusic.com
	Portions Copyright (C) 1999 Mark H. Weaver <mhw@netris.org>

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
	
	$Id: unixprefs.h,v 1.10 2003/09/16 17:34:54 kgk Exp $
____________________________________________________________________________*/


#ifndef INCLUDED_UNIXPREFS_H
#define INCLUDED_UNIXPREFS_H

#include <string>
#include <vector>
#include <map>

#include "config.h"
#include "errors.h"
#include "preferences.h"
#include "win32impl.h"
#include "mutex.h"


class UnixPrefEntry
{
 public:
    UnixPrefEntry();
    ~UnixPrefEntry();

    std::string prefix;	// Preceeding comments and indentation
    std::string key;
    std::string separator;
    std::string value;
    std::string suffix;	// Trailing space and comment (including NL)

    int operator == (const UnixPrefEntry& e) {
      return key == e.key;
    }
};

class UnixPrefs : public Preferences
{
 public:
    UnixPrefs();
    ~UnixPrefs();

    virtual Error SetDefaults();
    virtual Error Save();

    virtual Preferences *ComponentPrefs(const char *componentName);

    virtual Error GetPrefString(const std::string& pref, std::string& buffer);
    virtual Error SetPrefString(const std::string& pref, 
				const std::string& buffer);

    //<Deprecated>
    Error GetPrefString(const char* pref, char* buf, uint32_t* len);
    Error SetPrefString(const char* pref, const char* buf);
    //</Deprecated>

#if 0
    virtual LibDirFindHandle *GetFirstLibDir(char *path, uint32_t *len);
    virtual Error GetNextLibDir(LibDirFindHandle *hLibDirFind,
                   		char *path, uint32_t *len);
    virtual Error GetLibDirClose(LibDirFindHandle *hLibDirFind);
#endif

    std::string GetLibPath();

    int GetErrorLineNumber() const { return m_errorLineNumber; }

 private:
#if 0 
    const char*  kDefaultLibraryPath;
    const char*  kDefaultUI;
    const char*  kDefaultTextUI;
    const char*  kDefaultPMO;
    const char*  kDefaultALSADevice;
    const char*  kDefaultESOUNDHost;
#endif

    Mutex m_mutex;

//     HKEY   m_prefsKey;
    char *m_libDirs;

    char *m_prefsFilePath;
    int m_errorLineNumber;      // 0 if no error
    bool m_saveEnable, m_changed;

    typedef vector<UnixPrefEntry> EntriesT;
    typedef std::map<std::string, std::string> EntryMapT;
    EntriesT m_entries;
    EntryMapT m_ht;

};

#endif /* _UNIXPREFS_H */

/* arch-tag: 6f11dccb-2e85-41f3-b0c8-240a27199149
   (do not change this comment) */

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
	
	$Id: utility.h,v 1.12 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_UTILITY_H
#define INCLUDED_UTILITY_H

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#include "config.h"
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <vector>

#include "facontext.h"
#include "errors.h"
#include "preferences.h"
#include "i18n.h"

ZINF_EXPORT void CreateDirectoryPath(const char* path);
ZINF_EXPORT bool IsRelative(const char *path);
ZINF_EXPORT void ResolvePath(std::string&);
ZINF_EXPORT void RFC822GMTTimeString(struct tm* time, char buf[32]);
ZINF_EXPORT char *strdup_new(const char *str);
ZINF_EXPORT char *ZinfDir(Preferences *prefs);

ZINF_EXPORT Error FilePathToURL(const std::string& path, std::string& url);
ZINF_EXPORT Error URLToFilePath(const std::string& url, std::string& path);

ZINF_EXPORT Error FilePathToURL(const char* path, std::string& url);
ZINF_EXPORT Error URLToFilePath(const char* url, std::string& path);


ZINF_EXPORT void ToUpper(char *s);
ZINF_EXPORT void ToLower(char *s);

// "Blah blah" to "Blah%20blah"
ZINF_EXPORT void ReplaceSpaces(std::string &in, std::string &encoded); 


#ifndef WIN32
ZINF_EXPORT void LaunchBrowser(const char* url);
ZINF_EXPORT void LaunchBrowser(Preferences*, const std::string& url);
ZINF_EXPORT bool CopyFile(const char *pExistingFileName, 
              const char *pNewFileName,      // name of new file
              bool bFailIfExists);     // operation if file exists
#endif

ZINF_EXPORT void FindMusicFiles(const char* rootPath, 
                    std::vector<std::string>& urls, 
                    std::vector<std::string>& queries); 
#ifdef WIN32
ZINF_EXPORT bool ResolveLink(std::string& path);
#endif

/* func to find the real filename of a path, irregardless of case */
ZINF_EXPORT std::string FindFile(std::string oPath);

ZINF_EXPORT bool ShowHelp(FAContext *context, const char *helptopic);
ZINF_EXPORT bool GetProxySettings(FAContext *context, std::string &server, 
				  unsigned short &port);


ZINF_EXPORT std::vector<std::string> SplitPath(const std::string& path);


#endif /* INCLUDED_UTILITY_H */

/* arch-tag: bae54419-8ae4-4f65-bd12-1f2b54040e48
   (do not change this comment) */

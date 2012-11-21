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

        $Id: missingfile.h,v 1.5 2003/09/16 17:34:53 kgk Exp $
 ____________________________________________________________________________*/

#ifndef INCLUDED_MISSINGFILE_H_
#define INCLUDED_MISSINGFILE_H_

#include <string>

#include "eventdata.h"

class FAContext;

class ZINF_EXPORT MissingFile 
{
 public:
    MissingFile(FAContext *context);
    virtual ~MissingFile();

    Error FindMissingFile(PlaylistItem *item, const std::string &searchPath, 
                          std::string &newUrl);
    Error AcceptLocation(PlaylistItem *item,
                         const std::string &newUrl);

 protected:

    Error RecurseFindMissingFile(const std::string &file, 
				 const std::string &searchPath, 
                                 std::string &newUrl);

    FAContext *m_context;
 private:
};
#endif

/* arch-tag: 6daeea69-0f85-4f61-8355-bfa287267e9f
   (do not change this comment) */

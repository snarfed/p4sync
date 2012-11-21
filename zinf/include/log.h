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
        along with this program; if not, Write to the Free Software
        Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
        
        $Id: log.h,v 1.5 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/
#ifndef INCLUDED_LOG_H__
#define INCLUDED_LOG_H__

#include <stdio.h>
#include "config.h"
#include "i18n.h"

enum 
{
    LogMain    = 0,
    LogDecode  = 1,
    LogInput   = 2,
    LogOutput  = 3,
    LogPerf    = 4,

    LogLast    = 5,
    LogMax     = 32
};

class ZINF_EXPORT LogFile
{
    public:

                 LogFile(const char *szLogFileArg);
        virtual ~LogFile(void);

        bool     Open(void);
        bool     Close(void);
        void     ClearLogLevel(void) 
                 { m_iLogLevelFlags = 0; };
        void     AddLogLevel(int iLogLevelFlags);

        void     Error(const char *Format, ...);
        void     Log(int Section, const char *Format, ...);

    private:

        FILE *m_fpLog;
        char *m_szLogFile;
        int   m_iLogLevelFlags;
};

#endif

/* arch-tag: af9e871e-db8d-4f6c-8134-bc6335bc8190
   (do not change this comment) */

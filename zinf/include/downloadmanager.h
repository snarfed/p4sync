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
	
	$Id: downloadmanager.h,v 1.12 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_DOWNLOAD_MANAGER_H_
#define INCLUDED_DOWNLOAD_MANAGER_H_
#include "config.h"
#include <assert.h>
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <fstream>

#ifdef WIN32
#include <io.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif


#include "config.h"
#include "facontext.h"

#include "errors.h"
#include "mutex.h"
#include "semaphore.h"
#include "thread.h"
#include "metadata.h"
#include "registry.h"
#include "downloadformat.h"

#if !defined(WIN32) && !defined(HAVE_BEOS)
#include <arpa/inet.h>
#endif

#define kInvalidIndex 0xFFFFFFFF

typedef enum {
    kDownloadItemState_Null,
    kDownloadItemState_Queued,
    kDownloadItemState_Downloading,
    kDownloadItemState_Cancelled,
    kDownloadItemState_Paused,
    kDownloadItemState_Error,
    kDownloadItemState_Done

} DownloadItemState;

class DownloadItem {
 public:
    DownloadItem()
    {
        m_state = kDownloadItemState_Null;
        m_error = kError_NoErr;
        m_bytesTotal = 0;
        m_bytesReceived = 0;
        m_normalDownload = false;
#ifdef IPV6
	memset(&src_addr6, 0, sizeof(struct in6_addr));
#endif
    }

    DownloadItem(const char* src, const char* dest = NULL, 
		 const MetaData* metadata = NULL)
    {
        assert(src);

        if(src) {
#ifdef IPV6
		if ((inet_pton(AF_INET6, src, &src_addr6))>0) {
			printf("IPv6 Addr: %s\n", src);
			m_ipv6 = true;
		} else {
			m_ipv6 = false;
			SetSourceURL(src);
		}
#else
		SetSourceURL(src);
#endif		
	}

        if(dest)
            SetDestinationFile(dest);

        if(metadata)
		{
            SetMetaData(metadata);
        }

        m_state = kDownloadItemState_Null;
        m_error = kError_NoErr;
        m_bytesTotal = 0;
        m_bytesReceived = 0;
        m_normalDownload = false;
    }

    virtual ~DownloadItem() {}

    Error SetMetaData(const MetaData* metadata)
    {
        Error result = kError_InvalidParam;
        assert(metadata);

        if(metadata)
        {
            result = kError_NoErr;

            m_metadata = *metadata;
        }

        return result;
    }

    const MetaData& GetMetaData() const { return m_metadata; }

    Error SetSourceURL(const std::string &url) 
		{ m_src = url; return kError_NoErr;}
    Error GetSourceURL(char* buf, uint32_t* len) 
		{ return SetBuffer(buf, m_src.c_str(), len); }
    const std::string& SourceURL() const { return m_src; }

    Error SetSourceCookie(const std::string &cookie) 
		{ m_cookie = cookie; return kError_NoErr;}
    Error GetSourceCookie(char* buf, uint32_t* len) 
		{ return SetBuffer(buf, m_cookie.c_str(), len); }
    const std::string& SourceCookie() const { return m_cookie; }

    Error SetDestinationFile(const std::string &file) 
		{ m_dest = file; return kError_NoErr;}
    Error GetDestinationFile(char* buf, uint32_t* len) 
		{ return SetBuffer(buf, m_dest.c_str(), len); }
    const std::string& DestinationFile() const { return m_dest; }

    Error SetPlaylistName(const std::string &file) 
		{ m_playlist = file; return kError_NoErr;}
    Error GetPlaylistName(char* buf, uint32_t* len) 
		{ return SetBuffer(buf, m_playlist.c_str(), len); }
    const std::string& PlaylistName() const { return m_playlist; }

    DownloadItemState GetState() const { return m_state; }
    void SetState(DownloadItemState state) { m_state = state; }

    void SetDownloadError(Error error) { m_error = error; }
    Error GetDownloadError() const { return m_error; }

    void SetTotalBytes(uint32_t bytes) { m_bytesTotal = bytes; }
    uint32_t GetTotalBytes() const { return m_bytesTotal; }

    void SetBytesReceived(uint32_t bytes) { m_bytesReceived = bytes; }
    uint32_t GetBytesReceived() const { return m_bytesReceived; }

    void SetNormalDownload(void) { m_normalDownload = true; };
	bool IsNormalDownload(void) { return m_normalDownload; };

    Error SetMTime(const std::string &mtime) 
		{ m_mtime = mtime; return kError_NoErr;}
    Error GetMTime(char* buf, uint32_t* len) 
		{ return SetBuffer(buf, m_mtime.c_str(), len); }
    const std::string& MTime() const { return m_mtime; }

 protected:

#ifdef WIN32 
    Error Win32GetHostByName(char *szHostName, struct hostent *pHostInfo);
#endif

    Error SetBuffer(char* dest, const char* src, uint32_t* len)
    {
        Error result = kError_InvalidParam;

        assert(dest);
        assert(src);
        assert(len);

        if(dest && src)
        {
            uint32_t srclen = strlen(src) + 1;

            if(*len >= srclen)
            {
                strcpy(dest, src);
                result = kError_NoErr;
            }
            else
            {
                result = kError_BufferTooSmall;
            }

            *len = srclen;
        }

        return result;
    }

 private:
    MetaData m_metadata;
    std::string m_src;
    std::string m_dest;
    std::string m_cookie;
    std::string m_playlist;
    std::string m_mtime;
    DownloadItemState m_state;
    bool m_allowResume;
    Error m_error;
    uint32_t m_bytesReceived;
    uint32_t m_bytesTotal;
    bool   m_normalDownload;
#ifdef IPV6
	bool m_ipv6;
	struct in6_addr src_addr6;
#endif
};

struct sockaddr;

class DownloadManager {

 public:
    DownloadManager(FAContext* context);
    virtual ~DownloadManager();

    // Functions for adding items to Download Manager
    // Adding an item implicitly queues it for
    // downloading.
    ZINF_EXPORT Error AddItem(const char* url, const char* filename = NULL);
    ZINF_EXPORT Error AddItem(DownloadItem* item);
    ZINF_EXPORT Error AddItems(std::vector<DownloadItem*>* list);

    // Functions for removing items from Download Manager
    // Removing an item implicitly cancels a download
    // that is occurring.
    ZINF_EXPORT Error RemoveItem(DownloadItem* item);
    ZINF_EXPORT Error RemoveItem(uint32_t index);
    ZINF_EXPORT Error RemoveItems(uint32_t index, uint32_t count);
    ZINF_EXPORT Error RemoveItems(std::vector<DownloadItem*>* items);
    ZINF_EXPORT Error RemoveAll();

    // Changes item state to queued if it is cancelled or error.
    // This will indicate to the download thread that it should
    // attempt to retrieve this item. Has no effect if the item's
    // state is Done, or Downloading.
    ZINF_EXPORT Error QueueDownload(DownloadItem* item, 
				    bool bDownloadNow = false);
    ZINF_EXPORT Error QueueDownload(uint32_t index);

    // Changes item state to cancelled if it is queued or downloading.
    // If allowResume is true then data is retained for later download.
    // Has no effect if the item's state is Done, Cancelled, or Error.
    ZINF_EXPORT Error CancelDownload(DownloadItem* item, 
				     bool allowResume = false);
    ZINF_EXPORT Error CancelDownload(uint32_t index, bool allowResume = false);

    // These functions suspend/resume the current download progress.
    ZINF_EXPORT void  PauseDownloads(void);
    ZINF_EXPORT void  ResumeDownloads(void);
    ZINF_EXPORT bool  IsPaused(void);

    // File Format support
    ZINF_EXPORT Error GetSupportedDownloadFormats(DownloadFormatInfo* format, 
						  uint32_t index);
    ZINF_EXPORT Error ReadDownloadFile(const char* url, 
			    std::vector<DownloadItem*>* items = NULL);

    // Utility Functions
    bool            IsEmpty();
    uint32_t          CountItems();
    DownloadItem*   ItemAt(uint32_t index);
	uint32_t          IndexOf(DownloadItem* item);
    bool            HasItem(DownloadItem* item);

 protected:
    inline uint32_t CheckIndex(uint32_t index);
    uint32_t InternalIndexOf(std::vector<DownloadItem*>* list, 
			   DownloadItem* item);    

    static void download_thread_function(void* arg);
    void DownloadThreadFunction();

    DownloadItem* GetNextQueuedItem();

    Error Download(DownloadItem* item);
    void CleanUpDownload(DownloadItem* item);
    Error SubmitToDatabase(DownloadItem* item);

    void SendItemAddedMessage(DownloadItem* item);
    void SendItemRemovedMessage(DownloadItem* item);
    void SendStateChangedMessage(DownloadItem* item);
    void SendProgressMessage(DownloadItem* item);

    void LoadResumableDownloadItems();
    void SaveResumableDownloadItems();
    bool DoesDBDirExist(const char* path);

    Error Recv(int hHandle, char *pBuffer, int iSize, int iFlags, int &iRet, DownloadItem *item);
    Error Send(int hHandle, char *pBuffer, int iSize, int iFlags, int &iRet, DownloadItem *item);
    Error Connect(int hHandle, const sockaddr *pAddr, int &iRet, DownloadItem *item);

#ifdef WIN32 
    HWND m_hWnd;
    Error Win32GetHostByName(char *szHostName, struct hostent *pHostInfo);
#endif

 private:

    FAContext* m_context;

    Mutex m_mutex;

    std::vector<DownloadItem*> m_itemList;
	int                   m_downloadIndex;

    Registry m_formatRegistry;

    std::vector<DownloadFormatInfo*> m_formats;
   
    uint32_t m_current;

    Thread* m_downloadThread;
    volatile bool m_runDownloadThread;

    Semaphore m_queueSemaphore;
    Mutex m_quitMutex;
    bool  m_downloadsPaused, m_exit;
};

#endif // INCLUDED_DOWNLOAD_MANAGER_H_










/* arch-tag: b5098502-5aa6-4d50-b38e-93334cf988ad
   (do not change this comment) */

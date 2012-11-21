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

        $Id: musiccatalog.h,v 1.12 2003/10/20 00:51:03 kgk Exp $
 ____________________________________________________________________________*/

#ifndef INCLUDED_MUSICBROWSER_H_
#define INCLUDED_MUSICBROWSER_H_

#include <vector>
#include <map>
#include <set>
#include <string>

#include "config.h"
#include "event.h"
#include "database.h"
#include "metadata.h"
#include "playlist.h"
#include "timer.h"

class FAContext;
class Player;

typedef enum {
   kTypeTrack = 0,
   kTypeStream
} MetadataStorageType;

class AlbumList {
 public:
     AlbumList() { m_trackList = new std::vector<PlaylistItem *>; }
    ~AlbumList() {
                      while (m_trackList->size() > 0) {
                          delete (*m_trackList)[0];
                          m_trackList->erase(m_trackList->begin());
                      }
                      delete m_trackList;
                      m_trackList = NULL;
                  }

    std::vector<PlaylistItem *> *m_trackList;
    std::string name;
};

class ArtistList {
 public:
    ArtistList() { m_albumList = new std::vector<AlbumList *>; }
   ~ArtistList() {
                     while (m_albumList->size() > 0) {
                         delete (*m_albumList)[0];
                         m_albumList->erase(m_albumList->begin());
                     }
                     delete m_albumList;
                     m_albumList = NULL;
		 }
    
    std::vector<AlbumList *> *m_albumList;
    std::string name;
};

class ZINF_EXPORT MusicCatalog : public EventQueue
{
 public:
    MusicCatalog(FAContext *context, char *databasepath = NULL);
    virtual ~MusicCatalog();

    // std::string GetFilename(const std::string &strGUID);

    void StartTimer(void);
    void SetDatabase(const std::string& path);

    void SearchMusic(std::vector<std::string> &pathList, 
		     bool bBrowserMessages = true);
    void StopSearchMusic(void);

    void WriteMetaDataToDatabase(const std::string& url, const MetaData metadata,
                                 MetadataStorageType type = kTypeTrack);
    MetaData *ReadMetaDataFromDatabase(const char *url);

    Error AddPlaylist(const char *url);
    Error AddSong(const char *url);
    Error AddStream(const char *url);
    Error Add(const char *url);    

    PlaylistItem *GetPlaylistItemFromURL(const char *url);

    Error UpdateSong(PlaylistItem *item);

    Error RePopulateFromDatabase();

    Error RemovePlaylist(const char *url);
    Error RemoveSong(const char *url);
    Error RemoveStream(const char *url);
    Error Remove(const char *url);

    void  ClearCatalog(void);

    void  Sort(void);

    const std::vector<ArtistList *> *GetMusicList(void) { return m_artistList; }
    const std::vector<PlaylistItem *> *GetUnsortedMusic(void) { return m_unsorted; }
    const std::vector<std::string> *GetPlaylists(void) { return m_playlists; }
    const std::vector<PlaylistItem *> *GetStreams(void) { return m_streams; }

    void  GetCatalogLock(void) { m_catMutex->Acquire(); }
    void  ReleaseCatalogLock(void) { m_catMutex->Release(); }

    virtual Error AcceptEvent(Event *e);

    void PruneDirectory(std::string &directory);

    int32_t GetTotalNumTracks(void) { return m_trackCount; }
    
 protected:
    static void musicsearch_thread_function(void *arg);
    static void prune_thread_function(void *arg);
    
    void PruneThread(bool sendmessages);
    void DoSearchMusic(char *path, bool bSendMessages);
    void DoSearchPaths(std::vector<std::string> &pathList, bool bSendMessages);
    void PruneDatabase(bool sendmessages = false, bool spawn = false);

    /**
     * Performs a find operation for the <code>ArtistList</code> entry
     * with the specified name. Returns <code>true</code> if found. The
     * iterator will be updated to the point to insert a new artist to
     * retain ordering (if <code>false</code> is returned), otherwise it
     * will point to the <code>ArtistList</code> found.
     */
    bool FindArtistList(const std::string& artistName, std::vector<ArtistList*>::iterator& it) const;

    /**
     * Performs a find operation for the <code>AlbumList</code> entry
     * with the specified name in the specified
     * <code>ArtistList</code>. Returns <code>true</code> if
     * found. The iterator will be updated to the point to insert a
     * new artist to retain ordering (if <code>false</code> is
     * returned), otherwise it will point to the
     * <code>AlbumList</code> found.
     */
    bool FindAlbumList(const std::string& albumName, std::vector<AlbumList*>* albumList, std::vector<AlbumList*>::iterator& it) const;

    /**
     * Performs a find operation for the <code>PlaylistItem</code>
     * entry with the specified URL in the specified
     * <code>PlaylistItem</code> vector. Returns <code>true</code> if
     * found. The iterator will be updated to the point to insert a
     * new artist to retain ordering (if <code>false</code> is
     * returned), otherwise it will point to the
     * <code>PlaylistItem</code> found.
     */
    bool FindPlaylistItemByUrl(const std::string& url, std::vector<PlaylistItem*>* trackList, std::vector<PlaylistItem*>::iterator& it) const;

    bool m_exit;
    Mutex *m_mutex;

    FAContext *m_context;

    static void watch_timer(void *arg);
    void        WatchTimer();

 private:
    void GenerateSignature(PlaylistItem *track);

    bool CaseCompare(const std::string &s1, const std::string &s2) const;

    Mutex *m_catMutex;
    Mutex *m_timerMutex;

    std::vector<ArtistList *> *m_artistList;
    std::vector<PlaylistItem *> *m_unsorted;
    std::vector<std::string> *m_playlists;
    std::vector<PlaylistItem *> *m_streams;

    Database *m_database;

    bool   m_inUpdateSong;
    bool   m_addImmediately;
    bool   m_bSurpressAddMessages;

    PlaylistItem *m_newItem;
    PlaylistItem *m_oldItem;
    ArtistList *m_newArtist;
    ArtistList *m_oldArtist;
    AlbumList *m_newAlbum;
    AlbumList *m_oldAlbum;

    int32_t    m_timeout;
    TimerRef m_watchTimer;

    int32_t    m_trackCount;
  
    bool     m_killGUIDs;
    
    int      m_pendingMBLookups;
    Mutex   *m_MBLookupLock;
    std::vector<std::pair<std::string, std::string> > *m_MBRequests;
    bool     m_MBLookupThreadActive;
    bool     m_killMBThread;
};
#endif

/* arch-tag: 89e84494-0c64-469f-8d0d-79e241010792
   (do not change this comment) */

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
	
	$Id: playlist.h,v 1.14 2003/09/16 17:58:13 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PLAYLIST_H_
#define INCLUDED_PLAYLIST_H_

#include <assert.h>
#include <string>
#include <vector>
#include <functional>


#include "config.h"
#include "facontext.h"

#include "errors.h"
#include "mutex.h"
#include "thread.h"
#include "metadata.h"
#include "playlistformat.h"
#include "portabledevice.h"
#include "registry.h"
#include "plmevent.h"
#include "undomanager.h"

typedef enum {
    kPlaylistSortKey_FirstKey,
    kPlaylistSortKey_Artist = kPlaylistSortKey_FirstKey,
    kPlaylistSortKey_Album,
    kPlaylistSortKey_Title,
    kPlaylistSortKey_Year,
    kPlaylistSortKey_Track,
    kPlaylistSortKey_Genre,
    kPlaylistSortKey_Time,
    kPlaylistSortKey_Location,
    kPlaylistSortKey_FileName,
    kPlaylistSortKey_Comment,
    kPlaylistSortKey_LastKey,
    kPlaylistSortKey_Random // not used with normal sort function

} PlaylistSortKey;

typedef enum {
    kPlaylistSortType_Ascending,
    kPlaylistSortType_Descending
} PlaylistSortType;

typedef enum {
    kPlaylistKey_FirstKey,
    kPlaylistKey_MasterPlaylist = kPlaylistKey_FirstKey,
    kPlaylistKey_ExternalPlaylist,
    kPlaylistKey_PortablePlaylist,
    kPlaylistKey_LastKey

} PlaylistKey;

typedef enum {
    kPlaylistMode_RepeatNone = 0,
    kPlaylistMode_RepeatOne  = 1,
    kPlaylistMode_RepeatAll  = 2

} RepeatMode;

#define kInvalidIndex 0xFFFFFFFF

typedef enum {
    kPlaylistItemState_Normal,
    kPlaylistItemState_RetrievingMetaData,
    kPlaylistItemState_Delete

} PlaylistItemState;



/** 
 * PlaylistItem represents the metadata for a single track.
 * 
 *
 */
class PlaylistItem {

 public:
    /** 
     * Construct an empty playlist item
     */
    PlaylistItem():m_state(kPlaylistItemState_Normal){}
    PlaylistItem(const std::string&url) : 
        m_url(url),
        m_state(kPlaylistItemState_Normal)
        {}
    PlaylistItem(const char* url, const MetaData* metadata = NULL)
    {
        assert(url);

        if(url)
            SetURL(url);

        if(metadata)
            SetMetaData(metadata);

        m_state = kPlaylistItemState_Normal;
    }

    virtual ~PlaylistItem() {}

    ZINF_EXPORT Error SetMetaData(const MetaData* metadata)
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

    ZINF_EXPORT const MetaData& GetMetaData() const { return m_metadata; }

    ZINF_EXPORT Error SetURL(const std::string &url) 
		{ m_url = url; return kError_NoErr;}
    ZINF_EXPORT Error GetURL(char* buf, uint32_t* len) 
		{ return SetBuffer(buf, m_url.c_str(), len); }
    const std::string& URL() const { return m_url; }

    ZINF_EXPORT Error SetWebURL(const std::string &webUrl) 
		{ m_webUrl = webUrl; return kError_NoErr;}
    ZINF_EXPORT Error GetWebURL(char* buf, uint32_t* len) 
		{ return SetBuffer(buf, m_webUrl.c_str(), len); }
    const std::string& WebURL() const { return m_webUrl; }

    ZINF_EXPORT void SetState(PlaylistItemState state) { m_state = state; }
    ZINF_EXPORT PlaylistItemState GetState() const { return m_state; }

    ZINF_EXPORT bool operator==(const PlaylistItem& item) const
    {
        bool result = false;
        
        result = ( m_metadata == item.m_metadata &&
                   m_url == item.m_url);

        return result;
    }

    ZINF_EXPORT bool operator!=(const PlaylistItem& item) const
    {
	    return !(*this == item);
    }

    /** 
     *  Check that we can actually read this playlist item.
     *  This routine is used by the playlist readers to determine
     *  if we sghould attempt to read the item.   
     *  CAVEAT:   Currently works for local files only.
     */
    bool IsValid ();

 protected:
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
    std::string m_url, m_webUrl;
    PlaylistItemState m_state;
};

class PlaylistItemSort : 
	public std::binary_function<PlaylistItem*, PlaylistItem*, bool> {

 public:
    PlaylistItemSort(PlaylistSortKey sortKey, 
                     PlaylistSortType sortType = kPlaylistSortType_Ascending) : 
                     m_sortKey(sortKey), m_sortType(sortType) { }

    bool operator() (PlaylistItem* item1, PlaylistItem* item2) const;
   
 private:
    PlaylistSortKey m_sortKey;
    PlaylistSortType m_sortType;
};

class MetaDataSort : 
	public std::binary_function<PlaylistItem*, PlaylistItem*, bool> {

 public:
    MetaDataSort() { }

    bool operator() (MetaDataFormat* item1, MetaDataFormat* item2) const;
};

class PlaylistManager {

public:

    typedef std::vector<std::string> playlist_type;

    typedef playlist_type::value_type value_type;
    typedef playlist_type::size_type size_type;
    typedef playlist_type::iterator iterator;
    typedef playlist_type::const_iterator const_iterator;



public:  // vector interface
    iterator begin() { return m_urls.begin();}
    iterator end()   { return m_urls.end();}
    size_type size() { return m_urls.size(); }

    void clear() { m_urls.clear(); }
    void insert(iterator pos, const_iterator b, const_iterator e) {
        m_urls.insert(pos, b, e);
    }
    iterator insert(iterator b, const value_type&v){
        return m_urls.insert(b, v);
    }
    value_type& operator[](int x) {
        return m_urls.operator[](x);
    }
    void erase(iterator b, iterator e){
        m_urls.erase(b, e);
    }
    void erase(iterator b){
        m_urls.erase(b);
    }

public:

    ZINF_EXPORT PlaylistManager(FAContext* context);
    ZINF_EXPORT virtual ~PlaylistManager();

    ZINF_EXPORT Error SetCurrentItem(PlaylistItem* item);
    ZINF_EXPORT PlaylistItem* GetCurrentItem();
    
    ZINF_EXPORT Error SetCurrentIndex(uint32_t index);
    ZINF_EXPORT uint32_t GetCurrentIndex();

    ZINF_EXPORT Error GotoNextItem(bool userAction = false);
    ZINF_EXPORT Error GotoPreviousItem(bool userAction = false);

    ZINF_EXPORT bool HasAnotherItem();

    ZINF_EXPORT Error SetShuffleMode(bool shuffle);
    ZINF_EXPORT bool GetShuffleMode() const {return m_shuffle;}
    ZINF_EXPORT Error SetRepeatMode(RepeatMode mode);
    ZINF_EXPORT RepeatMode GetRepeatMode() const {return m_repeatMode;}
    ZINF_EXPORT Error ToggleRepeatMode();
    ZINF_EXPORT Error ToggleShuffleMode();

    // Functions for undoing your actions
    ZINF_EXPORT bool CanUndo();
    ZINF_EXPORT bool CanRedo();

    ZINF_EXPORT void Undo();
    ZINF_EXPORT void Redo();

    // Functions for adding items to playlist       
    ZINF_EXPORT Error AddItem(const char* url);
    ZINF_EXPORT Error AddItem(const char* url, uint32_t index);
    ZINF_EXPORT Error AddItem(const std::string& url);
    ZINF_EXPORT Error AddItem(const std::string& url, uint32_t index);
    ZINF_EXPORT Error AddItems(const std::vector<std::string>& urls);
    ZINF_EXPORT Error AddItems(const std::vector<std::string>& urls, 
			       uint32_t index);
    ZINF_EXPORT Error AddItem(PlaylistItem* item, bool queryForMetaData = true);
    ZINF_EXPORT Error AddItem(PlaylistItem* item, uint32_t index, 
			      bool queryForMetaData = true);
    ZINF_EXPORT Error AddItems(std::vector<PlaylistItem*>* list, 
			       bool queryForMetaData = true);
    ZINF_EXPORT Error AddItems(std::vector<PlaylistItem*>* list, uint32_t index, 
			       bool queryForMetaData = true);

    // Functions for removing items from playlist
    ZINF_EXPORT Error RemoveItem(PlaylistItem* item);
    ZINF_EXPORT Error RemoveItem(uint32_t index);
    ZINF_EXPORT Error RemoveItems(uint32_t index, uint32_t count);
    ZINF_EXPORT Error RemoveItems(std::vector<PlaylistItem*>* items);
    ZINF_EXPORT Error RemoveAll();

    // Functions for moving items around
    ZINF_EXPORT Error SwapItems(uint32_t index1, uint32_t index2);
    ZINF_EXPORT Error SwapItems(PlaylistItem* item1, PlaylistItem* item2);
    ZINF_EXPORT Error MoveItem(PlaylistItem* item, uint32_t index);
    ZINF_EXPORT Error MoveItem(uint32_t oldIndex, uint32_t newIndex);
    ZINF_EXPORT Error MoveItems(std::vector<PlaylistItem*>* items, 
				uint32_t index);
    ZINF_EXPORT Error MoveItems(std::vector<uint32_t>* items, uint32_t index);

    // Functions for updating
    // This function searches the items in the playlist
    // and updates the metadata if the tracks are the
    // same (matched based on URL)
    ZINF_EXPORT Error UpdateTrackMetaData(PlaylistItem* updatedTrack, bool writeToDisk = false);

    // Functions for sorting
    ZINF_EXPORT Error Sort(PlaylistSortKey key, PlaylistSortType type = kPlaylistSortType_Ascending);
    ZINF_EXPORT PlaylistSortKey GetPlaylistSortKey() const;
    ZINF_EXPORT PlaylistSortType GetPlaylistSortType() const;

    // Which playlist are we dealing with for purposes of editing:
    // 1) Master Playlist - list of songs to play
    // 2) Secondary Playlist - a playlist that we want to edit
    //      - External playlist
    //      - Portable playlist

    ZINF_EXPORT Error SetActivePlaylist(PlaylistKey key);
    ZINF_EXPORT PlaylistKey GetActivePlaylist() const;
    ZINF_EXPORT Error SetExternalPlaylist(char* url);
    ZINF_EXPORT Error GetExternalPlaylist(char* url, uint32_t* length);
    ZINF_EXPORT Error SetPortablePlaylist(DeviceInfo* device, 
                              PLMCallBackFunction function = NULL,
                              void* cookie = NULL);
    ZINF_EXPORT Error GetPortablePlaylist(DeviceInfo* device);

    // External playlist support
    ZINF_EXPORT bool  IsSupportedPlaylistFormat(const char *extension);

    ZINF_EXPORT Error GetSupportedPlaylistFormats(PlaylistFormatInfo* format, uint32_t index);

    Error ReadPlaylist(const std::string& url, 
                       std::vector<std::string>& items,
                       PLMCallBackFunction function = NULL,
                       void* cookie = NULL);

    ZINF_EXPORT Error ReadPlaylist(const char* url, 
                       std::vector<PlaylistItem*>* items = NULL, 
                       PLMCallBackFunction function = NULL,
                       void* cookie = NULL);

    ZINF_EXPORT Error WritePlaylist(const char* url, 
                        PlaylistFormatInfo* format, 
                        std::vector<PlaylistItem*>* items = NULL,
                        PLMCallBackFunction function = NULL,
                        void* cookie = NULL);

    ZINF_EXPORT Error WritePlaylist(const char* url,
                        std::vector<PlaylistItem*>* items = NULL,
                        PLMCallBackFunction function = NULL,
                        void* cookie = NULL);

    // Portable player communication
    ZINF_EXPORT Error GetSupportedPortables(DeviceInfo* device, uint32_t index);
    ZINF_EXPORT bool IsPortableAvailable(DeviceInfo* device);

    ZINF_EXPORT Error GetDeviceInfo(DeviceInfo* device);

    ZINF_EXPORT Error InitializeDevice(DeviceInfo* device, 
                           PLMCallBackFunction function = NULL,
                           void* cookie = NULL);

    ZINF_EXPORT Error ReadPortablePlaylist(DeviceInfo* device, 
                               std::vector<PlaylistItem*>* items = NULL,
                               PLMCallBackFunction function = NULL,
                               void* cookie = NULL);
    ZINF_EXPORT Error SyncPortablePlaylist(DeviceInfo* device, 
                               PLMCallBackFunction function = NULL,
                               void* cookie = NULL);

    ZINF_EXPORT Error DownloadItemFromPortable(DeviceInfo* device,
                                   PlaylistItem* item,
                                   const char* url,
                                   PLMCallBackFunction function = NULL,
                                   void* cookie = NULL);

    ZINF_EXPORT uint32_t Time();

    // Utility Functions
    ZINF_EXPORT bool            IsEmpty();
    ZINF_EXPORT uint32_t          CountItems();
    ZINF_EXPORT PlaylistItem*   ItemAt(uint32_t index);
    ZINF_EXPORT uint32_t          IndexOf(const PlaylistItem* item);
    ZINF_EXPORT bool            HasItem(const PlaylistItem* item);

    ZINF_EXPORT void RetrieveMetaData(PlaylistItem* item);
    ZINF_EXPORT void RetrieveMetaData(std::vector<PlaylistItem*>* list);

    /* and for the truely selfish function that wants its data immediately,
     * w/o spawning a thread, w/o sending messages, and no dbase lookup. 
     */
    ZINF_EXPORT void RetrieveMetaDataNow(PlaylistItem* item);

    ZINF_EXPORT bool readMetadataFile(const std::string&url, MetaDataFormat::map_type& m);
    ZINF_EXPORT bool writeMetadataFile(const std::string&url, MetaDataFormat::map_type& m);

 protected:
    inline uint32_t CheckIndex(uint32_t index);
    uint32_t InternalIndexOf(std::vector<PlaylistItem*>* list, 
			   const PlaylistItem* item);

    void AddItemToShuffleList(PlaylistItem* item);
    void AddItemsToShuffleList(std::vector<PlaylistItem*>* list);

    static void metadata_thread_function(void* arg);
    void MetaDataThreadFunction(std::vector<PlaylistItem*>* list);

    void InternalSetCurrentIndex(uint32_t index);


 private:
    void ShuffleIt(std::vector<PlaylistItem *> *toBeShuffled = NULL);
    
    FAContext* m_context;

    std::vector<PlaylistItem*>   m_masterList;
    std::vector<PlaylistItem*>   m_externalList;
    std::vector<PlaylistItem*>   m_portableList;
    std::vector<PlaylistItem*>*  m_activeList; 
    std::vector<PlaylistItem*>   m_shuffleList;

    playlist_type   m_urls;


    uint32_t           m_current;
    bool             m_shuffle;
    RepeatMode       m_repeatMode;
    PlaylistKey      m_playlistKey;
    PlaylistSortKey  m_sortKey;
    PlaylistSortType m_sortType;

    std::string      m_externalPlaylist;
    DeviceInfo  m_portableDevice;

    Mutex       m_mutex;

    Registry m_playlistRegistry;
    Registry m_portableRegistry;
    Registry m_metadataRegistry;

    std::vector<MetaDataFormat*>         m_metadataFormats;
    std::vector<PlaylistFormatInfo*>     m_playlistFormats;
    std::vector<DeviceInfo*>             m_portablePlayers;

    UndoManager m_undo;

    uint32_t m_time;

};





#endif // _PLAYLIST_H_


/* arch-tag: d2559888-c798-401e-81e7-5d1164e20828
   (do not change this comment) */

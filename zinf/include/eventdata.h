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
        
        $Id: eventdata.h,v 1.6 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_EVENTDATA_H_
#define INCLUDED_EVENTDATA_H_

#include <string>
#include <stdlib.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include "config.h"
#include "event.h"
#include "playlist.h"
#include "utility.h"

class     LogicalMediaConverter;
class     PhysicalMediaOutput;

class     UserMessageEvent:public Event
{
public:
    UserMessageEvent() : m_info() {
        m_type = INFO_UserMessage;
    }
    UserMessageEvent(const char *info) : m_info(info) {
        m_type = INFO_UserMessage;
    }
   const char *GetInfo() const { return m_info.c_str(); }
private:
    std::string m_info;
    
};

class StatusMessageEvent:public Event
{
public:
    StatusMessageEvent() : m_info() {
        m_type = INFO_StatusMessage;
    }
    StatusMessageEvent(const char *info) : m_info(info) {
        m_type = INFO_StatusMessage;
   }
   const char *GetStatusMessage() const  {return m_info.c_str();}
private:
    std::string m_info;
};

class ErrorMessageEvent:public Event
{
public:
    ErrorMessageEvent() : m_info() { 
        m_type = INFO_ErrorMessage;
    }
    ErrorMessageEvent(const char *info) : m_info(info){
      m_type = INFO_ErrorMessage;
    }
   const char *GetErrorMessage() const  {return m_info.c_str(); }
private:
    std::string m_info;
};


class     BrowserMessageEvent:public Event
{
public:
   BrowserMessageEvent() : m_info() {
       m_type = INFO_BrowserMessage;
   }
   BrowserMessageEvent(const std::string &info) : m_info(info) {
       m_type = INFO_BrowserMessage;
   }
   const char *GetBrowserMessage() const {return m_info.c_str();}
private:
    std::string m_info;
};

class     HeadlineMessageEvent:public Event
{
public:
   HeadlineMessageEvent() {
      m_type = INFO_HeadlineText;
   }
   HeadlineMessageEvent(const char *info, const char *url)
       : m_info(info), m_url(url) {
       m_type = INFO_HeadlineText;
   }
   const char *GetHeadlineMessage() const  { return m_info.c_str(); }
   const char *GetHeadlineURL() const  { return m_url.c_str(); }
private:
    std::string m_info;
    std::string m_url;
};


class     MediaInfoEvent:public Event
{
public:

    std::vector<Event*>  m_childEvents;
    std::string          m_filename;
    bool             m_filled;
    int32_t          m_totalMilliSeconds; // good for 49 days total time!
    int32_t          m_indexOfSong;
    int32_t          m_totalSongs;
    uint32_t         m_plmID;
    bool             m_copy;

    virtual ~MediaInfoEvent() {
        if (!m_copy) {
            std::vector<Event*>::iterator i = m_childEvents.begin();
            for (; i != m_childEvents.end(); i++) 
                delete *i;
        }
    }

    MediaInfoEvent() : m_childEvents(), m_copy(false) {
        m_type = INFO_MediaInfo;
        m_filled = false;
    }

    MediaInfoEvent(MediaInfoEvent &other)
        :Event(other), m_childEvents(other.m_childEvents), 
         m_filename(other.m_filename), m_copy(true) {
        m_type = other.m_type;
        m_filled = other.m_filled;
        m_totalMilliSeconds = other.m_totalMilliSeconds;
        m_indexOfSong = other.m_indexOfSong;
        m_totalSongs = other.m_totalSongs;
    }
    
   MediaInfoEvent(const char *fn, int32_t tms) 
       : m_filename(fn?fn:""), m_copy(false){
       m_filled = true;
       m_type = INFO_MediaInfo;
       m_totalMilliSeconds = tms;
       m_indexOfSong = 0;
       m_totalSongs = 0;
   }
   void      AddChildEvent(Event * pE) {
       if (pE) 
           m_childEvents.push_back(pE);
   }

   /** Returns the total song time in full hours
    */
   int32_t GetTotalHours()const { return m_totalMilliSeconds/1000/3600; }

   /** Returns the total song minutes.
    *  Note: full song time is hours:minutes:seconds
    */
   int32_t GetTotalMinutes() const
     { return (m_totalMilliSeconds/1000 - GetTotalHours()*3600)/60; }

   /** Returns the total song minutes.
    *  Note: full song time is hours:minutes:seconds
    */
   int32_t GetTotalSeconds() const { 
       return (m_totalMilliSeconds/1000 
               - GetTotalHours()*3600 - GetTotalMinutes()*60); 
   }
};

class     MediaTimeInfoEvent:public Event
{
public:
    int32_t m_currentMilliSeconds;
    int32_t m_currentFrame;
    
    MediaTimeInfoEvent( int32_t ms, int32_t f) {
        m_type = INFO_MediaTimeInfo;
        m_currentMilliSeconds = ms;
        m_currentFrame = f;
    }
    /** Returns the current song time in full hours
     */
    int32_t GetCurrentHours() const { 
       return m_currentMilliSeconds/1000/3600; 
   }

    /** Returns the current song minutes.
     *  Note: full song time is hours:minutes:seconds
     */
    int32_t GetCurrentMinutes() const {
        return (m_currentMilliSeconds/1000 - GetCurrentHours()*3600)/60; 
    }

   /** Returns the current song minutes.
    *  Note: full song time is hours:minutes:seconds
    */
    int32_t GetCurrentSeconds() const {
        return (m_currentMilliSeconds/1000 
                - GetCurrentHours()*3600 - GetCurrentMinutes()*60);
    }
};

class     VolumeEvent:public Event
{
public:
    VolumeEvent(int32_t t, int32_t left = 0, int32_t right = 0) {
        m_type = t;
        m_left = left;
        m_right = right;
    }
    int32_t     GetLeftVolume() const  { return m_left;}
    int32_t     GetRightVolume() const { return m_right;}
private:
   int32_t m_left, m_right;
};

class     ChangePositionEvent:public Event
{
public:
    ChangePositionEvent(int32_t p) {
        m_type = CMD_ChangePosition;
        m_frame = p;
    }
    int32_t     GetPosition() const  {return m_frame;}
private:
   int32_t m_frame;
};

class     MpegInfoEvent:public Event
{
public:
   enum {STEREO = 1, JOINT_STEREO = 2, DUAL = 3, MONO = 4};
   enum {LAYER_1 = 1, LAYER_2, LAYER_3};
   enum {MPEG_1 = 1, MPEG_2 = 2, MPEG_25 = 3};
private:
             int32_t m_totalFrames;
   float     m_secondsPerFrame;
   int32_t     m_bytesPerFrame;
   int32_t     m_bitrate;
   int32_t     m_sampleRate;
   int32_t     m_layer;
   int32_t     m_mpeg;
   int32_t     m_channels;
   int32_t     m_original;
   int32_t     m_copyright;
   int32_t     m_emphasis;
   int32_t     m_stereo;
   int32_t     m_modeExt;
   int32_t     m_crc;
   int32_t     m_wav;
public:
    MpegInfoEvent() {
        m_type = INFO_MPEGInfo;
    }
    MpegInfoEvent(int32_t tf, float spf, int32_t bpf, int32_t br, int32_t sr, int32_t layer, int32_t mpeg, int32_t chans, int32_t orig, int32_t copy, int32_t emph, int32_t stereo, int32_t mode_ext) {
        m_type = INFO_MPEGInfo;
        m_totalFrames = tf;
        m_secondsPerFrame = spf;
        m_bytesPerFrame = bpf;
        m_bitrate = br;
        m_sampleRate = sr;
        m_layer = layer;
        m_mpeg = mpeg;
        m_channels = chans;
        m_original = orig & 0x1;
        m_copyright = copy & 0x1;
        m_emphasis = emph;
        m_stereo = stereo;
        m_modeExt = mode_ext;
        m_wav = 0;
    }
    int32_t     GetTotalFrames() {return m_totalFrames;}
    float       GetSecondsPerFrame() {return m_secondsPerFrame;}
    int32_t     GetBytesPerFrame() {return m_bytesPerFrame;}
    int32_t     GetBitRate() {return m_bitrate;}
    int32_t     GetSampleRate() {return m_sampleRate;}
    int32_t     GetLayer() {return m_layer;}
    int32_t     GetMpegVersion() {return m_mpeg;}
    int32_t     GetChannels() {return m_channels;}
    int32_t     GetOriginal() {return m_original;}
    int32_t     GetCopyright() {return m_copyright;}
    int32_t     GetEmphasis() {return m_emphasis;}
    int32_t     GetStereo() {return m_stereo;}
    int32_t     GetModeExt() {return m_modeExt;}
    int32_t     GetCRC() {return m_crc;}
    int32_t     GetWAV() {return m_wav;}
    void        SetWAV(int wav) {m_wav = wav;}
};

class VorbisInfoEvent:public Event
{
public:
    VorbisInfoEvent(int32_t bitrate, 
                    int32_t channels, int32_t rate,float spf) {
        m_type = INFO_VorbisInfo;
        m_bitrate = bitrate;
        m_channels = channels;
        m_rate = rate; 
        m_secondsPerFrame = spf;
    }
    int32_t     GetBitRate() const { return m_bitrate; }
    int32_t     GetSampleRate() const { return m_rate;}
    int32_t     GetChannels() const {return m_channels;}
    float       GetSecondsPerFrame() const {return m_secondsPerFrame; }
private:
    int32_t     m_bitrate;
    int32_t     m_rate;
    int32_t     m_channels;
    float     m_secondsPerFrame;
};

class     SetEqualizerDataEvent:public Event
{
private:
    float    *m_eq;
    bool      m_enable;
    bool      m_IsEQData;
    float     m_preamp;
    
public:
    
    SetEqualizerDataEvent(bool enable) {
        m_type = CMD_SetEQData;
        m_enable = enable;
        m_IsEQData = false;
    }
    SetEqualizerDataEvent(float *eq, float preamp) {
        m_type = CMD_SetEQData;
        m_eq = eq;
        m_IsEQData = true;
        m_preamp = preamp;
    }
    float    *GetEQData() {return m_eq;}
    bool      IsEQData() {return m_IsEQData;}
    float     GetPreamp() {return m_preamp;}
    bool      GetEnableState() {return m_enable;}
    virtual ~ SetEqualizerDataEvent() {}
};

#define _VISUAL_ENABLE_
#ifdef  _VISUAL_ENABLE_
class     SendVisBufEvent:public Event
{
private:
    int32_t m_length;
    unsigned char *m_buf;
public:
    SendVisBufEvent(int32_t & wrote, void *pBuffer, int32_t length) {
        m_type = CMD_SendVisBuf;
        m_buf = new unsigned char[length];
        m_length = length;
        memcpy(m_buf, pBuffer, length);
        wrote = length;
    }
    unsigned char *GetBuf() {return m_buf;}
    int32_t     GetLength() {return m_length;}
    virtual ~SendVisBufEvent() { delete    m_buf;}
};
#endif // _VISUAL_ENABLE_
#undef  _VISUAL_ENABLE_

class     StreamBufferEvent:public Event
{
public:
    
    StreamBufferEvent(bool bBufferingUp, 
                      int32_t iInputPercent, int iOutputPercent) {
        m_type = INFO_BufferStatus;
        m_bBufferingUp = bBufferingUp;
        m_iInputPercent = iInputPercent;
        m_iOutputPercent = iOutputPercent;
    }

    bool      IsBufferingUp()     { return m_bBufferingUp;}
    int32_t     GetInputPercent() { return m_iInputPercent;}
    int32_t     GetOutputPercent(){ return m_iOutputPercent;}
private:
    bool      m_bBufferingUp;
    int32_t     m_iInputPercent, m_iOutputPercent;
};

class     StreamInfoEvent:public Event
{
public:

   char     *m_streamURL;
   char     *m_streamTitle;

             StreamInfoEvent(char *szTitle, char *szURL)
   {
      m_type = INFO_StreamInfo;
      if (szURL)
          m_streamURL = strdup_new(szURL);
      else
          m_streamURL = strdup_new("");
      if (szTitle)
          m_streamTitle = strdup_new(szTitle);
      else
          m_streamTitle = strdup_new("");
   };

   virtual ~ StreamInfoEvent()
   {
      delete [] m_streamURL;
      delete [] m_streamTitle;
   };

   void      GetURL(char *szUrl, int iSize)
   {
      strncpy(szUrl, m_streamURL, iSize - 1);
      szUrl[iSize - 1] = 0;
   }
   void      GetTitle(char *szTitle, int iSize)
   {
      strncpy(szTitle, m_streamTitle, iSize - 1);
      szTitle[iSize - 1] = 0;
   }
};

class CDInfoEvent : public Event {
private:
    uint32_t m_totalTracks;
    uint32_t m_cddb;
    std::string m_cdindex;
public:
    CDInfoEvent(const uint32_t numtracks, const uint32_t cddb, const char *cdindex)
    { m_type = INFO_CDDiscStatus; m_totalTracks = numtracks; 
      m_cddb = cddb; m_cdindex = cdindex; }
    virtual ~CDInfoEvent() {}

    const uint32_t GetNumTracks() const { return m_totalTracks; }
    const uint32_t GetCDDB() const { return m_cddb; }
    const std::string GetCdindex() const { return m_cdindex; }
};

class MusicCatalogStreamAddedEvent : public Event {
private:
    const PlaylistItem *m_item;
public:
    MusicCatalogStreamAddedEvent(const PlaylistItem *item)
    { m_type = INFO_MusicCatalogStreamAdded; m_item = item; }
    virtual ~MusicCatalogStreamAddedEvent() {}

    const PlaylistItem *Item() const { return m_item; }
};

class MusicCatalogStreamRemovedEvent : public Event {
private:
    const PlaylistItem* m_item;
public:
    MusicCatalogStreamRemovedEvent(const PlaylistItem *item)
    { m_type = INFO_MusicCatalogStreamRemoved; m_item = item; }
    virtual ~MusicCatalogStreamRemovedEvent() {}

    const PlaylistItem *Item() const { return m_item; }
};

class ArtistList;
class AlbumList;

class MusicCatalogTrackAddedEvent : public Event {
private:
    const PlaylistItem *m_item;
    const ArtistList *m_artist;
    const AlbumList *m_album;
public:
    MusicCatalogTrackAddedEvent(const PlaylistItem *item, 
                                const ArtistList *artist,
                                const AlbumList *album)
    { m_type = INFO_MusicCatalogTrackAdded; m_item = item; 
      m_artist = artist; m_album = album; }
    virtual ~MusicCatalogTrackAddedEvent() {}

    const PlaylistItem *Item() const { return m_item; }
    const ArtistList *Artist() const { return m_artist; }
    const AlbumList *Album() const { return m_album; }
};

class MusicCatalogTrackRemovedEvent : public Event {
private:
    const PlaylistItem* m_item;
    const ArtistList *m_artist;
    const AlbumList *m_album;
public:
    MusicCatalogTrackRemovedEvent(const PlaylistItem *item,
                                  const ArtistList *artist,
                                  const AlbumList *album)
    { m_type = INFO_MusicCatalogTrackRemoved; m_item = item;
      m_artist = artist; m_album = album; }
    virtual ~MusicCatalogTrackRemovedEvent() {}

    const PlaylistItem *Item() const { return m_item; }
    const ArtistList *Artist() const { return m_artist; }
    const AlbumList *Album() const { return m_album; }
};

class MusicCatalogTrackChangedEvent : public Event {
private:
    const PlaylistItem *m_newItem;
    const PlaylistItem *m_oldItem; 
    const ArtistList *m_newArtist;
    const ArtistList *m_oldArtist;
    const AlbumList *m_newAlbum;
    const AlbumList *m_oldAlbum;
public:
    MusicCatalogTrackChangedEvent(const PlaylistItem *oldItem,
                                  const PlaylistItem *newItem,
                                  const ArtistList *oldArtist,
                                  const ArtistList *newArtist,
                                  const AlbumList *oldAlbum,
                                  const AlbumList *newAlbum)
    { m_type = INFO_MusicCatalogTrackChanged; m_oldItem = oldItem; 
      m_newItem = newItem; m_oldArtist = oldArtist; m_newArtist = newArtist; 
      m_oldAlbum = oldAlbum; m_newAlbum = newAlbum; }
    virtual ~MusicCatalogTrackChangedEvent() {}

    const PlaylistItem *NewItem() const { return m_newItem; }
    const PlaylistItem *OldItem() const { return m_oldItem; }
    const ArtistList *NewArtist() const { return m_newArtist; }
    const ArtistList *OldArtist() const { return m_oldArtist; }
    const AlbumList *NewAlbum() const { return m_newAlbum; }
    const AlbumList *OldAlbum() const { return m_oldAlbum; }
};

class MusicCatalogPlaylistAddedEvent : public Event {
private:
    std::string m_item;
public:
    MusicCatalogPlaylistAddedEvent(std::string &item)
    { m_type = INFO_MusicCatalogPlaylistAdded; m_item = item; }
    virtual ~MusicCatalogPlaylistAddedEvent() {}

    const std::string Item() const { return m_item; }
};

class MusicCatalogPlaylistRemovedEvent : public Event {
private:
    std::string m_item;
public:
    MusicCatalogPlaylistRemovedEvent(std::string & item)
    { m_type = INFO_MusicCatalogPlaylistRemoved; m_item = item; }
    virtual ~MusicCatalogPlaylistRemovedEvent() {}

    const std::string Item() const { return m_item; }
};

class PlaylistItemAddedEvent : public Event {
  private:
    const PlaylistItem* m_item;
    const PlaylistManager* m_plm;
public:
	PlaylistItemAddedEvent(const PlaylistItem* item, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemAdded; m_item = item; m_plm = plm;}
	virtual ~PlaylistItemAddedEvent() {}

	const PlaylistItem* Item() const { return m_item; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistItemsAddedEvent : public Event {
  private:
    std::vector<PlaylistItem*> m_items;
    const PlaylistManager* m_plm;
public:
	PlaylistItemsAddedEvent(std::vector<PlaylistItem*>* items, 
				const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemsAdded; m_items = *items; m_plm = plm;}
	virtual ~PlaylistItemsAddedEvent() {}

	const std::vector<PlaylistItem*>* Items() const { return &m_items; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistItemMovedEvent : public Event {
private:
	const PlaylistItem* m_item;
    const PlaylistManager* m_plm;
    uint32_t m_oldIndex, m_newIndex;
public:
	PlaylistItemMovedEvent(uint32_t oldIndex, 
                           uint32_t newIndex, 
                           const PlaylistItem* item,
                           const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemMoved; m_item = item; m_plm = plm;
      m_oldIndex = oldIndex; m_newIndex = newIndex;}
	virtual ~PlaylistItemMovedEvent() {}

	const PlaylistItem* Item() const { return m_item; }
    const PlaylistManager* Manager() const { return m_plm; }
    uint32_t OldIndex() const { return m_oldIndex; }
    uint32_t NewIndex() const { return m_newIndex; }
};

class PlaylistItemRemovedEvent : public Event {
private:
	std::vector<PlaylistItem*> m_items;
    const PlaylistManager* m_plm;
    std::vector<uint32_t> m_indices;
public:
	PlaylistItemRemovedEvent(std::vector<PlaylistItem*>* items, 
                             std::vector<uint32_t>* indices, 
                             const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemRemoved; m_items = *items; m_plm = plm; m_indices = *indices; }
	virtual ~PlaylistItemRemovedEvent() {}

	const std::vector<PlaylistItem*>* Items() const { return &m_items; }
    const std::vector<uint32_t>* Indices() const { return &m_indices; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistItemsUpdatedEvent : public Event {
  private:
    std::vector<PlaylistItem*> m_items;
    const PlaylistManager* m_plm;
public:
	PlaylistItemsUpdatedEvent(std::vector<PlaylistItem*>* items, 
				  const PlaylistManager* plm) 
    { m_type = INFO_PlaylistItemsUpdated; m_items = *items; m_plm = plm;}
	virtual ~PlaylistItemsUpdatedEvent() {}

	const std::vector<PlaylistItem*>* Items() const { return &m_items; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistCurrentItemInfoEvent : public Event {
private:
	const PlaylistItem* m_item;
    const PlaylistManager* m_plm;
public:
	PlaylistCurrentItemInfoEvent(const PlaylistItem* item, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistCurrentItemInfo; m_item = item; m_plm = plm;}
	virtual ~PlaylistCurrentItemInfoEvent() {}

	const PlaylistItem* Item() const { return m_item; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistSortedEvent : public Event {
private:
	PlaylistSortKey m_key;
    const PlaylistManager* m_plm;
public:
	PlaylistSortedEvent(PlaylistSortKey key, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistSorted; m_key = key; m_plm = plm;}
	virtual ~PlaylistSortedEvent() {}

	PlaylistSortKey GetSortKey() const { return m_key; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistRepeatEvent : public Event {
private:
	RepeatMode m_rm;
    const PlaylistManager* m_plm;
public:
	PlaylistRepeatEvent(RepeatMode rm, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistRepeat; m_rm = rm; m_plm = plm;}
	virtual ~PlaylistRepeatEvent() {}

	RepeatMode GetRepeatMode() const { return m_rm; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class PlaylistShuffleEvent : public Event {
private:
	bool m_sm;
    const PlaylistManager* m_plm;
public:
	PlaylistShuffleEvent(bool sm, const PlaylistManager* plm) 
    { m_type = INFO_PlaylistShuffle; m_sm = sm; m_plm = plm;}
	virtual ~PlaylistShuffleEvent() {}

	bool GetShuffleMode() const { return m_sm; }
    const PlaylistManager* Manager() const { return m_plm; }
};

class DownloadItem;

class DownloadItemAddedEvent : public Event {
public:
    DownloadItemAddedEvent(DownloadItem* item) 
    { m_type = INFO_DownloadItemAdded; m_item = item; }
	virtual ~DownloadItemAddedEvent() {}

	DownloadItem* Item() { return m_item; }
private:
    DownloadItem* m_item;
};

class DownloadItemRemovedEvent : public Event {
public:
    DownloadItemRemovedEvent(DownloadItem* item) 
    { m_type = INFO_DownloadItemRemoved; m_item = item; }
	virtual ~DownloadItemRemovedEvent() {}

	DownloadItem* Item() { return m_item; }
private:
    DownloadItem* m_item;
};

class DownloadItemNewStateEvent : public Event {
public:
    DownloadItemNewStateEvent(DownloadItem* item) 
    { m_type = INFO_DownloadItemNewState; m_item = item; }
	virtual ~DownloadItemNewStateEvent() {}

	DownloadItem* Item() { return m_item; }
private:
    DownloadItem* m_item;
};

class DownloadItemProgressEvent : public Event {
public:
    DownloadItemProgressEvent(DownloadItem* item) 
    { m_type = INFO_DownloadItemProgress; m_item = item; }
	virtual ~DownloadItemProgressEvent() {}

	DownloadItem* Item() { return m_item; }
private:
    DownloadItem* m_item;
};

class LoadThemeEvent : public Event {
public:
    LoadThemeEvent(const char *url, const char *saved) 
        : m_url(url), m_saved(saved) { 
        m_type = CMD_LoadTheme; 
    }

   const char *URL() const { return m_url.c_str(); };
   const char *SavedTheme() const { return m_saved.c_str(); };

private:

    std::string m_url;
    std::string m_saved;
};

class ShowPreferencesEvent:public Event
{
private:
    int32_t m_page;
public:
    ShowPreferencesEvent(int32_t page = 0){
        m_type = CMD_ShowPreferences; 
        m_page = page; 
    }
    int32_t GetDefaultPage() const{ return m_page; }
};

class MissingFileEvent:public Event
{
private:
    PlaylistItem *m_item;
public:
    MissingFileEvent(PlaylistItem *item)
        { m_type = INFO_FileNotFound; m_item = item; }
    virtual ~ MissingFileEvent(){ }
    PlaylistItem *Item() const{ return m_item; }
};

class CDNotFoundEvent:public Event
{
private:
    std::string m_submitUrl;
public:
    CDNotFoundEvent(const std::string &url)
        { m_type = INFO_CDNotFound; m_submitUrl = url; }
    const std::string &URL() const { return m_submitUrl; }
};

#endif /* _EVENTDATA_H_ */

/* arch-tag: e13045ed-7cc2-42c1-baa8-75dde2c5dfa7
   (do not change this comment) */

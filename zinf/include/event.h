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
	
	$Id: event.h,v 1.7 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_EVENT_H_
#define INCLUDED_EVENT_H_

#include <stdint.h>
#include "config.h"
#include "errors.h"

class Event {
public:
             Event(int32_t c) { m_type = c; };
    virtual ~Event() {}
    int32_t Type() { return m_type; }
protected:
    Event() {}
    int32_t m_type;
};

class EventQueue {
public:
    virtual Error AcceptEvent(Event *) = 0;
};


#define CMD_Stop                1  // stop current play  (no arg)
#define CMD_Play                2  // Play current media piece from beginning (no arg)
#define CMD_NextMediaPiece      3  // set pointer in playlist to next mediapiece (no arg)
#define CMD_PrevMediaPiece      4  // set pointer in playlist to prev mediapiece (no arg)
#define CMD_Pause               5  // pause current play (no arg)
#define CMD_PlayPaused			6  // play, but start in the 'paused' mode
#define CMD_QuitPlayer          7  // tells player to quit (no arg)
#define CMD_Cleanup             8  // player tells modules to cleanup and get ready for termination (no arg)
//#define CMD_Terminate           9  // is sent by player to COOs, and the DummyCOO makes the main thread delete player and exit (no arg)
#define INFO_ReadyToDieUI      10 // sent by CIO to player notifying it that their cleanup has completed. (if arg !null, it is a ptr to this module, it wants to be deleted)
#define INFO_PlaylistDonePlay   11 // sent by player when the playlist has been exhausted (no arg)
#define INFO_DoneOutputting     12 // sent by PMO (or LMC controlling PMO) to Player when its done outputing (i.e. playing is done)
#define CMD_KillEventThread     13 // sent by player to various modules so they kill their event threads before deletion (no arg)
#define CMD_StartRead           14 // sent by player to PMI to start/continue reading bytes from source to its output buffer (no arg)
#define CMD_StopRead            15 // sent by player to PMI to stop reading bytes from source to its output buffer (no arg)
#define CMD_ResetRead           16 // sent by player to PMI to set read pointer back to beginning (no arg)
#define CMD_SeekRead            17 // sent to PMI to tell it to reset read stream to a particular point (arg = that point)
#define INFO_MediaInfo          18 // info about media title, length, etc... (arg = MediaVitalInfo)
#define INFO_MediaTimeInfo      19 // info about current time during play, etc (arg = MediaTimePositionInfo)
#define INFO_Stopped            20 // sent by player to COOs when it stops playing the current song (no arg)
#define INFO_Paused             21 // sent by player to COOs when it pauses playing the current song (no arg)
#define CMD_UnPause             22 // sent by CIO to player to tell it to unpause current song.  Note: a play causes the current song to be played from beginning (no arg)
#define CMD_TogglePause         24 // sent by CIO to player to change the boolean state Paused (no arg)
#define INFO_Playing            26 // sent by player to COO to say that it is beginning to play a song
#define CMD_ChangePosition      27 // sent by CIO to player to change the position in the song to decode from (arg = frame position)
#define INFO_ErrorMessage       28 // sent to UIs to signal an error condition in the decoder
#define INFO_MPEGInfo			29 // info about MPEG stuff (layer, samprate, etc.)
#define INFO_ID3TagInfo			30 // info about ID3 tag
#define CMD_PLMGetMediaInfo             31 // sent by PlaylistManager to player to get vital info about a particular playlistitem
#define CMD_PLMSetMediaInfo             32 // sent by Player to PlaylistManger to set the info the PLM requested.
#define INFO_PlaylistRepeat     33 // sent by PLM to player to inform of a setting of repeat mode
#define INFO_PlaylistShuffle    34 // sent by PLM to player to inform of a setting of shuffle mode
#define INFO_PlaylistUpdated	35 // sent by PLM to player to inform of a playlist update
#define INFO_PlaylistItemsUpdated 36 // sent by PLM to player to inform of a playlistitem update
#define INFO_UserMessage        37 // sent by UI to Player, diseminated by Player to each UI
#define _EQUALIZER_ENABLE_
#ifdef  _EQUALIZER_ENABLE_
#define CMD_SetEQData                   38      //set EQ data
#endif  //_EQUALIZER_ENABLE_
#undef  _EQUALIZER_ENABLE_

#define _VISUAL_ENABLE_
#ifdef  _VISUAL_ENABLE_
#define CMD_SendVisBuf                  39      //Send PCM buffer from LMC to player
#endif  //_VISUAL_ENABLE_
#undef  _VISUAL_ENABLE_

#define CMD_GetVolume           40
#define CMD_SetVolume           41

#define INFO_BufferingBegin       42 // Network rebuffering just started
#define INFO_BufferingEnd         43 // Network rebuffering just stopped
#define INFO_BufferStatus         44 // Buffer percent full information

#define CMD_PLMGetMediaTitle      45 // sent by PlaylistManager to player to get title about a particular playlistitem
#define INFO_StatusMessage      46 // sent to UIs so they can display status info

#define INFO_VolumeInfo         47
#define INFO_StreamInfo         48


#define INFO_SearchMusicDone    49 // sent by the music search thread on completion
#define INFO_BrowserMessage     50 // status updates sent to the music browser

#define INFO_DownloadItemAdded    51  // sent by DownloadManager to UIs by way of Player so the UIs know that an item has been added to the download manager
#define INFO_DownloadItemRemoved  52  // sent by DownloadManager to UIs by way of Player so the UIs know that an item has been removed from the download manager
#define INFO_DownloadItemNewState 53  // sent by DownloadManager to UIs by way of Player so the UIs know that an item in the download manager has changed state
#define INFO_DownloadItemProgress 54  // sent by DownloadManager to UIs by way of Player so the UIs know that progress has been made on a download

#define CMD_ToggleDownloadUI     55 // sent to the DownloadManager UI via Player to toggle its visibility
#define CMD_TogglePlaylistUI     56 // sent to the MusicBrowserUI via Player to popup/disappear in Playlist Manager ui mode
#define CMD_ToggleMusicBrowserUI 57 // sent to the MusicBrowserUI via the Player to popup/disappear in MusicBrowser mode

#define INFO_PlaylistItemAdded    58 // sent by PLM to player to inform of a playlistitem update
#define INFO_PlaylistItemRemoved    59 // sent by PLM to player to inform of a playlistitem update
#define INFO_PlaylistCurrentItemInfo 60 // sent by PLM to player to inform of the current items info
#define INFO_PlaylistItemMoved 61 // sent by PLM to player to inform of items moving
#define INFO_PlaylistSorted         62 // sent by PLM to player to inform of items sorting
#define INFO_PrefsChanged           63 // send by a preferences window whenever the preferences changed
#define CMD_ShowPreferences         64 // send by some module to have the theme ui show the prefences dialog
#define CMD_AddFiles                65 // sent by the theme ui to have the music browser show an add tracks dialog for quickly adding/playing tracks
#define CMD_QueryPlayerState      66 // asks the player object to resend out its state
#define CMD_LoadTheme             67 // load a theme from a file

#define INFO_MusicCatalogTrackAdded 68 // sent by MC to the player to inform of a track added
#define INFO_MusicCatalogTrackRemoved 69 // sent by MC to the player to inform of a track removal
#define INFO_MusicCatalogPlaylistAdded 70 // sent by MC to the player after a playlist was added
#define INFO_MusicCatalogPlaylistRemoved 71 // sent by the MC to the player after a playlist removal
#define INFO_MusicCatalogTrackChanged 72 // sent by the MC to the player after a track was changed
#define INFO_MusicCatalogCleared    73 // sent by the MC whenever it clears its tree
#define INFO_MusicCatalogStreamAdded  74 // sent by the MC when a stream is added
#define INFO_MusicCatalogStreamRemoved 75 // sent by the MC when a stream is removed

#define INFO_PlaylistItemsAdded 76 // sent by PLM to player to inform of a playlistitem update
#define INFO_DoneOutputtingDueToError 77 // sent by PMO (or LMC controlling PMO) to Player when its done outputing but an error occurred
#define INFO_HeadlineText             78 // Used to send Slashdot headlines to UIs
#define INFO_CDDiscStatus             79 // sent by the CD PMO when a disc starts playing, so the MB gui knows the number of tracks, etc
#define INFO_MusicCatalogRegenerating 80 // sent by the MC to tell the UIs to ignore it for awhile
#define INFO_MusicCatalogDoneRegenerating 81 // sent by the MC to tell the UIs to regenerate their database
#define INFO_VorbisInfo             82 // Used to send vorbis stream info
#define INFO_FileNotFound           83 // Used to send file not found info
#define CMD_EditCurrentPlaylistItemInfo  84 // Used to instruct the info editor to edit the current metadata item
#define INFO_CDNotFound              93 // sent by mbcd plugin to musicbrowser 
#define INFO_DatabaseUpgraded        94 // sent by the MB to UIs when it wipes the database.
#define INFO_DoneOutputtingDueToOutputError 95 //Sent by PMO an error occurs
#endif // _EVENT_H_

/* arch-tag: 1da7a1f6-5994-435c-a18d-bae7b07e6d1d
   (do not change this comment) */

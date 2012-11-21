/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file zinf_player.cpp
 *
 * Implements the czinf_player class, a concrete subclass of cplayer for zinf.
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA
 */



#include "zinf_player.h"
#include "../player.h"
#include "../server.h"
#include "../client.h"
#include "../trace.h"
#include "../settings.h"

#include "include/event.h"
#include "include/eventdata.h"
#include "include/facontext.h"
#include "include/playlist.h"
#include "include/thread.h"
#include "include/ui.h"

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
using namespace std;


czinf_player::czinf_player() :
  facontext(0), pos_ms(0), ms_per_frame(0), length_ms(0), length_frames(0),
  song(0), state(&kstopped()) {

  ctrace::instance() << acquire("zinf") << "ctor!" << release();
}


// p4sync factory method
cplayer &cplayer::instance()
{
  static czinf_player player;
  return player;
}


// p4sync player methods
const vector<string> czinf_player::get_playlist()
{
  ctrace::instance() << acquire("zinf") << "get_playlist" << release();

  vector<string> playlist(facontext->plm->begin(), facontext->plm->end());
  return playlist;
}

void czinf_player::set_playlist(const vector<string> &playlist)
{
  ctrace::instance() << acquire("zinf") << "set_playlist" << release();

  facontext->plm->clear();
  copy(playlist.begin(), playlist.end(), facontext->plm->begin());
}


int czinf_player::get_cur_song()
{
  ctrace::instance() << acquire("zinf") << "get_cur_song: " << song
                     << release();

  return facontext->plm->SetCurrentIndex(song);
}


void czinf_player::set_cur_song(int song)
{
  ctrace::instance() << acquire("zinf") << "set_cur_song: " << song
                     << release();

  facontext->plm->SetCurrentIndex(song);
}


int czinf_player::get_pos_ms()
{
  ctrace::instance() << acquire("zinf") << "get_pos_ms: " << pos_ms
                     << release();
  return pos_ms;
}


void czinf_player::set_pos_ms(int pos)
{
  ctrace::instance() << acquire("zinf") << "set_pos_ms: " << pos << release();

  int frame = static_cast<int>(pos / ms_per_frame);
  facontext->target->AcceptEvent(new ChangePositionEvent(frame));

  pos_ms = pos;
}


void czinf_player::add_timer(int freq_ms, bool (*callback)())
{
}


void czinf_player::play()
{
  ctrace::instance() << acquire("zinf") << "play" << release();

  facontext->target->AcceptEvent(new Event(CMD_Play));
}


void czinf_player::stop()
{
  ctrace::instance() << acquire("zinf") << "stop" << release();

  facontext->target->AcceptEvent(new Event(CMD_Stop));
}


const string &czinf_player::get_state()
{
  ctrace::instance() << acquire("zinf") << "get_state " << *state << release();
  return *state;
}


// zinf plugin methods
extern "C" {
  UserInterface *Initialize(FAContext * context)
  {
    ctrace::instance() << acquire("zinf") << "Initialize" << release();

    czinf_player zinf = reinterpret_cast<czinf_player &>(cplayer::instance());
    zinf.set_facontext(context);
    return &zinf;
  }
}

Error czinf_player::Init(int32_t startup_level) {
  ctrace::instance() << acquire("zinf") << "init" << release();
  this->plugin_enabled();

  return kError_NoErr;
}


Error czinf_player::AcceptEvent(Event *event) {
  ctrace::instance() << acquire("zinf") << "AcceptEvent, event type ";
  if (event)
    ctrace::instance() << event->Type();
  else
    ctrace::instance() << "null";
  ctrace::instance() << release();

  if (!event)
    return kError_NoErr;

  switch (event->Type()) {
  case INFO_MediaInfo: {
    MediaInfoEvent *media = reinterpret_cast<MediaInfoEvent *>(event);
    song = media->m_indexOfSong;
    length_ms = media->m_totalMilliSeconds;
    break;
  }

  case INFO_MediaTimeInfo: {
    MediaTimeInfoEvent *time = reinterpret_cast<MediaTimeInfoEvent *>(event);
    pos_ms = time->m_currentMilliSeconds;
    break;
  }

  case INFO_MPEGInfo: {
    MpegInfoEvent *mpeg = reinterpret_cast<MpegInfoEvent *>(event);
    ms_per_frame = mpeg->GetSecondsPerFrame() * 1000;
    length_frames = mpeg->GetTotalFrames();
    break;
  }

  case INFO_VorbisInfo: {
    VorbisInfoEvent *vorbis = reinterpret_cast<VorbisInfoEvent *>(event);
    ms_per_frame = vorbis->GetSecondsPerFrame() * 1000;
    length_frames = static_cast<int>(length_ms / ms_per_frame);
    break;
  }

  case INFO_Playing: {
    state = &kplaying();
    break;
  }

  case INFO_Paused: {
    state = &kpaused();
    break;
  }

  case INFO_Stopped: {
    state = &kstopped();
    break;
  }

  default:
    ctrace::instance() << acquire("zinf") << "unknown event type" << release();
  }

  return kError_NoErr;
}

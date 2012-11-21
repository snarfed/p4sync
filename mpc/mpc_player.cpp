/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file mpc_player.cpp
 *
 * Implements the cmpc_player class, a concrete subclass of cplayer for mpc.
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



#include "mpc_player.h"
#include "../player.h"
#include "../server.h"
#include "../client.h"
#include "../trace.h"
#include "../settings.h"
#include "libmpdclient.h"

#include <pthread.h>
#include <sys/select.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
using namespace std;

extern "C" mpd_Connection *setup_connection();   // defined in main.c

mpd_Connection *gconn;

void connect() {
  gconn = setup_connection();
}

void disconnect() {
  mpd_closeConnection(gconn);
}



// called by main.c
extern "C" void mpc_p4sync_startup()
{
  ctrace::instance() << acquire("mpc") << "startup" << release();
  cplayer::instance().plugin_enabled();
}

extern "C" void mpc_p4sync_shutdown()
{
  ctrace::instance() << acquire("mpc") << "shutdown" << release();
  cplayer::instance().plugin_disabled();
}


// p4sync factory method
cplayer &cplayer::instance()
{
  static cmpc_player player;
  return player;
}


// p4sync player methods
const vector<string> cmpc_player::get_playlist()
{
  ctrace::instance() << acquire("mpc") << "get_playlist" << release();
  connect();

  vector<string> playlist;
  mpd_sendPlaylistInfoCommand(gconn, -1);

  while (mpd_InfoEntity *entity = mpd_getNextInfoEntity(gconn)) {
    if (entity->type == MPD_INFO_ENTITY_TYPE_PLAYLISTFILE)
      playlist.push_back(entity->info.playlistFile->path);
    mpd_freeInfoEntity(entity);
  }

  disconnect();
  return playlist;
}

void cmpc_player::set_playlist(const vector<string> &playlist)
{
  ctrace::instance() << acquire("mpc") << "set_playlist" << release();
  connect();

  mpd_sendClearCommand(gconn);

  for (vector<string>::const_iterator it = playlist.begin();
       it != playlist.end(); it++)
    mpd_sendAddCommand(gconn, it->c_str());

  disconnect();
}


int cmpc_player::get_cur_song()
{
  ctrace::instance() << acquire("mpc") << "get_cur_song " << release();
  connect();

  mpd_sendStatusCommand(gconn);
  mpd_Status *status = mpd_getStatus(gconn);
  int song = status->song;
  mpd_freeStatus(status);

  disconnect();
  return song;
}


void cmpc_player::set_cur_song(int song)
{
  ctrace::instance() << acquire("mpc") << "set_cur_song: " << song
                     << release();
  connect();

  mpd_sendPlayCommand(gconn, song);

  disconnect();
}


int cmpc_player::get_pos_ms()
{
  ctrace::instance() << acquire("mpc") << "get_pos_ms: " << release();
  connect();

  mpd_sendStatusCommand(gconn);
  mpd_Status *status = mpd_getStatus(gconn);
  int pos_ms = static_cast<int>(status->elapsedTime * 1000);
  mpd_freeStatus(status);

  disconnect();
  return pos_ms;
}


void cmpc_player::set_pos_ms(int pos)
{
  ctrace::instance() << acquire("mpc") << "set_pos_ms: " << pos << release();
  connect();

  float pos_secs = static_cast<float>(pos) / 1000;
  mpd_sendSeekCommand(gconn, 0 /* current song */, pos_secs);

  disconnect();
}


typedef struct {
  pthread_t *thread;
  bool (*callback)();
  int freq_ms;
} timer_data;

void *timer(void *data)
{
  timer_data *td = reinterpret_cast<timer_data *>(data);

  struct timeval timeout = { 0, td->freq_ms * 1000 };
  fd_set empty;
  FD_ZERO(&empty);

  while (true) {
    // wait until the timeout; use select to get sub-second precision
    struct timeval slept = timeout;
    while (slept.tv_usec > 0)
      select(0, &empty, &empty, &empty, &slept);

    // call the callback
    if (!td->callback())
      return 0;
  }

  delete td->thread;
  delete td;
}


void cmpc_player::add_timer(int freq_ms, bool (*callback)())
{
  timer_data *td = new timer_data();  // freed inside timer()
  assert(td);
  td->thread = new pthread_t();       // freed inside timer()
  td->callback = callback;
  td->freq_ms = freq_ms;

  int ret = pthread_create(td->thread, 0, &timer, td);
  assert(ret >= 0);
}


void cmpc_player::play()
{
  ctrace::instance() << acquire("mpc") << "play" << release();
  connect();

  mpd_sendPlayCommand(gconn, -1 /* current song */);

  disconnect();
}


void cmpc_player::stop()
{
  ctrace::instance() << acquire("mpc") << "stop" << release();
  connect();

  mpd_sendStopCommand(gconn);

  disconnect();
}


const string &cmpc_player::get_state()
{
  ctrace::instance() << acquire("mpc") << "get_state " << release();
  connect();

  mpd_sendStatusCommand(gconn);
  mpd_Status *status = mpd_getStatus(gconn);
  int state = status->state;
  mpd_freeStatus(status);

  switch (state) {
  case MPD_STATUS_STATE_PLAY:
    disconnect();
    return cplayer::kplaying();

  case MPD_STATUS_STATE_PAUSE:
    disconnect();
    return cplayer::kpaused();

  case MPD_STATUS_STATE_STOP:
  default:
    disconnect();
    return cplayer::kstopped();
  }
}

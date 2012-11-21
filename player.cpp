/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file player.cpp
 *
 * Defines generic player plugin code - connecting or starting the server on
 * init, disconnecting or stopping the server on teardown, setting auto-sync
 * timeouts, etc.
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

#include "player.h"
#include "client.h"
#include "server.h"
#include "settings.h"
#include "trace.h"

#include <cassert>

// callbacks
bool server_callback();
bool sync_callback();
bool play_callback();


void cplayer::plugin_enabled()
{
  assert(!enabled);

  ctrace::instance() << acquire("player") << "plugin_enabled" << release();
  enabled = true;

  if (csettings::instance().get<bool>("run_server")) {
    server_init(csettings::instance().get<int>("server_port"));
    add_timer(csettings::instance().get<int>("serve_freq_ms"),
              server_callback);

  } else {
    add_timer(100, play_callback);

    int ret = client_connect(csettings::instance().get<string>("sync_to_host"),
                             csettings::instance().get<int>("sync_to_port"));

    // only start the re-synching timer if we're connected
    if (ret == 0) {
      int sync_freq = csettings::instance().get<int>("sync_freq_ms");
      if (sync_freq > 0)
        add_timer(sync_freq, sync_callback);
    }
  }
}


void cplayer::plugin_disabled()
{
  assert(enabled);

  ctrace::instance() << acquire("player") << "plugin_disabled" << release();
  enabled = false;

  if (csettings::instance().get<bool>("run_server"))
    server_shutdown();
  else
    client_disconnect();

  csettings::instance().write();
}


// if the plugin is enabled and this player is the server, this callback is
// called every server_freq_ms
bool server_callback()
{
  if (!cplayer::instance().is_plugin_enabled() ||
      !csettings::instance().get<bool>("run_server"))
    return false;

  serve();
  return true;
}

// if the plugin is enabled and this player is a client, this callback is
// called every sync_freq_ms
bool sync_callback()
{
  if (!cplayer::instance().is_plugin_enabled() ||
      csettings::instance().get<bool>("run_server"))
    return false;

  sync_player();
  return true;
}

// if the plugin is enabled and this player is a client, this callback is
// called every 100ms or so. it checks to see whether the play button was
// pressed. if it was, it tries to connect and sync.
bool play_callback()
{
  static string last_state = cplayer::kplaying();
  static int last_song = -1;
  static int last_pos = -1;

  if (!cplayer::instance().is_plugin_enabled() ||
      csettings::instance().get<bool>("run_server"))
    return false;

  // detect whether they pressed play. if the player was previously stopped or
  // paused, this is easy; we just compare the current and last states. if it
  // was playing, though, we have to check whether it's on the same song, but
  // it restarted from the beginning.
  const string &cur_state = cplayer::instance().get_state();
  const int &cur_song = cplayer::instance().get_cur_song();
  const int &cur_pos = cplayer::instance().get_pos_ms();

  if (cur_state == cplayer::kplaying()) {
    bool restarted = (last_song == cur_song &&
                      cur_pos < last_pos && cur_pos <= 1000);

    if (restarted || last_state != cplayer::kplaying()) {
      ctrace::instance() << acquire("player") << "pressed play!" << release();
      client_connect();
      sync_player();
    }
  }

  last_state = cur_state;
  last_song = cur_song;
  last_pos = cur_pos;
  return true;
}

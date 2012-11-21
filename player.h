/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file player.h
 *
 * Defines a abstract class that represents an MP3 player. Plugins for each
 * player (Winamp, WMP, XMMs) should provide concrete subclasses of this class
 * that implement the abstract methods.
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

#ifndef _PLAYER_H
#define _PLAYER_H

#include <string>
#include <vector>
using namespace std;

class cplayer {
public:

  /** instance
   *
   * Factory method. Each player implementation should define this method to
   * return an instance of itself, properly initialized.
   */
  static cplayer &instance();


  /** plugin_enabled, plugin_disabled, is_plugin_enabled
   *
   * Call the plugin_*() methods when your plugin is enabled and disabled. They
   * start/stop the server (if you're a server) and connect/disconnect from
   * another server (if you're a client). They also set up and tear down timer
   * functions for synching, serving, etc.
   *
   * is_plugin_enabled() is just an accessor.
   */
  void plugin_enabled();
  void plugin_disabled();
  bool is_plugin_enabled() { return enabled; }

  /** get_playlist, set_playlist
   *
   * Accessors for the current playlist, as a vector of filenames, with paths.
   */
  virtual const vector<string> get_playlist() = 0;
  virtual void set_playlist(const vector<string> &playlist) = 0;


  /** get_cur_song, set_cur_song
   *
   * Accessors for the currently playing song, as an index into the playlist.
   * The index is 0-based.
   */
  virtual int get_cur_song() = 0;
  virtual void set_cur_song(int cur) = 0;


  /** get_pos_ms, set_pos_ms
   *
   * Accessors for the current position in the song, in milliseconds.
   */
  virtual int get_pos_ms() = 0;
  virtual void set_pos_ms(int ms) = 0;


  /** add_timer
   *
   * Adds a timer, a callback that will be called at a regular interval. The
   * callback should continue to be called until it returns false.
   */
  virtual void add_timer(int freq_ms, bool (*callback)()) = 0;


  /** play, stop, get_state
   *
   * Playback controls and accessors.
   */
  virtual void play() = 0;
  virtual void stop() = 0;
  virtual const string &get_state() = 0;

  // constants, for use with get_state
  const static string &kplaying() { static string val("playing"); return val; }
  const static string &kpaused()  { static string val("paused"); return val; }
  const static string &kstopped() { static string val("stopped"); return val; }

 protected:
  cplayer() : enabled(false) {}

  bool enabled;
};


#endif // _PLAYER_H

/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file mock_player.h
 *
 * Defines a mock MP3 player that implements the abstract cplayer class. Used
 * for unit testing.
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

#ifndef _MOCK_PLAYER_H
#define _MOCK_PLAYER_H

#include "player.h"
#include "sys.h"

#include <sys/time.h>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;


class cmock_player : public cplayer {
public:

  static const int kdefault_pos_ms = 12345;

  // public ctor/dtor, so we can instantiate it multiple times for testing
  cmock_player::cmock_player() :
    cur_song(0), cur_pos_ms(kdefault_pos_ms), state(cplayer::kstopped())
  {
    playlist.push_back("./foo");
    playlist.push_back("./bar");
    playlist.push_back("./baz z");
  }

  virtual ~cmock_player() {}

  const vector<string> get_playlist() { return playlist; }
  void set_playlist(const vector<string> &list) { playlist = list; }

  int get_cur_song() { return cur_song; }
  void set_cur_song(int cur) { cur_song = cur; }

  int get_pos_ms() {
    return cur_pos_ms;
  }

  void set_pos_ms(int ms) {
    if (state == kplaying())
      cur_pos_ms = ms;
  }

  virtual void add_timer(int freq_ms, bool (*callback)()) {
    last_timer = callback;
  }

  // it looks weird, i know. it's returning a function pointer.
  bool (*get_last_timer())() {
    return last_timer;
  }

  void play() {
    state = cplayer::kplaying();
    cur_pos_ms = kdefault_pos_ms;
  }

  void stop() {
    state = cplayer::kstopped();
    cur_pos_ms = 0;
  }

  const string &get_state() { return state; }


private:

  friend cplayer &cplayer::instance();

  vector<string> playlist;
  int cur_song;
  int cur_pos_ms;
  string state;
  bool (*last_timer)();
};


// factory method, not really needed since the ctor is public
cplayer &cplayer::instance() {
  static cmock_player mock;
  return mock;
}



#endif // _MOCK_PLAYER_H

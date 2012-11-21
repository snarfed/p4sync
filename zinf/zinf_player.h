/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file zinf_player.h
 *
 * Defines the czinf_player class, a concrete subclass of cplayer for zinf.
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


#ifndef _ZINF_PLAYER_H
#define _ZINF_PLAYER_H

#include "../player.h"
#include "include/ui.h"
#include <string>
#include <vector>
using namespace std;

class FAContext;


// holy shit, multiple inheritance can actually be useful! who woulda thunk it?
class czinf_player : public cplayer, public UserInterface
{
public:
  czinf_player();

  // cplayer stuff
  const vector<string> get_playlist();
  void set_playlist(const vector<string> &playlist);
  int get_cur_song();
  void set_cur_song(int cur);
  int get_pos_ms();
  void set_pos_ms(int ms);
  void add_timer(int freq_ms, bool (*callback)());
  void play();
  void stop();
  const string &get_state();

  // UserInterface stuff
  void set_facontext(FAContext *context)    { facontext = context; }
  FAContext *get_facontext()                { return facontext; }
  virtual Error Init(int32_t startup_level);
  virtual Error AcceptEvent(Event *event);

private:
  FAContext *facontext;
  int pos_ms;
  float ms_per_frame;
  int length_ms;
  int length_frames;
  int song;
  const string *state;
};

#endif  // _ZINF_PLAYER_H

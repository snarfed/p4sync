/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file client.cpp
 *
 * Defines functions for p4sync clients to contact p4sync servers and the web
 * and SNTP servers that run inside them.
 *
 * The code is exposed as plain functions, instead of a class, so that players
 * that require a C interface for their plugins can still use it.
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


#include "client.h"
#include "player.h"
#include "trace.h"
#include "sys.h"
#include "libmsntp/libmsntp.h"
#include "http-tiny/http_lib.h"

#include <cassert>
#include <utility>
#include <sstream>
#include <string>
using namespace std;


static string ghostname = "";
static int gport = 0;
struct timeval goffset;
bool gconnected = false;


// local functions. not static so client_test can test them.
const vector<string> get_server_playlist();
int get_server_cur_song();
pair<struct timeval, int> get_server_cur_pos();
const string get_server_state();
pair<http_retcode, string> p4sync_http_get(const string &uri);
int pos_to_sync(const struct timeval &server_time, int server_pos);


int client_connect() {
  if (gconnected)
    return 0;
  else if (ghostname != "" && gport != 0)
    return client_connect(ghostname, gport);
  else
    return -1;
}

int client_connect(const string &hostname, const int port) {
  gconnected = false;
  ghostname = string(hostname);
  gport = port;

  ctrace::instance() << acquire("client") << "Connecting to p4sync server "
                     << hostname << ":" << port << "." << release();

  // check that a p4sync http server is up and responding
  pair<http_retcode, string> ret = p4sync_http_get("" /* root page */);

  if (ret.first != OK200) {
    ctrace::instance() << acquire("client") << "Received HTTP error code "
                       << ret.first << ", aborting." << release();
    return ret.first;
  }

  if (ret.second.find("p4sync</a> is serving!") == string::npos) {
    ctrace::instance() << acquire("client") << "Received:\n" << ret.second
                       << "\ninstead of p4sync preamble. Aborting."
                       << release();
    return 1;
  }

  ctrace::instance() << acquire("client") << "Connected to p4sync http server."
                     << release();

  // check that an sntp server (over udp) is up and responding, and record the
  // offset between the local clock and the server clock
  char *hostname_cstr = const_cast<char *>(hostname.c_str());
  int err = msntp_get_offset(hostname_cstr, port, &goffset);
  if (err) {
    ctrace::instance() << acquire("client") << "Received libmsntp error code "
                       << err << ", aborting." << release();
    return 2;
  }

  float offset = (float)goffset.tv_sec + (float)goffset.tv_usec / 1000000;
  ctrace::instance()
    << acquire("client") << "Connected to p4sync sntp server. Server clock is "
    << offset << " seconds " << ((offset >= 0) ? "ahead of" : "behind")
    << " local clock." << release();

  gconnected = true;
  return 0;
}


// this is currently a noop
void client_disconnect() {
  if (gconnected) {
    ctrace::instance() << acquire("client") << "Disconnecting from server "
                       << ghostname << ":" << gport << release();
    gconnected = false;
  }
}


// FUCKING GET ME A TOWEL!!
void sync_player() {
  if (!cplayer::instance().is_plugin_enabled())
    return;

  if (!gconnected) {
    ctrace::instance() << acquire("client") << "Client is not connected!"
                       << release();
    return;
  }

  cplayer &player = cplayer::instance();

  // only sync if both client and server are playing
  if (player.get_state() != cplayer::kplaying())
    return;

  if (get_server_state() != cplayer::kplaying())
    return;


  // sync playlist
  vector<string> playlist = get_server_playlist();
//   if (playlist != player.get_playlist())
//     player.set_playlist(playlist);

  // sync song
  int song = get_server_cur_song();
  if (song != player.get_cur_song())
    player.set_cur_song(song);

  // sync position
  player.play();

  pair<struct timeval, int> server_pos = get_server_cur_pos();
  int seek = pos_to_sync(server_pos.first, server_pos.second);
  if (seek < 0)
    return;

  // TODO: only seek if the current pos is more than a delta off
  if (seek != player.get_pos_ms())
    player.set_pos_ms(seek);
}


// local functions

const vector<string> get_server_playlist() {
  pair<http_retcode, string> ret = p4sync_http_get("playlist");

  vector<string> playlist;

  if (ret.first == OK200) {
    stringstream sstream(ret.second);

    // HACK: we silently truncate if the song name is more than 4000 chars.
    // this should never happen, but still, we should do something better.
    char buf[4000];

    while (sstream.getline(buf, sizeof(buf)))
      playlist.push_back(buf);
  }

  return playlist;
}

int get_server_cur_song() {
  pair<http_retcode, string> ret = p4sync_http_get("cursong");

  int song = -1;

  if (ret.first == OK200) {
    stringstream sstream(ret.second);
    sstream >> song;
  }

  return song;
}

/** get_server_curpos
 *
 * Returns the server's current time and the position in the song, in ms, at
 * that time, as a pair.
 */
pair<struct timeval, int> get_server_cur_pos() {
  pair<http_retcode, string> ret = p4sync_http_get("curpos");

  struct timeval time = { 0, 0 };
  int pos = 0;

  if (ret.first == OK200) {
    stringstream sstream(ret.second);
    string buf;

    sstream >> buf >> time.tv_sec >> time.tv_usec;
    assert(buf == "server_time");

    sstream >> buf >> pos;
    assert(buf == "song_pos_ms");
  }

  return make_pair(time, pos);
}

const string get_server_state() {
  pair<http_retcode, string> ret = p4sync_http_get("state");

  if (ret.first == OK200)
    return ret.second;
  else
    return "";
}


/** pos_to_sync
 *
 * Returns the position in the current song, in ms, that the local player
 * should seek to in order to synchronize with the server. The time difference
 * between the local and server clocks is compensated for.
 *
 * On failure, returns -1.
 */
int pos_to_sync(const struct timeval &server_time, int server_pos) {
  // calculate the position to seek to, and sanity check it. it shouldn't be
  // negative or over, say, 24 hours. note that goffset is server minus client.
  // if positive, the server's clock is ahead of the client's.
  // 
  // obligatory ascii art:
  //                            client
  //                server     seeks to
  //                  pos        here
  // song [============|==========|==============]
  // 
  //  elapsed:         {------------------}
  // 
  //  offset:                     (-------)
  // 
  struct timeval server_now = timeval_add(gettimeofday(), goffset);
  int delta = timeval_to_ms(timeval_subtract(server_now, server_time));
  int seek_to = server_pos + delta;

  ctrace::instance() << acquire("client") << "Calculated seek is "
                     << seek_to << " ms" << release();

  if (seek_to < 0) {
    ctrace::instance() << acquire("client")
                       << "Error: can't seek before the beginning of the song."
                       << release();
    return -1;
  } else if (seek_to > 24 * 60 * 60 * 1000) {
    ctrace::instance() << acquire("client")
                       << "Error: won't seek over a day ahead."
                       << release();
    return -1;
  }

  return seek_to;
}


/** p4sync_http_get
 *
 * GETs a given URI from the current p4sync server. If the returned
 * http_retcode is OK200, the returned string will be the retrieved data;
 * otherwise, the http_retcode contains the HTTP error code, and the string is
 * undefined.
 */
pair<http_retcode, string> p4sync_http_get(const string &uri) {
  ctrace::instance() << acquire("client") << "GETting " << ghostname << ":"
                     << gport << "/" << uri << release();

  if (!cplayer::instance().is_plugin_enabled()) {
    ctrace::instance() << acquire("client") << "Plugin is not initialized!"
                       << release();
    return make_pair(ERR501, "");
  }

  http_server = const_cast<char *>(ghostname.c_str());
  http_port = gport;

  char *data, *uri_cstr = const_cast<char *>(uri.c_str());
  int length;
  http_retcode retcode = http_get(uri_cstr, &data, &length, 0);

  string datastr;
  if (retcode == OK200) {
    datastr = string(data, length);
    ctrace::instance() << acquire("client") << "Received:\n" << datastr
                       << release();
  } else {
    ctrace::instance() << acquire("client") << "Received HTTP error code "
                       << retcode << release();
  }

  return make_pair(retcode, datastr);
}

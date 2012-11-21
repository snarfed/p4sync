/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file server.cpp
 *
 * Defines functions for managing the web server (libwebserver) and SNTP server
 * (libmsntp) that run on the server, as declared in server.h.
 *
 * The code exposed as plain functions, instead of a class, so that players
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


#include "server.h"
#include "player.h"
#include "trace.h"
#include "settings.h"
#include "sys.h"
#include "libwebserver/web_server.h"
#include "libmsntp/libmsntp.h"

#include <iostream>
#include <string>
#include <numeric>
#include <cassert>
using namespace std;


struct web_server gweb_server;

// this is shown on the root page, ie localhost:port/
const static string kroot_html(
  "<a href='http://snarfed.org/p4sync'>p4sync</a> is serving! \n"
  "<ul><li><a href='/playlist'> /playlist </a></li> \n"
  "<li><a href='/cursong'> /cursong </a></li> \n"
  "<li><a href='/curpos'> /curpos </a></li> \n"
  "<li><a href='/state'> /state </a></li> \n"
  "<li><a href='/version'> /version </a></li></ul> \n");


// web server handlers 
void root_handler();
void playlist_handler();
void cursong_handler();
void curpos_handler();
void state_handler();
void version_handler();
void prepare_to_handle(const string &content_type);


int server_init(int port)
{
  if (!cplayer::instance().is_plugin_enabled()) {
    ctrace::instance() << acquire("server") << "Asked to initialize, but "
                       << "plugin is not initialized." << release();
    return -1;
  }

  // initialize libwebserver
  ctrace::instance() << acquire("server") << "Starting libwebserver on port "
                     << port << "." << release();
  web_server_init(&gweb_server, port, NULL /* log file */, 0 /* flags */);

  // add handlers. (WS_USELEN tells libwebserver to calculate and sent the
  // Content-length header.)
  web_server_addhandler(&gweb_server, "* /", root_handler, WS_USELEN);
  web_server_addhandler(&gweb_server, "* /playlist", playlist_handler,
                        WS_USELEN);
  web_server_addhandler(&gweb_server, "* /cursong", cursong_handler,
                        WS_USELEN);
  web_server_addhandler(&gweb_server, "* /curpos", curpos_handler,
                        WS_USELEN);
  web_server_addhandler(&gweb_server, "* /state", state_handler, WS_USELEN);
  web_server_addhandler(&gweb_server, "* /version", version_handler,
                        WS_USELEN);

  // initialize libmsntp
  ctrace::instance() << acquire("server") << "Starting libmsntp on port "
                     << port << "." << release();
  msntp_start_server(port);

  return 0;
}

void server_shutdown() {
  ctrace::instance() << acquire("server") << "Shutting down." << release();
  web_server_shutdown(&gweb_server);
  msntp_stop_server();
}

int serve() {
  if (!cplayer::instance().is_plugin_enabled())
    return -1;

  web_server_run(&gweb_server);
  msntp_serve();
  return 0;
}


/* root, playlist, cursong, curpos, status handlers
 *
 * Handlers for the http requests "/", "/playlist", "/cursong", "/curpos", and
 * "/status," respectively. They return what they say they return. cursong is
 * the index, status is "playing" or "stopped," and curpos is the time the
 * currently playing song was started, as seconds and microseconds, separated
 * by a space, since the epoch (midnight 1/1/1970). The "/" handler prints
 * p4sync information and links to the other handlers.
 */
void root_handler() {
  prepare_to_handle("html");
  cout << kroot_html;
}

void playlist_handler()
{
  prepare_to_handle("plain");

  const vector<string> playlist = cplayer::instance().get_playlist();

  for (vector<string>::const_iterator it = playlist.begin();
       it != playlist.end(); it++) {
    cout << *it << endl;

    // extract path
    string::size_type path_end = it->rfind(DIR_SEPARATOR);
    assert(path_end != string::npos);
    string path = it->substr(0, path_end + 1);

    // the song should have a path. expose files in that path in the web
    // server, under /playlist/*, so that clients can download the songs
    ctrace::instance() << acquire("server") << "Exposing path " + path
                       << " under /playlist/" << release();
    web_server_aliasdir(&gweb_server, "playlist",
                        const_cast<char *>(path.c_str()), WS_USELEN);
  }
}

void cursong_handler()
{
  prepare_to_handle("plain");
  cout << cplayer::instance().get_cur_song();
}

void curpos_handler()
{
  prepare_to_handle("plain");

  struct timeval now = gettimeofday();
  cout << "server_time " << now.tv_sec << " " << now.tv_usec << endl;
  cout << "song_pos_ms " << cplayer::instance().get_pos_ms() << endl;
}

void state_handler()
{
  prepare_to_handle("plain");
  cout << cplayer::instance().get_state();
}

void version_handler()
{
  prepare_to_handle("plain");
  cout << VERSION;
}


void prepare_to_handle(const string &content_type) {
  if (ClientInfo) {
    // ClientInfo will be null in unit tests
    ctrace::instance() << acquire("server") << "Request from "
                       << ClientInfo->inetname << ": " << ClientInfo->request
                       << release();
  }

  cout << "Content-type: text/" << content_type << "\r\n\r\n";
}


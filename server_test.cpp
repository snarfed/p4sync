/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file server_test.cpp
 *
 * Unit test for server.cpp.
 *
 * Note that libwebserver redirects stdout for its handlers, so this test
 * redirects stdout to an internal stringbuf so it can check the handler
 * output.
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

#include "cxxtest/cxxtest/TestSuite.h"
#include "server.h"
#include "settings.h"
#include "libwebserver/web_server.h"

#include <iostream>
#include <sstream>
#include <string>

// defined in server.cpp
extern void root_handler();
extern void playlist_handler();
extern void cursong_handler();
extern void curpos_handler();
extern void state_handler();
extern void version_handler();
extern struct web_server gweb_server;

class cserver_test : public CxxTest::TestSuite
{
private:
  stringbuf output;
  streambuf *original_cout;

public:
  void setUp() {
    // need to start libwebserver because the playlist handler calls
    // web_server_aliasdir()
    if (gweb_server.socket <= 0)
      web_server_init(&gweb_server, 12345 /* port */, NULL /* log file */,
                      0 /* flags */);

    if (!cplayer::instance().is_plugin_enabled())
      cplayer::instance().plugin_enabled();
    cplayer::instance().play();

    // redirect cout to a stringbuf, so it can be checked by tests
    output.str("");
    original_cout = cout.rdbuf(&output);
  }

  void tearDown() {
    // restore cout
    cout.rdbuf(original_cout);
  }

  void test_init() {
    // server should refuse to start if the player plugin is not enabled
    cplayer::instance().plugin_disabled();
    TS_ASSERT_EQUALS(-1, server_init(12345 /* port */));
  }

  void test_root_handler() {
    root_handler();

    const string start("p4sync</a> is serving!");
    TS_ASSERT_DIFFERS(string::npos, output.str().find(start));
  }

  void test_playlist_handler() {
    playlist_handler();

    TS_ASSERT_EQUALS("Content-type: text/plain\r\n\r\n./foo\n./bar\n./baz z\n",
                     output.str());
  }

  void test_cursong_handler() {
    cursong_handler();

    TS_ASSERT_EQUALS("Content-type: text/plain\r\n\r\n0", output.str());
  }

  void test_curpos_handler() {
    // pause to let it get farther into the song
    sleep_ms(100);

    curpos_handler();

    istream received(&output);
    string buf;

    received >> buf;
    TS_ASSERT_EQUALS("Content-type:", buf);
    received >> buf;
    TS_ASSERT_EQUALS("text/plain", buf);

    struct timeval time;
    received >> buf >> time.tv_sec >> time.tv_usec;
    TS_ASSERT_EQUALS("server_time", buf);
    TS_ASSERT_DELTA(gettimeofday().tv_sec, time.tv_sec, 1);

    int pos;
    received >> buf >> pos;
    TS_ASSERT_EQUALS("song_pos_ms", buf);
    TS_ASSERT_EQUALS(cmock_player::kdefault_pos_ms, pos);
  }

  void test_state_handler() {
    state_handler();

    TS_ASSERT_EQUALS("Content-type: text/plain\r\n\r\nplaying", output.str());
  }

  void test_version_handler() {
    version_handler();

    TS_ASSERT_EQUALS("Content-type: text/plain\r\n\r\n" VERSION, output.str());
  }
};

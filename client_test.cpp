/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file client_test.cpp
 *
 * Unit test for client.cpp.
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

#include "cxxtest/TestSuite.h"
#include "client.h"
#include "mock_player.h"
#include "player.h"
#include "trace.h"
#include "settings.h"
#include "sys.h"
#include "http-tiny/http_lib.h"

#include <iostream>
#include <sstream>
#include <string>
using namespace std;


// defined in client.cpp
extern struct timeval goffset;
extern const vector<string> get_server_playlist();
extern int get_server_cur_song();
extern pair<struct timeval, int> get_server_cur_pos();
extern const string get_server_state();
int pos_to_sync(const struct timeval &server_time, int server_pos);
extern pair<http_retcode, string> p4sync_http_get(const string &uri);


class cclient_test : public CxxTest::TestSuite
{
public:
  static cplayer &player;

  void setUp() {
    // turn off debugging output
    csettings::instance().set<bool>(ctrace::kmaster_setting, false);

    if (!player.is_plugin_enabled())
      player.plugin_enabled();

    int ret = client_connect("localhost",
                             csettings::instance().get<int>("server_port"));
    TS_ASSERT_EQUALS(0, ret);
  }

  void tearDown() {
    client_disconnect();
  }

  void test_connect() {
    // client should refuse to connect if the player plugin is not enabled
    player.plugin_disabled();
    TS_ASSERT_EQUALS(ERR501, client_connect("localhost", 1234 /* port */));
  }

  void test_get_server_playlist() {
    const vector<string> playlist = get_server_playlist();
    TS_ASSERT_EQUALS(3, playlist.size());
    TS_ASSERT_EQUALS("./foo", playlist[0]);
    TS_ASSERT_EQUALS("./bar", playlist[1]);
    TS_ASSERT_EQUALS("./baz z", playlist[2]);
  }

  void test_song_download() {
    const vector<string> playlist = get_server_playlist();

    // the tests are running in the local directory where this source files
    // lives, so try to download this file
    pair<http_retcode, string> ret = p4sync_http_get("playlist/"__FILE__);
    TS_ASSERT_EQUALS(OK200, ret.first);
  }

  void test_get_server_cur_song() {
    TS_ASSERT_EQUALS(0, get_server_cur_song());
  }

  void test_get_server_cur_pos() {
    pair<struct timeval, int> ret = get_server_cur_pos();
    TS_ASSERT_DELTA(gettimeofday().tv_sec, ret.first.tv_sec, 1);
    TS_ASSERT_EQUALS(cmock_player::kdefault_pos_ms, ret.second);
  }

  void test_get_server_state() {
    TS_ASSERT_EQUALS(cplayer::kplaying(), get_server_state());
  }

  void test_pos_to_sync() {
    struct timeval time = gettimeofday();

    // error condition: seeking negative is not allowed
    time.tv_sec += 10;
    TS_ASSERT_EQUALS(-1, pos_to_sync(time, 0));

    // error condition: seeking more than a day ahead is not allowed
    time.tv_sec -= 25 * 60 * 60;
    TS_ASSERT_EQUALS(-1, pos_to_sync(time, 0));

    // test a few different valid server times
    time = gettimeofday();
    check_pos_to_sync(time);

    time.tv_usec -= 5000;
    check_pos_to_sync(time);

    time.tv_sec -= 5;
    check_pos_to_sync(time);
  }

  // for a given server time, test a wide range of positions and clock offsets
  void check_pos_to_sync(const struct timeval &time) {
    int poses_to_check[3] = { 1230, 12300, 123000 };
    struct timeval offsets[5] = { { 0, 0 }, { 1, 0 }, { -1, 0 },
                                  { 0, -200 * 1000 }, { 0, 200 * 1000 } };

    for (uint i = 0; i < 3; i++) {
      for (uint j = 0; j < 5; j++) {
        int pos = poses_to_check[i];
        goffset = offsets[j];
        struct timeval now = gettimeofday();
      
        int seek = pos_to_sync(time, pos);
        struct timeval delta = timeval_subtract(now, time);
        int expected = pos + timeval_to_ms(delta) - timeval_to_ms(goffset);
        TS_ASSERT_DELTA(expected, seek, 100);
      }
    }
  }

  void test_sync_pos() {
    // if plugin isn't enabled, sync should be a noop
    player.plugin_disabled();
    int cur = player.get_pos_ms();
    sync_player();
    TS_ASSERT_EQUALS(cur, player.get_pos_ms());

    // if stopped, sync should be a noop
    player.plugin_enabled();
    player.set_pos_ms(0);
    player.stop();

    sync_player();
    TS_ASSERT_EQUALS(cplayer::kstopped(), player.get_state());
    TS_ASSERT_EQUALS(0, player.get_pos_ms());

    // if playing, sync should seek to the right position
    player.play();
    player.set_pos_ms(1);

    sync_player();
    TS_ASSERT_EQUALS(cplayer::kplaying(), player.get_state());

    int pos = player.get_pos_ms();
    TS_ASSERT_LESS_THAN_EQUALS(cmock_player::kdefault_pos_ms, pos);
    TS_ASSERT_DELTA(cmock_player::kdefault_pos_ms, pos, 100);
  }

  void test_sync_song() {
    int song = get_server_cur_song();
    TS_ASSERT_DIFFERS(-1, song);

    int different = (song == 0) ? 1 : 0;
    player.set_cur_song(different);
    player.play();

    sync_player();
    TS_ASSERT_EQUALS(song, player.get_cur_song());
  }

  // TODO: re-enable this test when playlist synching is working!
  void temporarily_dont_test_sync_playlist() {
    vector<string> playlist;
    player.set_playlist(playlist);

    player.play();
    sync_player();

    playlist = player.get_playlist();
    TS_ASSERT_EQUALS(3, playlist.size());
    TS_ASSERT_EQUALS("./foo", playlist[0]);
    TS_ASSERT_EQUALS("./bar", playlist[1]);
    TS_ASSERT_EQUALS("./baz z", playlist[2]);
  }
};


// initialize static members
cplayer &cclient_test::player(cplayer::instance());

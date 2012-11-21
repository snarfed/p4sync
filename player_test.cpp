/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file player_test.cpp
 *
 * Unit test for player.cpp.
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
#include "player.h"
#include "settings.h"


// defined in player.cpp
extern bool server_callback();
extern bool sync_callback();
extern bool play_callback();

// defined in client.cpp
extern int client_connect();
extern void client_disconnect();
extern bool gconnected;

class cplayer_test : public CxxTest::TestSuite
{
public:
  static cmock_player &mock;
  int orig_server_port;

  void setUp() {
    // switch ports so we don't collide with server_stub, which is running
    orig_server_port = csettings::instance().get<int>("server_port");
    csettings::instance().set("server_port", orig_server_port + 1);
  }

  void tearDown() {
    csettings::instance().set("server_port", orig_server_port);
  }

  void test_plugin_enabled() {
    if (mock.is_plugin_enabled())
      mock.plugin_disabled();

    // when we're a server, plugin_enabled should start the servers and add a
    // server_callback() timer
    csettings::instance().set("run_server", true);
    mock.plugin_enabled();
    TS_ASSERT_EQUALS(true, mock.is_plugin_enabled());

    // this only indirectly tests that the servers started up, but meh
    TS_ASSERT_EQUALS(server_callback, mock.get_last_timer());

    mock.plugin_disabled();
    TS_ASSERT_EQUALS(false, mock.is_plugin_enabled());

    // when we're a client, plugin_enabled should connect to the server and add
    // play_callback() and sync_callback() timers
    csettings::instance().set("run_server", false);
    csettings::instance().set("sync_freq_ms", 5000);
    mock.plugin_enabled();
    TS_ASSERT_EQUALS(sync_callback, mock.get_last_timer());
  }

  // play_callback() should sync if the user recently pressed "play"
  void test_play_callback() {
    client_disconnect();

    mock.stop();
    play_callback();
    TS_ASSERT_EQUALS(cplayer::kstopped(), mock.get_state());
    TS_ASSERT_EQUALS(false, gconnected);

    mock.play();
    sleep_ms(100);  // pause to let it get farther into the song

    play_callback();
    TS_ASSERT_EQUALS(cplayer::kplaying(), mock.get_state());
    TS_ASSERT_EQUALS(true, gconnected);

    client_disconnect();
    mock.play();
    mock.set_pos_ms(1);
    play_callback();
    TS_ASSERT_EQUALS(cplayer::kplaying(), mock.get_state());
    TS_ASSERT_EQUALS(true, gconnected);
  }
};


// initialize static members
cmock_player &cplayer_test::mock(
  reinterpret_cast<cmock_player &>(cplayer::instance()));

/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file mock_player_test.cpp
 *
 * Unit test for mock_player.h.
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
#include "mock_player.h"
#include "player.h"


class cmock_player_test : public CxxTest::TestSuite
{
private:
  cmock_player mock;

public:

  void setUp() {
    if (!mock.is_plugin_enabled())
        mock.plugin_enabled();
  }

  void test_create() {
    TS_ASSERT_EQUALS("./foo", mock.get_playlist()[0]);
    TS_ASSERT_EQUALS("./bar", mock.get_playlist()[1]);
    TS_ASSERT_EQUALS("./baz z", mock.get_playlist()[2]);
    TS_ASSERT_EQUALS(0, mock.get_cur_song());
    TS_ASSERT_EQUALS(cplayer::kstopped(), mock.get_state());
  }

  void test_is_plugin_enabled() {
    TS_ASSERT_EQUALS(true, mock.is_plugin_enabled());
    mock.plugin_disabled();
    TS_ASSERT_EQUALS(false, mock.is_plugin_enabled());
  }

  void test_playlist() {
    vector<string> empty;
    mock.set_playlist(empty);
    TS_ASSERT_EQUALS(0, mock.get_playlist().size());

    vector<string> single;
    single.push_back("snarf");
    mock.set_playlist(single);
    TS_ASSERT_EQUALS(1, mock.get_playlist().size());
    TS_ASSERT_EQUALS("snarf", mock.get_playlist()[0]);
  }

  void test_cur_song() {
    mock.set_cur_song(6);
    TS_ASSERT_EQUALS(6, mock.get_cur_song());
  }

  void test_pos() {
    mock.play();
    TS_ASSERT(mock.get_pos_ms() == cmock_player::kdefault_pos_ms);

    mock.set_pos_ms(987);
    TS_ASSERT_EQUALS(987, mock.get_pos_ms());
  }

  void test_pos_while_stopped() {
    mock.stop();
    TS_ASSERT_EQUALS(0, mock.get_pos_ms());

    mock.set_pos_ms(100);
    TS_ASSERT_EQUALS(0, mock.get_pos_ms());
  }

  void test_add_timer() {
    mock.add_timer(0, (bool (*)())123);
    TS_ASSERT_EQUALS((bool (*)())123, mock.get_last_timer());
  }

  void test_state() {
    mock.play();
    TS_ASSERT_EQUALS(cplayer::kplaying(), mock.get_state());

    mock.stop();
    TS_ASSERT_EQUALS(cplayer::kstopped(), mock.get_state());
  }
};

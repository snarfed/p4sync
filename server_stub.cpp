/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file server_stub.cpp
 *
 * Simple stub p4sync server. Runs the built-in SNTP and web servers and
 * returns valid responses for the supported handlers. Used with the
 * client_test.cpp unit test.
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
#include "mock_player.h"
#include "settings.h"
#include "sys.h"

#include <iostream>
using namespace std;


int main(int argc, char **argv) {
  csettings::instance().set("run_server", true);
  cplayer::instance().plugin_enabled();  // this starts the server
  cplayer::instance().play();

  while (true) {
    serve();
    sleep_ms(10);  // pause for a while so we don't peg the cpu
  }

  cplayer::instance().plugin_disabled();
}

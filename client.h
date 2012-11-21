/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file client.h
 *
 * Declares functions for p4sync clients to contact p4sync servers and the web
 * and SNTP servers that run inside them.
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

#ifndef _CLIENT_H
#define _CLIENT_H

#include <string>
using namespace std;


/** client_connect, client_disconnect
 *
 * Connect to, and disconnect from, a p4sync server. On connect, retrieves the
 * server's clock using SNTP, and records the difference from the local clock.
 * If the hostname and port are not provided, connects to the last server.
 *
 * Return 0 on success, > 0 on failure.
 */
int client_connect(const string &hostname, const int port);
int client_connect();
void client_disconnect();


/** sync
 *
 * If the player is already playing, synchronizes the player with the server.
 * (If the player is not playing, we respect the user's wishes and don't start
 * it playing). Loads the server's playlist, starts playing the same song as
 * the server, retrieves the server's start time, calculates the offset, and
 * seeks to the correct position.
 */
void sync_player();


#endif  // _CLIENT_H

/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file server.h
 *
 * Declares functions for managing the web server (libwebserver) and SNTP
 * server (libmsntp) that run on the p4sync server. Player plugins should call
 * these functions on startup, shutdown, and in their main loop or callback.
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

#ifndef _SERVER_H
#define _SERVER_H


/** server_init, server_shutdown
 *
 * Initialize and shut down the web and SNTP servers. The web server serves
 * HTTP over TCP, and SNTP is served over UDP, both on the same port.
 */
int server_init(int port);
void server_shutdown();


/* serve
 *
 * Listens for web and SNTP client requests and handles them. If no requests
 * arrive, this function will time out. Should be called repeatedly from within
 * a plugin main loop or callback.
 */
int serve();


#endif // _SERVER_H

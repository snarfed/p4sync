/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file xmms_plugin.c
 *
 * The actual XMMS plugin. Written in C so that XMMS will load it. There's no
 * real code here, just wrappers that call methods in xmms_player.h.
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

#include "xmms_plugin.h"
#include <xmms/plugin.h>


// defined in xmms_player.cpp
extern void xmms_init();
extern void xmms_about();
extern void xmms_config();
extern void xmms_cleanup();


// callbacks used by xmms
GeneralPlugin *get_gplugin_info();
static void xmms_c_init();
static void xmms_c_about();
static void xmms_c_config();
static void xmms_c_cleanup();

static GeneralPlugin gxmms_plugin = {
  0,              // filled in by XMMS
  0,              // filled in by XMMS
  -1,             // session ID
  "p4sync " VERSION,
  xmms_c_init,
  xmms_c_about,
  xmms_c_config,
  xmms_c_cleanup
};


GeneralPlugin *get_gplugin_info(void)
{
  return &gxmms_plugin;
}


int get_xmms_session() {
  return gxmms_plugin.xmms_session;
}


// these just pass through to extern "C" functions in xmms_player
void xmms_c_init()
{
  xmms_init();
}

void xmms_c_about()
{
  xmms_about();
}

void xmms_c_config()
{
  xmms_config();
}

void xmms_c_cleanup()
{
  xmms_cleanup();
}

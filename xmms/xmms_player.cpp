/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file xmms_player.cpp
 *
 * Implements the cxmms_player class, a concrete subclass of cplayer for XMMS.
 * Also implements the real callback methods for the xmms plugin, which are
 * passed through from the extern "C" functions in xmms_plugin.c.
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

#include "xmms_player.h"
#include "xmms_plugin.h"
#include "../player.h"
#include "../server.h"
#include "../client.h"
#include "../trace.h"
#include "../settings.h"

#include <xmms/xmmsctrl.h>
#include <xmms/util.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
using namespace std;


static GtkWidget *gwindow, *gsync_radio, *gserver_radio, *gsync_host,
  *gsync_port, *gserver_port;

extern "C" void sync_focus_callback();
extern "C" void server_focus_callback();


// called by xmms_plugin.c
extern "C" void xmms_init()
{
  ctrace::instance() << acquire("xmms") << "init" << release();
  cplayer::instance().plugin_enabled();
}

extern "C" void xmms_cleanup()
{
  ctrace::instance() << acquire("xmms") << "cleanup" << release();
  cplayer::instance().plugin_disabled();
}

extern "C" void xmms_about()
{
  ctrace::instance() << acquire("xmms") << "about" << release();

  xmms_show_message("p4sync " VERSION,
                    "p4sync " VERSION "\nhttp://snarfed.org/p4sync",
                    "OK", FALSE /* modal */, NULL, NULL);
}

extern "C" void ok_callback(GtkButton *ok, gpointer data)
{
  bool enabled = cplayer::instance().is_plugin_enabled();

  if (enabled)
    cplayer::instance().plugin_disabled();

  bool run_server = GTK_TOGGLE_BUTTON(gserver_radio)->active;
  csettings::instance().set("run_server", run_server);
  csettings::instance().set("sync_to_host",
                            gtk_entry_get_text(GTK_ENTRY(gsync_host)));
  csettings::instance().set("sync_to_port",
                            gtk_entry_get_text(GTK_ENTRY(gsync_port)));
  csettings::instance().set("server_port",
                            gtk_entry_get_text(GTK_ENTRY(gserver_port)));

  if (enabled)
    cplayer::instance().plugin_enabled();

  gtk_widget_destroy(gwindow);
}

// if the user clicks on one of the sync text widgets, this callback selects
// the sync radio button
extern "C" void sync_focus_callback() {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gsync_radio), TRUE);
}

// if the user clicks on the server port text widget, this callback selects the
// server radio button
extern "C" void server_focus_callback() {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gserver_radio), TRUE);
}


extern "C" void xmms_config()
{
  ctrace::instance() << acquire("xmms") << "config" << release();

  GtkWidget *ok, *cancel, *vbox, *bbox, *sync_box, *server_box;

  if (gwindow)
    return;

  // main window
  gwindow = gtk_window_new(GTK_WINDOW_DIALOG);
  gtk_window_set_title(GTK_WINDOW(gwindow), "Configure p4sync");
  gtk_window_set_policy(GTK_WINDOW(gwindow), FALSE, FALSE, FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(gwindow), 10);
  gtk_signal_connect(GTK_OBJECT(gwindow), "destroy",
                     GTK_SIGNAL_FUNC(gtk_widget_destroyed), &gwindow);

  vbox = gtk_vbox_new(FALSE, 10);

  // "sync to" box, radio button, label, and address/port entries
  sync_box = gtk_hbox_new(FALSE, 5);
  gsync_radio = gtk_radio_button_new_with_label(NULL, "Sync to");
  GSList *radio_group = gtk_radio_button_group(GTK_RADIO_BUTTON(gsync_radio));

  gtk_box_pack_start_defaults(GTK_BOX(sync_box), gsync_radio);
  gsync_host = gtk_entry_new();
  gtk_signal_connect(GTK_OBJECT(gsync_host), "focus_in_event",
                     sync_focus_callback, NULL);
  gtk_box_pack_start_defaults(GTK_BOX(sync_box), gsync_host);
  gtk_box_pack_start_defaults(GTK_BOX(sync_box), gtk_label_new(":"));
  gsync_port = gtk_entry_new_with_max_length(5);
  gtk_signal_connect(GTK_OBJECT(gsync_port), "focus_in_event",
                     sync_focus_callback, NULL);
  gtk_widget_set_usize(gsync_port, 45, -1 /* use default height */);
  gtk_box_pack_start_defaults(GTK_BOX(sync_box), gsync_port);

  // server box, radio button, label, and port entry
  server_box = gtk_hbox_new(FALSE, 5);
  gserver_radio = gtk_radio_button_new_with_label(radio_group, 
                                                  "Run server on port");

  gtk_box_pack_start_defaults(GTK_BOX(server_box), gserver_radio);
  gserver_port = gtk_entry_new_with_max_length(5);
  gtk_signal_connect(GTK_OBJECT(gserver_port), "focus_in_event",
                     server_focus_callback, NULL);
  gtk_widget_set_usize(gserver_port, 45, -1 /* use default height */);
  gtk_box_pack_start_defaults(GTK_BOX(server_box), gserver_port);

  // OK and Cancel buttons
  bbox = gtk_hbutton_box_new();
  ok = gtk_button_new_with_label("Ok");
  cancel = gtk_button_new_with_label("Cancel");
  GTK_WIDGET_SET_FLAGS(ok, GTK_CAN_DEFAULT);
  gtk_widget_grab_default(ok);
  gtk_signal_connect_object(GTK_OBJECT(cancel), "clicked",
                            GTK_SIGNAL_FUNC(gtk_widget_destroy),
                            GTK_OBJECT(gwindow));
  gtk_signal_connect(GTK_OBJECT(ok), "clicked",
                     GTK_SIGNAL_FUNC(ok_callback), NULL);
  gtk_box_pack_start_defaults(GTK_BOX(bbox), ok);
  gtk_box_pack_start_defaults(GTK_BOX(bbox), cancel);

  // main container
  gtk_container_add(GTK_CONTAINER(gwindow), vbox);
  gtk_box_pack_start_defaults(GTK_BOX(vbox), sync_box);
  gtk_box_pack_start_defaults(GTK_BOX(vbox), server_box);
  gtk_box_pack_start_defaults(GTK_BOX(vbox), bbox);

  // populate with current values
  gtk_entry_set_text(GTK_ENTRY(gsync_host),
    csettings::instance().get<string>("sync_to_host").c_str());
  gtk_entry_set_text(GTK_ENTRY(gsync_port),
    csettings::instance().get<string>("sync_to_port").c_str());
  gtk_entry_set_text(GTK_ENTRY(gserver_port),
    csettings::instance().get<string>("server_port").c_str());

  if (csettings::instance().get<bool>("run_server"))
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gserver_radio), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gsync_radio), TRUE);

  gtk_widget_show_all(gwindow);
}


// factory method
cplayer &cplayer::instance()
{
  static cxmms_player player;
  return player;
}


// xmms_player class methods
const vector<string> cxmms_player::get_playlist()
{
  ctrace::instance() << acquire("xmms") << "get_playlist" << release();

  uint num_songs = xmms_remote_get_playlist_length(get_xmms_session());
  vector<string> playlist;

  for (uint i = 0; i < num_songs; i++) {
    string filename(xmms_remote_get_playlist_file(get_xmms_session(), i));
    playlist.push_back(filename);
    ctrace::instance() << acquire("xmms") << filename << release();
  }

  return playlist;
}

void cxmms_player::set_playlist(const vector<string> &playlist)
{
  ctrace::instance() << acquire("xmms") << "set_playlist" << release();

  xmms_remote_playlist_clear(get_xmms_session());

  // add each song
  for (vector<string>::const_iterator it = playlist.begin();
       it != playlist.end(); it++) {

    char *song_cstr = const_cast<char *>(it->c_str());
    xmms_remote_playlist(get_xmms_session(), &song_cstr, 1 /* number ??? */,
                         true /* enqueue */);
    ctrace::instance() << acquire("xmms") << *it << release();
  }
}


int cxmms_player::get_cur_song()
{
  int song = xmms_remote_get_playlist_pos(get_xmms_session());

  ctrace::instance() << acquire("xmms") << "get_cur_song: " << song
                     << release();
  return song;
}


void cxmms_player::set_cur_song(int song)
{
  ctrace::instance() << acquire("xmms") << "set_cur_song: " << song
                     << release();
  xmms_remote_set_playlist_pos(get_xmms_session(), song);
}


int cxmms_player::get_pos_ms()
{
  int pos = xmms_remote_get_output_time(get_xmms_session());

  ctrace::instance() << acquire("xmms") << "get_pos_ms: " << pos << release();
  return pos;
}


void cxmms_player::set_pos_ms(int pos)
{
  ctrace::instance() << acquire("xmms") << "set_pos_ms: " << pos << release();
  xmms_remote_jump_to_time(get_xmms_session(), pos);
}


void cxmms_player::add_timer(int freq_ms, bool (*callback)())
{
  if (freq_ms > 0) {
    int tag = g_timeout_add(freq_ms, (GSourceFunc)callback, NULL);
    assert(tag != 0);
  }
}


void cxmms_player::play()
{
  ctrace::instance() << acquire("xmms") << "play" << release();
  if (!xmms_remote_is_playing(get_xmms_session()))
    xmms_remote_play(get_xmms_session());
}


void cxmms_player::stop()
{
  ctrace::instance() << acquire("xmms") << "stop" << release();
  xmms_remote_stop(get_xmms_session());
}


const string &cxmms_player::get_state()
{
  ctrace::instance() << acquire("xmms") << "get_state" << release();

  // order matters here. if xmms is paused, xmms_remote_is_paused() and
  // xmms_remote_is_playing() both return true
  if (xmms_remote_is_paused(get_xmms_session()))
    return kpaused();
  else if (xmms_remote_is_playing(get_xmms_session()))
    return kplaying();
  else
    return kstopped();
}

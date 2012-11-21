/**
 * measure.c
 * http://snarfed.org/xmms+time+precisiom
 *
 * Copyright 2005 Ryan Barrett <xmms@ryanb.org>
 *
 * Attempts to measure the precision of the xmms plugin calls for seeking and
 * getting the current position in the song. My hypothesis is that the
 * precision varies widely (up to three orders of magnitude) depending on the
 * input and output plugins used. Measurements are printed to stdout as long as
 * xmms is playing.
 *
 * To build (at least on RH9), first install xmms-devel, then do:
 *
 * $ gcc -Wall -lxmms -shared -I/usr/include/glib-1.2 -I/usr/lib/glib/include \
 *     -I/usr/include/gtk-1.2 measure.c -o libmeasure.so
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

#include <xmms/plugin.h>
#include <xmms/xmmsctrl.h>
#include <xmms/util.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


const static unsigned int kxmms_session = 0;
static gboolean genabled = FALSE;
static struct timeval glast_timestamp = { 0, 0 };
static int glast_pos = 0;

void init();
void about();
void cleanup();
gboolean get(void *clientdata);
gboolean seek(void *clientdata);
static float timeval_to_ms(const struct timeval time);
static struct timeval timeval_subtract(struct timeval x, struct timeval y);

static GeneralPlugin gxmms_plugin = {
  0,              // filled in by XMMS
  0,              // filled in by XMMS
  0,              // session ID
  "measure",
  init,
  about,
  NULL,           // configure
  cleanup
};


GeneralPlugin *get_gplugin_info(void)

{
  return &gxmms_plugin;
}


void init()
{
  int tag = g_timeout_add(2000, get, NULL);
  assert(tag != 0);
  genabled = TRUE;
}

void about()
{
  xmms_show_message("measure",
                    "measure\nhttp://snarfed.org/xmms+time+precision",
                    "OK", FALSE /* modal */, NULL, NULL);
}

void cleanup() {
  genabled = FALSE;
}

gboolean get(void *clientdata) {
  struct timeval now;
  int pos;
  float elapsed;

  if (!genabled || !xmms_remote_is_playing(kxmms_session))
    return TRUE;

  pos = xmms_remote_get_output_time(kxmms_session);
  gettimeofday(&now, NULL);

  // report elapsed real time and play time
  if (glast_pos > 0) {
    elapsed = timeval_to_ms(timeval_subtract(now, glast_timestamp));
    printf("%f ms elapsed, %d ms played\n", elapsed, pos - glast_pos);
  }

  glast_timestamp = now;
  glast_pos = pos;
  return TRUE;
}

gboolean seek(void *clientdata) {
  struct timeval then, now;
  int before, after, played;
  float elapsed;

  if (!genabled || !xmms_remote_is_playing(kxmms_session))
    return TRUE;

  // get "before" position
  gettimeofday(&then, NULL);
  before = xmms_remote_get_output_time(kxmms_session);
  gettimeofday(&now, NULL);
  elapsed = timeval_to_ms(timeval_subtract(now, then));
/*   printf("xmms_remote_get_output_time() took %f ms\n", elapsed); */

  // seek
  xmms_remote_jump_to_time(kxmms_session, 2000);

  // get "after" position
  gettimeofday(&then, NULL);
  after = xmms_remote_get_output_time(kxmms_session);
  gettimeofday(&now, NULL);
  elapsed = timeval_to_ms(timeval_subtract(now, then));
/*   printf("xmms_remote_get_output_time() took %f ms\n", elapsed); */

  // report position before and after seek
  printf("before %d ms, seek to 2000, after %d ms\n", before, after);

  // report elapsed real time and play time
  if (glast_pos > 0) {
    elapsed = timeval_to_ms(timeval_subtract(then, glast_timestamp));
    played = before - glast_pos;
    printf("%f ms elapsed since last seek, %d ms reported play time\n",
           elapsed, played);
  }

  glast_pos = after;
  glast_timestamp = then;

  return TRUE;
}


// utility functions

/** timeval_to_ms
 *
 * Converts a struct timeval to number of milliseconds since the epoch.
 */
float timeval_to_ms(const struct timeval time) {
  return time.tv_sec * 1000 + (float)time.tv_usec / 1000;
}

/** timeval_subtract
 *
 * Based on timeval_subtract from the glibc manual:
 * http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 *
 * Subtract the struct timeval values x and y, and return the result.
 */
struct timeval timeval_subtract(struct timeval x, struct timeval y)
{
  struct timeval result;

  /* Perform the carry for the later subtraction by updating y. */
  if (x.tv_usec < y.tv_usec) {
    int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
    y.tv_usec -= 1000000 * nsec;
    y.tv_sec += nsec;
  }
  if (x.tv_usec - y.tv_usec > 1000000) {
    int nsec = (x.tv_usec - y.tv_usec) / 1000000;
    y.tv_usec += 1000000 * nsec;
    y.tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result.tv_sec = x.tv_sec - y.tv_sec;
  result.tv_usec = x.tv_usec - y.tv_usec;
  return result;
}

/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file sys.cpp
 *
 * Contain functions for cross-platform portability.
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

#include "sys.h"

#include <cassert>

#ifdef _WIN32
#include <errno.h>
#include <Winsock2.h>

/**
 * Closes a socket or file, given a file descriptor.
 *
 * closesocket on win32 does the exact same thing as close(2) on *nix, it's
 * just named differently. sigh.
 */
int close(int sock)
{
  closesocket(sock);
}


/**
 * gettimeofday() -- gets the current time in elapsed seconds and
 * microsends since GMT Jan 1, 1970.
 *
 * ARGUMENTS: - Pointer to a timeval struct to return the time into
 *
 * RETURN CODES: - 0 on success
 * -1 on failure
 *
 * We got this code from:
 * http://www.freecode.com/viewsource/webstone/WebStone2.5-src::src::gettimeofday.c/
 */
int gettimeofday(struct timespec *curTimeP, int *junk)
{
  struct _timeb localTime;

  if (curTimeP == NULL) {
    errno = EFAULT;
    return (-1);
  }

 /**
  * Compute the elapsed time since Jan 1, 1970 by first
  * obtaining the elapsed time from the system using the
  * _ftime(..) call and then convert to the "timeval"
  * equivalent.
  */

  _ftime(&localTime);

  curTimeP->tv_sec = localTime.time + localTime.timezone;
  curTimeP->tv_nsec = localTime.millitm * 1000;

  return(0);
}

#endif // _WIN32


/** sleep_ms
 *
 * Like sleep(1), but allows millisecond sleep times. Uses select(2) under the
 * covers.
 */
void sleep_ms(int ms) {
  assert(ms >= 0);

  struct timeval timeout = { ms / 1000, (ms % 1000) * 1000 };
  select(0, 0, 0, 0, &timeout);
}


// note that this is not a substitue implementation on platforms without
// gettimeofday. it's an *overloaded* version that's more user friendly.
struct timeval gettimeofday() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv;
}


long timeval_to_ms(const struct timeval &time) {
  return time.tv_sec * 1000 + time.tv_usec / 1000;
}

struct timeval timeval_subtract(const struct timeval &x,
                                const struct timeval &y)
{
  struct timeval result, u(x), v(y);

  /* Perform the carry for the later subtraction by updating v. */
  if (u.tv_usec < v.tv_usec) {
    int nsec = (v.tv_usec - u.tv_usec) / 1000000 + 1;
    v.tv_usec -= 1000000 * nsec;
    v.tv_sec += nsec;
  }
  if (u.tv_usec - v.tv_usec > 1000000) {
    int nsec = (u.tv_usec - v.tv_usec) / 1000000;
    v.tv_usec += 1000000 * nsec;
    v.tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result.tv_sec = u.tv_sec - v.tv_sec;
  result.tv_usec = u.tv_usec - v.tv_usec;
  return result;
}


struct timeval timeval_add(const struct timeval &x, const struct timeval &y) {
  struct timeval sum = { x.tv_sec + y.tv_sec, x.tv_usec + y.tv_usec };
  return sum;
}

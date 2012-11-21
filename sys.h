/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file sys.h
 *
 * Contains functions and types for cross-platform portability.
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

#ifndef _SYS_H
#define _SYS_H


#if defined(_WIN32) || defined(__APPLE__)
  typedef unsigned int uint;
  typedef unsigned short ushort;
  typedef unsigned long ulong;
#endif  // _WIN32 || __APPLE__


#ifdef _WIN32
  #include <sys/timeb.h>
  int gettimeofday(struct timespec *curTimeP, int *junk);
  int close(int sock);
#else
  #include <sys/time.h>
#endif  // _WIN32


/** sleep_ms
 *
 * Like sleep(1), but allows millisecond sleep times. Uses select(2) under the
 * covers.
 */
void sleep_ms(int ms);


/** gettimeofday
 *
 * An overloaded, C++-friendly version of gettimeofday that only returns the
 * current time.
 */
struct timeval gettimeofday();


/** timeval_to_ms
 *
 * Converts a struct timeval to number of milliseconds since the epoch.
 */
long timeval_to_ms(const struct timeval &time);


/** timeval_subtract
 *
 * Based on timeval_subtract from the glibc manual:
 * http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 *
 * Subtracts y from x, and returns the difference as a struct timeval.
 */
struct timeval timeval_subtract(const struct timeval &x,
                                const struct timeval &y);


/** timeval_add
 *
 * Simple addition. Does *not* carry the overflow, so the result's tv_usec
 * member may be greater than 1000000!
 */
struct timeval timeval_add(const struct timeval &x, const struct timeval &y);


#ifdef _WIN32
  #define DIR_SEPARATOR '\\'
#else
  #define DIR_SEPARATOR '/'
#endif


#ifndef INADDR_NONE
  #define INADDR_NONE static_cast<ulong>(-1)  // for Solaris (and others?)
#endif

#endif  // _SYS_H


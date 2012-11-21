/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file sys_test.cpp
 *
 * Unit test for sys.cpp.
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

#include "cxxtest/cxxtest/TestSuite.h"
#include "sys.h"


/** operator== for struct timevals
 *
 * Needed to use TS_ASSERT_EQUALS with struct timevals.
 */
bool operator== (const struct timeval &x, const struct timeval &y) {
  return x.tv_sec == y.tv_sec && x.tv_usec == y.tv_usec;
}

class csys_test : public CxxTest::TestSuite
{
private:

  // these are statically initialized at the bottom of this file
  static const struct timeval zero, a, b, c;

public:

  void test_sleep_ms() {
    check_sleep_ms(100);
    check_sleep_ms(1);
    check_sleep_ms(0);
  }

  void check_sleep_ms(int ms) {
    struct timeval start = gettimeofday();
    sleep_ms(ms);
    struct timeval now = gettimeofday();

    int slept_ms = timeval_to_ms(timeval_subtract(now, start));
    int delta = max(100, slept_ms / 10);
    TS_ASSERT_DELTA(ms, slept_ms, delta);
  }

  void test_timeval_to_ms() {
    TS_ASSERT_EQUALS(0, timeval_to_ms(zero));
    TS_ASSERT_EQUALS(1900, timeval_to_ms(a));
    TS_ASSERT_EQUALS(2100, timeval_to_ms(b));
    TS_ASSERT_EQUALS(4000, timeval_to_ms(c));
  }

  void test_timeval_subtract() {
    const struct timeval
      b_minus_a = { 0, 200000 },
      c_minus_b = { 1, 900000 },
      a_minus_c = { -3, 900000 };

    TS_ASSERT_EQUALS(zero, timeval_subtract(zero, zero));
    TS_ASSERT_EQUALS(zero, timeval_subtract(a, a));
    TS_ASSERT_EQUALS(zero, timeval_subtract(b, b));
    TS_ASSERT_EQUALS(zero, timeval_subtract(c, c));
    TS_ASSERT_EQUALS(b_minus_a, timeval_subtract(b, a));
    TS_ASSERT_EQUALS(c_minus_b, timeval_subtract(c, b));
    TS_ASSERT_EQUALS(a_minus_c, timeval_subtract(a, c));
  }

  void test_timeval_add() {
    const struct timeval
      a_plus_b = { 3, 1000000 },
      a_plus_c = { 5, 900000 },
      b_plus_c = { 6, 100000 };

    TS_ASSERT_EQUALS(zero, timeval_add(zero, zero));
    TS_ASSERT_EQUALS(a, timeval_add(zero, a));
    TS_ASSERT_EQUALS(a, timeval_add(a, zero));
    TS_ASSERT_EQUALS(b, timeval_add(zero, b));
    TS_ASSERT_EQUALS(c, timeval_add(zero, c));
    TS_ASSERT_EQUALS(a_plus_b, timeval_add(a, b));
    TS_ASSERT_EQUALS(a_plus_c, timeval_add(a, c));
    TS_ASSERT_EQUALS(b_plus_c, timeval_add(b, c));
  }
};

// static initialization
const struct timeval csys_test::zero = { 0, 0 };
const struct timeval csys_test::a = { 1, 900000 };
const struct timeval csys_test::b = { 2, 100000 };
const struct timeval csys_test::c = { 4, 0 };

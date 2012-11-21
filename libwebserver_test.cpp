/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>,
 *
 * @file libwebserver_test.cpp
 *
 * A simple smoke test for libwebserver. Definitely *not* comprehensive.
 */

#include "cxxtest/TestSuite.h"
#include "libwebserver/web_server.h"

#include <stdio.h>


void foo_handler() {
  printf("bar");
}

class clibwebserver_test : public CxxTest::TestSuite
{
public:
  void test_libwebserver() {
    // initialize libwebserver
    struct web_server server;
    int ret = web_server_init(&server, 9001, 0 /* log file */, 0 /* flags */);
    TS_ASSERT_EQUALS(1, ret);

    ret = web_server_addhandler(&server, "* /foo", foo_handler, 0 /* flags */);
    TS_ASSERT_EQUALS(1, ret);

    // run the server and make sure it doesn't blow up.
    ret = web_server_run(&server);
    TS_ASSERT_EQUALS(2 /* 2 == no requests */, ret);

    // shut it down, and make sure we can start it again
    web_server_shutdown(&server);
    ret = web_server_init(&server, 9001, NULL, 0);
    TS_ASSERT_EQUALS(1, ret);

    ret = web_server_run(&server);
    TS_ASSERT_EQUALS(2, ret);
  }
};

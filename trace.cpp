/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file trace.cpp
 *
 * Implements the ctrace class. For more information, see trace.h.
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

#include "trace.h"

#include <fstream>
#include <iostream>
using namespace std;


/*****************************************************************************
 * INITIALIZE STATIC MEMBERS
 *****************************************************************************/
const string ctrace::kmaster_setting("trace.master");
const string ctrace::klogfile_setting("trace.file");


/*****************************************************************************
 * METHODS
 *****************************************************************************/

// ctor
//
// the file version, uses the filename passed in
ctrace::ctrace(const string &filename) :
  file(filename.c_str(), ios::out | ios::app), // only append!
  out(file)
{
//   assert(filename != "");

  // wipe the file at the beginning (since trace should append to it) 
  fstream wipe(filename.c_str(), ios::out | ios::trunc);
}


// acquire manipulator (parameterized)
// 
// only acquires the trace stream if tracing is on *and* the given module
// setting is true. declared as a friend of the ctrace class.
ctrace &operator<< (ctrace &trace, const acquire &a)
{
  trace.module = a.module;

  if (!trace.is_on(a.module))
    return trace;  // this trace module is turned off

  trace.out << a.module << ": ";
  return trace;
}
  


// release manipulator
// 
// releases trace and makes it available to other modules. declared as a friend
// of the ctrace class.
ctrace &operator<< (ctrace &trace, const release &r)
{
  if (!trace.is_on(trace.module))
    return trace;

  trace.module = "";
  trace.out << endl;
  return trace;
}



/*****************************************************************************
 * TEST MAIN
 *****************************************************************************/

/*
int main(int argc, char **argv)
{
  csettings::instance().set(ctrace::kmaster_setting, true);
//   csettings::instance().set(ctrace::klogfile_setting, "trace_test.log");

  ctrace::instance() << acquire() << "asdf ";
  ctrace::instance() << "this should print ";
  ctrace::instance() << 2.34 << " " << 3;

  double asdf = 2.999999999999999999999999999;
  ctrace::instance() << asdf;

  ctrace::instance() << " but this (below) shouldn't " << release();

  csettings::instance().set(ctrace::kmaster_setting, false);

  ctrace::instance() << acquire() << "you should not see this!!!" << release();


  csettings::instance().set(ctrace::kmaster_setting, true);

  ctrace::instance() << acquire() << "but this should!!!" << release();


  ctrace::instance() << acquire("trace_test") << "this should only print if "
                     << "the trace_test setting in the dotfile is true."
                     << release();
}
*/

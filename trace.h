/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file trace.h
 *
 * Defines the ctrace class, a general-purpose class designed to support
 * tracing output (aka logging) with iostreams. The ctrace class is derived
 * from ostream, so it implements the stream insertion operator (<<).
 *
 * Tracing can be turned on and off easily. Before it prints anything, ctrace
 * checks the return value from the is_on method, and only prints the trace
 * statement if is_on returns true. This method can be implemented any one of a
 * number of ways - right now it uses csettings to check whether the
 * tracing_output setting is true or false.
 *
 * Also, ctrace supports fine-grained control of tracing statements with module
 * settings. Before inserting anything into the ctrace stream, you must insert
 * a call to the parameterized manipulator acquire. This manipulator takes a
 * string, which is the name of a setting (called a module setting). This
 * setting is retrieved from csettings. If it's true, the tracing statement is
 * printed; otherwise, the stream insertion operator does nothing.
 *
 * For example, this tracing output will print "module1: hello 1.23" if the
 * setting "trace.module1" is true. Otherwise, nothing will be printed.
 *
 *  ctrace::instance() << acquire("module1") << "hello " << 1.23 << release();
 *
 * The ctrace class is implemented as a singleton. Use the instance method to
 * retrieve the ctrace instance.
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

#ifndef _TRACE_H
#define _TRACE_H

#include "settings.h"

#include <string>
#include <iostream>
#include <fstream>
using namespace std;


/*****************************************************************************
 * FORWARD DECLARATIONS
 *****************************************************************************/
class ctrace;
class acquire;
class release;
ctrace &operator<< (ctrace &trace, const acquire &a);
ctrace &operator<< (ctrace &trace, const release &r);


/*****************************************************************************
 * CTRACE CLASS
 *****************************************************************************/
class ctrace
{
  public:

  /** instance
   *
   * Returns the only instance of ctrace. This implements the singleton
   * pattern.
   */
  static ctrace &instance();

  /** operator<<
   *
   * Stream insertion operator. If the tracing stream is currently owned (as a
   * result of the acquire manipulator), inserts into cerr. Otherwise, does
   * nothing.
   */
  template <typename T>
  friend ctrace &operator<< (ctrace &trace, const T &elem);


  protected:
  // see below for acquire and release classes
  friend ctrace &operator<< (ctrace &trace, const acquire &a);
  friend ctrace &operator<< (ctrace &trace, const release &a);

  /** ctors, dtor
   */
  ctrace() : out(cerr) {}
  ctrace(const string &filename);
  ~ctrace() {}

  /** is_on
   *
   * Returns true if tracing output for the given module is turned on, false
   * if it's turned off. (If the master trace setting is turned off, is_off
   * always returns false.)
   */
  bool is_on(const string &module) const;


  /**************************************************************************
   * CONSTANTS AND MEMBERS
   **************************************************************************/
  protected:
  string module;  // module that currently owns ctrace; protected by mtx

  // !!! IMPORTANT !!! the filename ctor initializes out to refer to file, so
  // file *must* be declared before out here.
  fstream file;   // only opened if a log file is specified
  ostream &out;   // points to the log file if specified, cerr otherwise

  public:
  static const string kmaster_setting;
  static const string klogfile_setting;
};

/*****************************************************************************
 * ACQUIRE CLASS
 *****************************************************************************/
/** acquire
 *
 * A parameterized manipulator that takes the module name to print the tracing
 * statement under. The module name is a string that must be a setting
 * currently loaded into csettings. If that setting is true (1), the tracing
 * statement will be printed; otherwise, nothing will be printed. (The default
 * module name is the global trace setting.)
 *
 * If is_on() returns false, acquire does nothing.
 *
 * A call to the acquire manipulator is valid until the release manipulator
 * is called.
 *
 * NOTE: the acquire manipulator is *blocking* - if the module setting is
 * true, and another thread currently owns the ctrace instance, acquire
 * blocks until that thread releases the mutex.
 */
class acquire
{
  public:
  acquire(const string &module = "") : module(module) {}
  ~acquire() {}
  friend ctrace &operator<< (ctrace &trace, const acquire &a);

  protected:
  string module;
};


/*****************************************************************************
 * RELEASE CLASS
 *****************************************************************************/
/** release
 *
 * A manipulator that releases the ctrace instance - should only be called
 * after a call to acquire.
 */
class release
{
  public:
  release()  {}
  ~release()  {}
  friend ctrace &operator<< (ctrace &trace, const release &r);
};


/*****************************************************************************
 * TEMPLATIZED METHODS
 *****************************************************************************/
template <typename T>
ctrace &operator<< (ctrace &trace, const T &elem)
{
  if (!trace.is_on(trace.module))
    return trace;  // this trace module is turned off

  trace.out << elem;
  return trace;
}


/*****************************************************************************
 * INLINE METHODS
 *****************************************************************************/
// Note that, yes, there are actually *two* instances of ctrace, one that
// points to cerr and one that points to the log file, if any. only one is ever
// returned and used, though. 
inline ctrace &ctrace::instance()
{
  // by default, ctrace writes to cerr....
  static ctrace trace_err;

  // ...but if a log file is specified, use that instead
  static string filename = csettings::instance().get<string>(klogfile_setting);
  static ctrace trace_file(filename);

  if (filename == "")
    return trace_err;
  else
    return trace_file;
}


inline bool ctrace::is_on(const string &module) const
{
  if (!csettings::instance().get<bool>(kmaster_setting))
    return false;

  return (module == "" || csettings::instance().get<bool>("trace." + module));
}

#endif  // _TRACE_H



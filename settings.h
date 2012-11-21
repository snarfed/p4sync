/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005
 * Ryan Barrett <p4@ryanb.org>
 *
 * @file settings.h
 *
 * Defines the csettings class, which provides operations on settings files.
 * Key/value pairs can be stored in settings files and read at runtime, so
 * program settings can be changed without recompiling.
 *
 * Settings are stored in the settings file in the format:
 *
 * # <comment>
 * #
 * <key> = <value>
 *
 * where key is a string (without quotes or whitespace) and value is an
 * integer, a float, a boolean, or a string (again, without quotes or
 * whitespace). If value is a boolean, it should be "true" or "false" (without
 * quotes). Any line that begins with a # is considered a comment and is not
 * parsed. To enter boolean values, use 0 for false and 1 for true.
 *
 * The csettings class provides methods for looking up settings and adding
 * settings. The init file is read when the csettings instance is constructed,
 * but not written when the csettings instance is destroyed.
 *
 * This line shows how a static constant could be initialized w/csettings:
 *
 * static const int kexample = csettings::instance().get<int>("setting");
 *
 * The csettings class is implemented as a singleton. Use the instance method
 * to retrieve the csettings instance.
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

#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <iostream>
#include <sstream>
#include <string>
#include <map>
using namespace std;

/*****************************************************************************
 * TEST MAIN
 *****************************************************************************/
// only one of these can be defined at one time
//#define LKA_MAIN
//#define CONN_MGR_MAIN
//#define MSG_PING_MAIN
//#define RECEIVEQ_MAIN
//#define ID_TABLE_MAIN
//#define SERVICER_MAIN
//#define MSG_DISCOVER_MAIN
//#define P4IM_MAIN
//#define CONN_MAIN

/*****************************************************************************
 * GLOBAL FUNCTIONS
 *****************************************************************************/
/** sget
 *
 * A wrapper for csettings::instance().get() - provided solely to make static
 * initializer lines shorter.
 *
 * sigh...compiler issues...
 */
// template <typename S>
// S sget(const string &key);


/*****************************************************************************
 * CSETTINGS CLASS
 *****************************************************************************/
class csettings
{
  public:

  /** instance
   *
   * Returns the only instance of csettings. This implements the singleton
   * pattern.
   */
  static csettings &instance();

  /** get
   *
   * Returns the value associated with a given key. An assert is raised if the
   * given setting does not exist. There is an explicitly instantiated version
   * for bool that returns false if the setting does not exist.
   *
   * Note: this template should only be instantiated with the string, int,
   * float, and bool types.
   */
  template <typename T>
  T get(const string &key) const;

  /** set
   *
   * Sets the value associated with a given key. If the setting does not exist,
   * it is added.
   *
   * Note: this template should only be instantiated with the string, int,
   * float, and bool types.
   */
  template <typename T>
  void set(const string &key, const T &value);

  /** write
   *
   * Writes the current settings out to the settings file.
   */
  void write();

  protected:
  /** operator<<
   *
   * Stream insertion operator. Prints all of the key/value pairs, one pair
   * per line.
   */
  friend ostream &operator<<(ostream &ostr, const csettings &cs);

  /** ctor
   *
   * Reads in the settings file with the name stored in kfilename. Private so
   * that csettings can be implemented as a singleton.
   */
  csettings();

  /** dtor
   *
   */
  ~csettings();

  /** default
   *
   * Returns the default settings value, which will be returned if a setting
   * does not exist. This is specialized for certain types such as string, int,
   * and bool.
   */
  template <typename T>
  static T defvalue();

  /**************************************************************************
   * CONSTANTS AND MEMBERS
   **************************************************************************/
  const string kfilename;

  map<string, string> settings;
};



/*****************************************************************************
 * TEMPLATIZED METHODS
 *****************************************************************************/
template <typename T>
T csettings::get(const string &key) const
{
  stringstream s;
  s.setf(ios::boolalpha);      // this requires gcc 3 or greater

  // look up setting, use 0 if not found (this will be false for bool)
  map<string, string>::const_iterator found = settings.find(key);

  // not found, return default value for this type
  if (found == settings.end())
    return defvalue<T>();

  // convert to type T
  s << (*found).second;
  T ret;
  s >> ret;
  return ret;
}


template <typename T>
void csettings::set(const string &key, const T &value)
{
  stringstream s;
  string store_value;

  // convert to string
  s.setf(ios::boolalpha);      // this requires gcc 3 or greater
  s << value;
  s >> store_value;

  settings[key] = store_value;
}


/*****************************************************************************
 * INLINE METHODS
 *****************************************************************************/


inline csettings &csettings::instance()
{
  static csettings s;

  return s;
}


/*
template <typename S>
inline S sget(const string &key)
{
  return csettings::instance().get<S>(key);
}
*/

#endif  // _SETTINGS_H

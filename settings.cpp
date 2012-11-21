/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005
 * Ryan Barrett <p4@ryanb.org>
 *
 * @file settings.cpp
 *
 * Implements the csettings class. For more information, see csettings.h.
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

#include "settings.h"
#include "sys.h"

#include <cstdlib>
#include <fstream>
#include <cassert>
#include <algorithm>
using namespace std;

/*****************************************************************************
 * PUBLIC METHODS
 *****************************************************************************/
struct print_setting {
  print_setting(ostream &ostr) : ostr(ostr) {}

  void operator() (const pair<string, string> &elem) {
    ostr.width(35);
    ostr.fill(' ');
    ostr.flags(ios::left);
    ostr << elem.first.c_str();

    ostr << elem.second << endl;
  }

private:
  ostream &ostr;
};

ostream &operator<<(ostream &ostr, const csettings &cs)
{
  for_each(cs.settings.begin(), cs.settings.end(), print_setting(ostr));
  return ostr;
}

void csettings::write()
{
  // TODO: preserve order and comments in settings file
  ofstream file(kfilename.c_str());
  if (!file.is_open())
    return;

  for (map<string, string>::iterator it = settings.begin();
       it != settings.end(); it++)
    file << it->first << " = " << it->second << endl;
}


/*****************************************************************************
 * PRIVATE METHODS
 *****************************************************************************/
// explicit template specializations for defvalue(). (no default definition!)
template <> string csettings::defvalue() { return ""; }
template <> bool csettings::defvalue()   { return false; }
template <> uint csettings::defvalue()   { return 0; }
template <> ushort csettings::defvalue() { return defvalue<uint>(); }
template <> ulong csettings::defvalue()  { return defvalue<uint>(); }
template <> int csettings::defvalue()    { return defvalue<uint>(); }
template <> float csettings::defvalue()  { return defvalue<uint>(); }
template <> double csettings::defvalue() { return defvalue<uint>(); }


// ctor
csettings::csettings() : kfilename("p4sync.cfg")
{
  // these constants are here, instead of a static constant in the csettings
  // class, because it's used in other static initialization that happens
  // before the csettings class's static initialization
  static const char kcomment_char = '#', kequals_char  = '=';

  // p4sync default settings. if a settings file is found, its values will
  // override these values
  settings["trace.master"] = "true";
  settings["trace.client"] = "true";
  settings["trace.server"] = "true";
  settings["trace.player"] = "true";
  settings["trace.xmms"] = "false";
  settings["trace.zinf"] = "false";
  settings["trace.mpc"] = "true";
  settings["run_server"] = "false";
  settings["sync_to_host"] = "localhost";
  settings["sync_to_port"] = "9044";
  settings["sync_freq_ms"] = "30000";
  settings["server_port"] = "9044";
  settings["serve_freq_ms"] = "100";

  ifstream file(kfilename.c_str());
  if (!file.is_open())
    return;

  file.setf(ios::skipws);           // skip whitespace

  while (file.good()) {
    string key;

    file >> key;
    if (key[0] == kcomment_char) {  // file comment
      const int bufsize = 256;
      char buf[256];
      file.getline(buf, bufsize);
      continue;
    }

    char equals;
    string value;
    file >> equals >> value;        // new setting to add!
    if (equals == kequals_char && !key.empty() && !value.empty())
      settings[key] = value;
  }

}


csettings::~csettings()
{
//   cerr << "csettings dtor()" << endl;
}  



/*****************************************************************************
 * TEST MAIN
 *****************************************************************************/


// this line shows how a static constant could be initialized w/csettings
//
// static const int kexample = csettings::instance().get<int>("setting");

/*
int main(int argc, char **argv)
{
  csettings &s = csettings::instance();

  const string a = "a", b = "b", c = "c";

  s.set(a, string("asdf"));
  s.set(b, 1.23f);
  s.set(c, 4);

  cout.setf(ios::boolalpha);      // this requires gcc 3 or greater
  cout << "a's value is " << s.get<string>(a) << endl;
  cout << "b's value is " << s.get<float>(b) << endl;
  cout << "c's value is " << s.get<int>(c) << endl;
  cout << "d's value is " << s.get<string>("d") << endl;

  cout << "a's value as an int is " << s.get<int>(a) << endl;
  cout << "d's value as an int is " << s.get<int>("d") << endl;
  cout << "c's value as a bool is " << s.get<bool>("c") << endl;

  cout << "setting c's value to false..." << endl;
  s.set(c, false);
  cout << "c's value as a string is " << s.get<string>(c) << endl;
  cout << "c's value as a bool is " << s.get<bool>(c) << endl;

  cout << endl << "looking up values in dotfile \".p4\"..." << endl;
  cout << "test1's value is " << s.get<string>("test1") << endl;
  cout << "test2's value is " << s.get<string>("test2") << endl;
  cout << "test3's value is " << s.get<string>("test3") << endl;

//  cout << "a's value with sget is " << sget<string>(a) << endl;

  cout << "result of cout << csettings::instance() :" << endl
       << csettings::instance();

  cout << "unknown string setting: " << s.get<string>("foo") << endl;
  cout << "unknown bool setting: "   << s.get<bool>("foo") << endl;
  cout << "unknown uint setting: "   << s.get<uint>("foo") << endl;
  cout << "unknown ushort setting: " << s.get<ushort>("foo") << endl;
  cout << "unknown ulong setting: "  << s.get<ulong>("foo") << endl;
  cout << "unknown int setting: "    << s.get<int>("foo") << endl;
  cout << "unknown float setting: "  << s.get<float>("foo") << endl;
  cout << "unknown double setting: " << s.get<double>("foo") << endl;

  bool bar = s.get<bool>("foo");
  cout << (bar ? "true!" : "false!") << endl;

  s.write();

  return 0;
}*/

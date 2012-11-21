/* (PD) 2001 The Bitzi Corporation
 * Please see file COPYING or http://bitzi.com/publicdomain 
 * for more info.
 *
 * $Id: browser.h,v 1.4 2003/09/16 17:34:54 kgk Exp $
 */
#ifndef BROWSER_H
#define BROWSER_H
#include <string>

typedef enum
{
   eBrowserNetscape = 0,
   eBrowserMozilla,
   eBrowserKonqueror,
   eBrowserOpera,
   eBrowserLynx
} BrowserEnum;

void launch_browser(const char* url, BrowserEnum browser);
void launch_browser(const std::string& url, const std::string& browser);

#endif

/* arch-tag: 9c36aa82-c3e2-40ae-ac4f-83df122150c9
   (do not change this comment) */

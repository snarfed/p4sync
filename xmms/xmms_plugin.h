/**
 * p4sync
 * http://snarfed.org/p4sync
 *
 * Copyright 2005 Ryan Barrett <p4sync@ryanb.org>
 *
 * @file xmms_plugin.h
 *
 * Exports an accessor for the current xmms session.
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

#ifndef _XMMS_PLUGIN_H
#define _XMMS_PLUGIN_H


#ifdef __cplusplus
extern "C" {
#endif

/** get_xmms_session
 *
 * Returns the session id of the xmms instance that this plugin is running in.
 */
int get_xmms_session();


#ifdef __cplusplus
}
#endif

#endif  // _XMMS_PLUGIN_H

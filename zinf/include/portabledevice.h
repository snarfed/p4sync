/*____________________________________________________________________________
	
	Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)

	Portions Copyright (C) 1999 EMusic.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	
	$Id: portabledevice.h,v 1.5 2003/09/16 17:34:53 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PORTABLE_DEVICE_H_
#define INCLUDED_PORTABLE_DEVICE_H_

#include <string>
#include <vector>


#include <assert.h>

#include "config.h"
#include "errors.h"
#include "facontext.h"
#include "plmevent.h"

typedef enum {
    kConnection_Unknown, 
    kConnection_Parallel, 
    kConnection_USB
} ConnectionType;

class PortableDevice;

typedef PortableDevice* DeviceRef;

class PlaylistItem;

class DeviceInfo {
 public:

    DeviceInfo():m_ref(NULL),m_type(kConnection_Unknown),
                 m_bytesTotal(0),m_bytesUsed(0),m_port(0),
                 m_numEntries(0) {}
    
    virtual ~DeviceInfo() {}

    Error SetManufacturer(const char* manufacturer)
    {m_manufacturer = manufacturer; return kError_NoErr; }
    const char* GetManufacturer() const { return m_manufacturer.c_str(); }

    Error SetDevice(const char* device) 
    { m_device = device; return kError_NoErr; }
    const char* GetDevice() { return m_device.c_str(); }

    Error SetPluginName(const char* name) 
    { m_pluginName = name; return kError_NoErr; }
    const char* GetPluginName() { return m_pluginName.c_str(); }

    Error SetCapacity(uint32_t bytesTotal, uint32_t bytesUsed) 
    { m_bytesTotal = bytesTotal; m_bytesUsed = bytesUsed; return kError_NoErr; }
    void GetCapacity(uint32_t* bytesTotal, uint32_t* bytesUsed) 
    { if(bytesTotal) *bytesTotal = m_bytesTotal; if(bytesTotal) *bytesUsed = m_bytesUsed; }

    Error SetNumEntries(uint32_t count) { m_numEntries = count; return kError_NoErr; }
    uint32_t GetNumEntries() const { return m_numEntries; }

    Error SetConnectionType(ConnectionType type) { m_type = type; return kError_NoErr; }
    ConnectionType GetConnectionType() const { return m_type; }

    Error SetPortAddress(uint32_t port) { m_port = port; return kError_NoErr; }
    uint32_t GetPortAddress() const { return m_port; }

    Error SetRef(DeviceRef ref) { m_ref = ref; return kError_NoErr; }
    DeviceRef GetRef() { return m_ref; }

 private:
  
    std::string m_manufacturer;
    std::string m_device;
    DeviceRef m_ref;
    ConnectionType m_type;
    uint32_t m_bytesTotal;
    uint32_t m_bytesUsed;
    uint32_t m_port;
    uint32_t m_numEntries;
    std::string m_pluginName;
};

class PortableDevice {
 
 public:

    PortableDevice(FAContext *context) { m_context = context; }
    virtual ~PortableDevice() {}

    virtual Error GetSupportedDevices(DeviceInfo* device, uint32_t index) = 0;

    virtual bool IsDeviceAvailable(DeviceInfo* device) = 0;

    virtual Error GetDeviceInfo(DeviceInfo* device) = 0;

    virtual Error InitializeDevice(DeviceInfo* device, 
                                   PLMCallBackFunction function = NULL,
                                   void* cookie = NULL) = 0;

    virtual Error ReadPlaylist(DeviceInfo* device, 
                               std::vector<PlaylistItem*>* items,
                               PLMCallBackFunction function = NULL,
                               void* cookie = NULL) = 0;


    virtual Error WritePlaylist(DeviceInfo* device, 
                                std::vector<PlaylistItem*>* items,
                                PLMCallBackFunction function = NULL,
                                void* cookie = NULL) = 0;


    virtual Error DownloadSong(DeviceInfo* device, 
                               PlaylistItem* item,
                               const char* url,
                               PLMCallBackFunction function = NULL,
                               void* cookie = NULL) = 0;
  protected:
     FAContext *m_context;
};


#endif // _PORTABLE_DEVICE_H_

/* arch-tag: c6d15f3f-f746-4a7a-9bd3-c4e60cccf819
   (do not change this comment) */

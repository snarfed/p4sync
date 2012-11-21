/*____________________________________________________________________________
        
        Zinf - Zinf Is Not FreeA*p (The Free MP3 Player)

        Portions Copyright (C) 1998-1999 EMusic.com

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
        
        $Id: player.h,v 1.17 2004/02/02 17:39:35 kgk Exp $
____________________________________________________________________________*/

#ifndef INCLUDED_PLAYER_H_
#define INCLUDED_PLAYER_H_

#include <vector>
#include <map>
#include <string>

#include "config.h"
#include "thread.h"
#include "event.h"
#include "queue.h"
#include "ui.h"
#include "mutex.h"
#include "playlist.h"
#include "semaphore.h"
#include "registry.h"
#include "preferences.h"
#include "properties.h"
#include "propimpl.h"
#include "musiccatalog.h"
#ifdef EXPERIMENTAL
#include "musicdb.h"
#endif
#include "downloadmanager.h"
#include "timer.h"
#include "lmc.h"
#include "i18n.h"

//#ifndef WIN32
//#include "hashtable.h"
//#endif



/////////////////////
///   Player state
///
typedef enum
{
   PlayerState_Paused = 1,
   PlayerState_Playing,
   PlayerState_Stopped
}
PlayerState;

class FAContext;

////////////////////////
/// Player class representing the main class
///
class Player : public EventQueue
{

 public:
    static Player *GetPlayer(FAContext *context);
    virtual  ~Player();

    int32_t     RegisterActiveUI(UserInterface * ui);
    int32_t     RegisterLMCs(Registry * registry);
    int32_t     RegisterPMIs(Registry * registry);
    int32_t     RegisterPMOs(Registry * registry);
    int32_t     RegisterUIs(Registry * registry);

    Registry* GetLMCRegistry() const;
    Registry* GetPMIRegistry() const;
    ZINF_EXPORT Registry* GetPMORegistry() const;
    Registry*  GetUIRegistry() const;

    void      Run();
    bool      SetArgs(int32_t argc, char **argv);
    void      HandleSingleArg(char *arg);
    ZINF_EXPORT void      AddTheme(const char *url);
    void      SetTerminationSemaphore(Semaphore *);
    void      testQueue();
    static void EventServiceThreadFunc(void *);
    virtual Error AcceptEvent(Event *);
    virtual RegistryItem *ChooseLMC(const char *szUrl);
    virtual RegistryItem *ChoosePMI(const char *szUrl, char *szTitle = NULL);

    ZINF_EXPORT bool    IsSupportedExtension(const char *ext);
    char *GetExtension(const char *title);
 
    bool  IsSupportedProtocol(const char *proto);
    char *GetProtocol(const char *title);
 
    PlayerState  State() const { return m_playerState; }

    ZINF_EXPORT void UpdateCDNow();
    
 protected:
              Player(FAContext *context);
    void      GetUIManipLock();
    void      ReleaseUIManipLock();
    int32_t     CompareNames(const char *, const char *);
    void      SendToUI(Event *);
    void      Usage(const char *);

    bool         SetState(PlayerState);

    int32_t     ServiceEvent(Event *);
    void      CreatePMO(const PlaylistItem * pc, Event * pC);

    static void cd_timer(void* arg);
    void        CDTimer();

    FAContext *m_context;

 private:
    // These are event loop handling functions
    void DoneOutputting(Event *pEvent) ;
    void Stop(Event *pEvent);
    void GetVolume(Event *pEvent);
    void SetVolume(Event *pEvent);
    void ChangePosition(Event *pEvent);
    void GetMediaInfo(Event *pEvent) ;
    void Play(Event *pEvent);
    void Next(Event *pEvent);
    void Previous(Event *pEvent);
    void Pause(Event *pEvent);
    void UnPause(Event *pEvent);
    void TogglePause(Event *pEvent);
    int  Quit(Event *pEvent);
    int  ReadyToDieUI(Event *pEvent);
    void HandleMediaInfo(Event *pEvent);
    void HandleMediaTimeInfo(Event *pEvent);
    void SendEventToUI(Event *pEvent);
    void GetMediaTitle(Event *pEvent);
    void ToggleUI(Event *pEvent);
    void HandleQueryState();
    void SendEventToCatalog(Event *pEvent);   
    void HandlePrefsChanged(Event *pEvent);
    
    #define _EQUALIZER_ENABLE_
    #ifdef  _EQUALIZER_ENABLE_
    void Player::SetEQData(Event *pEvent);
    #endif // _EQUALIZER_ENABLE_
    #undef  _EQUALIZER_ENABLE_    

    #define _VISUAL_ENABLE_
    #ifdef  _VISUAL_ENABLE_
    void Player::SendVisBuf(Event *pEvent); 
    #endif // _VISUAL_ENABLE_
    #undef  _VISUAL_ENABLE_  

    Properties m_props;
    bool      m_didUsage;
    bool      m_autoplay, m_bTrackChanging;
    Semaphore *m_pTermSem;
    static Player *m_thePlayer;
    Semaphore *m_eventSem;
    PlayerState m_playerState;
    Queue < Event * >*m_eventQueue;
    Thread   *m_eventServiceThread;
    int32_t     m_quitWaitingFor;  // keeps track of how many CIO's 
    // and COO's haven't sent in 
    // their "Ready To Die" infos.

    Thread   *m_signatureThread;
    
    int32_t     m_imQuitting;
    std::vector < UserInterface * >*m_uiList;

    Mutex    *m_uiManipLock;
    Mutex    *m_lmcMutex;
    Mutex    *m_pmiMutex;
    Mutex    *m_pmoMutex;
    Mutex    *m_uiMutex;
    Mutex    *m_sigStopMutex;

    Semaphore *m_signatureSem;

    PlaylistManager *m_plm;

    PhysicalMediaOutput *m_sigspmo;
 
    PhysicalMediaOutput *m_pmo;
    LogicalMediaConverter *m_lmc;
    UserInterface *m_ui;

    Registry *m_lmcRegistry;
    Registry *m_pmiRegistry;
    Registry *m_pmoRegistry;
    Registry *m_uiRegistry;

    std::vector<std::string> m_argUIList;

    int32_t     m_argc;
    char    **m_argv;

    std::map<std::string, RegistryItem *> *m_lmcExtensions;
    std::map<std::string, RegistryItem *> *m_pmiProtocols;
    
    MusicCatalog *m_musicCatalog;
    MusicCollection *m_musicCollection;

    UserInterface *m_browserUI;

    DownloadManager *m_dlm;
    UserInterface *m_downloadUI;

    TimerRef m_cdTimer;
    bool     m_cdTimerActive;
    float    m_eqValues[32], m_eqPreamp;
    bool     m_eqEnabled;
};

#endif // _PLAYER_H_

/* arch-tag: edd83622-830f-4ed9-91a4-0e9c7336d245
   (do not change this comment) */

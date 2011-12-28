  #include <irrlicht.h>

#ifndef _I_RUNNER
  #define _I_RUNNER
  
namespace irr {
  namespace ode {
    class CIrrOdeManager;
  }
}

class IRunner {
  protected:
    IrrlichtDevice *m_pDevice;
    
    irr::video::IVideoDriver  *m_pDrv;
    irr::scene::ISceneManager *m_pSmgr;
    irr::gui::IGUIEnvironment *m_pGui;
    irr::gui::ICursorControl  *m_pCursor;
    
    ode::CIrrOdeManager *m_pOdeMngr;
  
  public:
    IRunner(const wchar_t *sTitle) {
      m_pDevice=createDevice(video::EDT_OPENGL,core::dimension2d<u32>(640,480),16,false,false,false,0);
      m_pDevice->setWindowCaption(sTitle);
      m_pDevice->setResizable(true);

      m_pDrv   =m_pDevice->getVideoDriver();
      m_pSmgr  =m_pDevice->getSceneManager();
      m_pGui   =m_pDevice->getGUIEnvironment();
      m_pCursor=m_pDevice->getCursorControl();
      
      m_pOdeMngr=irr::ode::CIrrOdeManager::getSharedInstance();
    }
    
    virtual ~IRunner() {
      //drop the world so it is destroyed
      m_pDevice->drop();
    }
    
    irr::IrrlichtDevice *getDevice() { return m_pDevice; }
    
    irr::video::IVideoDriver  *getVideoDriver   () { return m_pDrv   ; }
    irr::scene::ISceneManager *getSceneManager  () { return m_pSmgr  ; }
    irr::gui::IGUIEnvironment *getGuiEnvironment() { return m_pGui   ; }
    irr::gui::ICursorControl  *getCursorControl () { return m_pCursor; }
    
    virtual void run()=0;
    
    virtual void setEventReceiver(IEventReceiver *pReceiver)=0;
};

#endif

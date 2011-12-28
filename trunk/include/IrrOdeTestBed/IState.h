  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>

#ifndef _I_STATE
  #define _I_STATE
  
using namespace irr;

class IState {
  protected:
    IrrlichtDevice *m_pDevice;
    
    scene::ISceneManager *m_pSmgr;
    video::IVideoDriver  *m_pDrv;
    gui::IGUIEnvironment *m_pGui;
    
    ode::CIrrOdeManager *m_pOdeMngr;
    
    IRunner *m_pRunner;
    
    core::stringw m_sName,  /**<! name of the test */
                  m_sDescr; /**<! description of the test */
    
  public:
    IState(IrrlichtDevice *pDevice, IRunner *pRunner) {
      m_pDevice=pDevice;
      m_pRunner=pRunner;
      
      m_pSmgr=pDevice->getSceneManager  ();
      m_pDrv =pDevice->getVideoDriver   ();
      m_pGui =pDevice->getGUIEnvironment();
      
      m_pOdeMngr=ode::CIrrOdeManager::getSharedInstance();
    }
    
    virtual ~IState() { }
    
    virtual void activate()=0;
    virtual void deactivate()=0;
    
    virtual s32 update()=0;
    
    virtual const wchar_t *getTestName   () { return m_sName .c_str(); }
    virtual const wchar_t *getDescription() { return m_sDescr.c_str(); }
};

#endif

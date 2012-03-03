  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>

#ifndef _I_STATE
  #define _I_STATE

class IState {
  protected:
    irr::IrrlichtDevice *m_pDevice;

    irr::scene::ISceneManager *m_pSmgr;
    irr::video::IVideoDriver  *m_pDrv;
    irr::gui::IGUIEnvironment *m_pGui;

    irr::ode::CIrrOdeManager *m_pOdeMngr;

    IRunner *m_pRunner;

    irr::core::stringw m_sName,  /**<! name of the test */
                       m_sDescr; /**<! description of the test */

  public:
    IState(irr::IrrlichtDevice *pDevice, IRunner *pRunner) {
      m_pDevice=pDevice;
      m_pRunner=pRunner;

      m_pSmgr=pDevice->getSceneManager  ();
      m_pDrv =pDevice->getVideoDriver   ();
      m_pGui =pDevice->getGUIEnvironment();

      m_pOdeMngr=irr::ode::CIrrOdeManager::getSharedInstance();
    }

    virtual ~IState() { }

    virtual void activate()=0;
    virtual void deactivate()=0;

    virtual irr::s32 update()=0;

    virtual const wchar_t *getTestName   () { return m_sName .c_str(); }
    virtual const wchar_t *getDescription() { return m_sDescr.c_str(); }
};

#endif

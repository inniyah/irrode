#ifndef _C_TEST_HELI
  #define _C_TEST_HELI

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

using namespace irr; 

/**
 * @class CTestHeli
 * Test with a user-controlled helicopter
 */
class CTestHeli: public IState, public IEventReceiver, public ode::IIrrOdeEventListener {
  protected:
    ode::CIrrOdeBody *m_pObject;
    
    ode::CIrrOdeImpulseMotor *m_pMotor;
    ode::CIrrOdeTorqueMotor  *m_pControl;
    ode::CIrrOdeAeroDrag     *m_pAero;
    
    scene::ICameraSceneNode *m_pCam;
    
    gui::IGUIStaticText *m_pInfo;
    
    f32 m_fPitch,   /**!< pitch control position */
        m_fYaw,     /**<! yaw control position */
        m_fRoll,    /**<! roll control position */
        m_fThrust,  /**<! thrust */
        m_fDelta;   /**<! thrust control position */
    
  public:
    CTestHeli(IrrlichtDevice *pDevice, IRunner *pRunner);
    
    virtual ~CTestHeli() {
    }
    
    virtual void activate();
    virtual void deactivate();
    virtual s32 update();
    
    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
};

#endif


#ifndef _C_TEST_PLANE
  #define _C_TEST_PLANE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

using namespace irr; 

/**
 * @class CTestPlane
 * Test with a user-controlled airplane
 */
class CTestPlane: public IState, public IEventReceiver, public ode::IIrrOdeEventListener {
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
    CTestPlane(IrrlichtDevice *pDevice, IRunner *pRunner);
    
    virtual ~CTestPlane() {
    }
    
    virtual void activate();
    virtual void deactivate();
    virtual s32 update();
    
    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
};

#endif


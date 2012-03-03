#ifndef _C_TEST_HELI
  #define _C_TEST_HELI

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestHeli
 * Test with a user-controlled helicopter
 */
class CTestHeli: public IState, public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  protected:
    irr::ode::CIrrOdeBody *m_pObject;

    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::ode::CIrrOdeTorqueMotor  *m_pControl;
    irr::ode::CIrrOdeAeroDrag     *m_pAero;

    irr::scene::ICameraSceneNode *m_pCam;

    irr::gui::IGUIStaticText *m_pInfo;

    irr::f32 m_fPitch,   /**!< pitch control position */
             m_fYaw,     /**<! yaw control position */
             m_fRoll,    /**<! roll control position */
             m_fThrust,  /**<! thrust */
             m_fDelta;   /**<! thrust control position */

  public:
    CTestHeli(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestHeli() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();

    virtual bool OnEvent(const irr::SEvent &event);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif


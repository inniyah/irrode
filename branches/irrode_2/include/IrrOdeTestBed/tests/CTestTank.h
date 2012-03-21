#ifndef _C_TEST_TANK
  #define _C_TEST_TANK

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestTank
 * Test with a user-controlled car
 */
class CTestTank: public IState, public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  protected:
    irr::ode::CIrrOdeBody *m_pObject;
    irr::ode::CIrrOdeMotor *m_pEngine[4];

    irr::scene::ICameraSceneNode *m_pCam;

    irr::gui::IGUIStaticText *m_pInfo;

    irr::f32 m_fPower,
             m_fSteer;

  public:
    CTestTank(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestTank() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();

    virtual bool OnEvent(const irr::SEvent &event);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif




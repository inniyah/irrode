#ifndef _C_TEST_CAR
  #define _C_TEST_CAR

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestCar
 * Test with a user-controlled car
 */
class CTestCar: public IState, public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  protected:
    irr::ode::CIrrOdeBody *m_pObject;

    irr::ode::CIrrOdeMotor *m_pEngine[2];
    irr::ode::CIrrOdeServo *m_pSteer[2];

    irr::scene::ICameraSceneNode *m_pCam;

    irr::gui::IGUIStaticText *m_pInfo;

  public:
    CTestCar(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestCar() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();

    virtual bool OnEvent(const irr::SEvent &event);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif



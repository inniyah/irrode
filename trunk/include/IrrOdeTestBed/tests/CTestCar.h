#ifndef _C_TEST_CAR
  #define _C_TEST_CAR

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

using namespace irr; 

/**
 * @class CTestCar
 * Test with a user-controlled car
 */
class CTestCar: public IState, public IEventReceiver, public ode::IIrrOdeEventListener {
  protected:
    ode::CIrrOdeBody *m_pObject;
    
    ode::CIrrOdeMotor *m_pEngine[2];
    ode::CIrrOdeServo *m_pSteer[2];
    
    scene::ICameraSceneNode *m_pCam;
    
    gui::IGUIStaticText *m_pInfo;
    
  public:
    CTestCar(IrrlichtDevice *pDevice, IRunner *pRunner);
    
    virtual ~CTestCar() {
    }
    
    virtual void activate();
    virtual void deactivate();
    virtual s32 update();
    
    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
};

#endif



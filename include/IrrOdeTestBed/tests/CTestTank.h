#ifndef _C_TEST_TANK
  #define _C_TEST_TANK

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

using namespace irr; 

/**
 * @class CTestTank
 * Test with a user-controlled car
 */
class CTestTank: public IState, public IEventReceiver, public ode::IIrrOdeEventListener {
  protected:
    ode::CIrrOdeBody *m_pObject;
    ode::CIrrOdeMotor *m_pEngine[4];
    
    scene::ICameraSceneNode *m_pCam;
    
    gui::IGUIStaticText *m_pInfo;
    
    f32 m_fPower,
        m_fSteer;
    
  public:
    CTestTank(IrrlichtDevice *pDevice, IRunner *pRunner);
    
    virtual ~CTestTank() {
    }
    
    virtual void activate();
    virtual void deactivate();
    virtual s32 update();
    
    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
};

#endif




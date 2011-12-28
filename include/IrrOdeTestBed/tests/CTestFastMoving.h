#ifndef _C_TEST_FAST_MOVING
  #define _C_TEST_FAST_MOVING

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

using namespace irr; 

/**
 * @class CTestFastMoving
 * Test with a fast moving cannonball
 */
class CTestFastMoving : public IState, public IEventReceiver, public ode::IIrrOdeEventListener {
  protected:
    typedef struct SCannonBall {
      s32 iTtl;
      ode::CIrrOdeBody *pBody;
    }
    SCannonBall;
    
    gui::IGUIButton *m_pFire,
                    *m_pFireFast;
    
    ode::CIrrOdeBody  *m_pTemplate;
    ode::CIrrOdeWorld *m_pWorld;
    
    core::array<SCannonBall *> m_aBalls;
    
    void fire(bool bFast);
    
  public:
    CTestFastMoving(IrrlichtDevice *pDevice, IRunner *pRunner);
    
    virtual ~CTestFastMoving() {
    }
    
    virtual void activate();
    virtual void deactivate();
    virtual s32 update();
    
    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
};

#endif

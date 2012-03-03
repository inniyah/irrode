#ifndef _C_TEST_FAST_MOVING
  #define _C_TEST_FAST_MOVING

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestFastMoving
 * Test with a fast moving cannonball
 */
class CTestFastMoving : public IState, public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  protected:
    typedef struct SCannonBall {
      irr::s32 iTtl;
      irr::ode::CIrrOdeBody *pBody;
    }
    SCannonBall;

    irr::gui::IGUIButton *m_pFire,
                         *m_pFireFast;

    irr::ode::CIrrOdeBody  *m_pTemplate;
    irr::ode::CIrrOdeWorld *m_pWorld;

    irr::core::array<SCannonBall *> m_aBalls;

    void fire(bool bFast);

  public:
    CTestFastMoving(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestFastMoving() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();

    virtual bool OnEvent(const irr::SEvent &event);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

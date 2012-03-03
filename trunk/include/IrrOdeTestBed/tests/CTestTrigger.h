#ifndef _C_TEST_TRIGGER
  #define _C_TEST_TRIGGER

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestTrigger
 * Test for the trigger feature
 */
class CTestTrigger : public IState, public irr::ode::IIrrOdeEventListener {
  protected:
    bool m_bTrigger[3];
    irr::gui::IGUIStaticText *m_lblTrigger[3];

  public:
    CTestTrigger(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestTrigger() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif


#ifndef _C_TEST_TRIGGER
  #define _C_TEST_TRIGGER

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

using namespace irr;

/**
 * @class CTestTrigger
 * Test for the trigger feature
 */
class CTestTrigger : public IState, public ode::IIrrOdeEventListener {
  protected:
    bool m_bTrigger[3];
    gui::IGUIStaticText *m_lblTrigger[3];

  public:
    CTestTrigger(IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestTrigger() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual s32 update();

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
};

#endif


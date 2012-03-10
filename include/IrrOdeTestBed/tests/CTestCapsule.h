#ifndef _C_TEST_CAPSULE
  #define _C_TEST_CAPSULE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestCapsule
 * Load test with a capsule
 */
class CTestCapsule : public IState {
  public:
    CTestCapsule(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestCapsule() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();
};

#endif



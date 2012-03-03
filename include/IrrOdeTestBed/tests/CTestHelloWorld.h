#ifndef _C_TEST_HELLO_WORLD
  #define _C_TEST_HELLO_WORLD

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestHelloWorld
 * A simple test, just a sphere bouncing off a static box
 */
class CTestHelloWorld : public IState {
  public:
    CTestHelloWorld(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestHelloWorld() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();
};

#endif

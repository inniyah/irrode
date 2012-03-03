#ifndef _C_TEST_BOX_PILE
  #define _C_TEST_BOX_PILE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestBoxPile
 * Load test with a pile of boxes
 */
class CTestBoxPile : public IState {
  public:
    CTestBoxPile(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestBoxPile() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();
};

#endif


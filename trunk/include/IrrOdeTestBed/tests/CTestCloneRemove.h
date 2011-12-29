#ifndef _C_TEST_CLONE_REMOVE
  #define _C_TEST_CLONE_REMOVE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

using namespace irr;

/**
 * @class CTestCloneRemove
 * Test clone and remove functionality
 */
class CTestCloneRemove : public IState, public IEventReceiver {
  protected:
    gui::IGUIButton *m_btnRemoveStatic,
                    *m_btnRemoveDynamic,
                    *m_btnClone,
                    *m_btnRemovecloned;

    core::list<ode::CIrrOdeBody *> m_lCloned;

    ode::CIrrOdeWorld *m_pWorld;
    ode::CIrrOdeBody  *m_pTemplate;
    scene::ISceneNode *m_pStatic,
                      *m_pDynamic;
  public:
    CTestCloneRemove(IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestCloneRemove() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual s32 update();

    virtual bool OnEvent(const SEvent &event);
};

#endif



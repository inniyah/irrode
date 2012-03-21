#ifndef _C_TEST_CLONE_REMOVE
  #define _C_TEST_CLONE_REMOVE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

/**
 * @class CTestCloneRemove
 * Test clone and remove functionality
 */
class CTestCloneRemove : public IState, public irr::IEventReceiver {
  protected:
    irr::gui::IGUIButton *m_btnRemoveStatic,
                         *m_btnRemoveDynamic,
                         *m_btnClone,
                         *m_btnRemovecloned;

    irr::core::list<irr::ode::CIrrOdeBody *> m_lCloned;

    irr::ode::CIrrOdeWorld *m_pWorld;
    irr::ode::CIrrOdeBody  *m_pTemplate;
    irr::scene::ISceneNode *m_pStatic,
                           *m_pDynamic;
  public:
    CTestCloneRemove(irr::IrrlichtDevice *pDevice, IRunner *pRunner);

    virtual ~CTestCloneRemove() {
    }

    virtual void activate();
    virtual void deactivate();
    virtual irr::s32 update();

    virtual bool OnEvent(const irr::SEvent &event);
};

#endif



  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

  #include <CMenu.h>
  #include <tests/CTestHelloWorld.h>
  #include <tests/CTestBoxPile.h>
  #include <tests/CTestFastMoving.h>
  #include <tests/CTestHeli.h>
  #include <tests/CTestPlane.h>
  #include <tests/CTestCar.h>
  #include <tests/CTestTank.h>
  #include <tests/CTestTrigger.h>
  #include <tests/CTestCloneRemove.h>
  #include <tests/CTestSlipstream.h>

class CRunner : public IRunner, public irr::IEventReceiver {
  protected:
    irr::core::array<IState *> m_aStates;
    irr::u32 m_iActive;
    irr::s32 m_iRet;

    irr::IEventReceiver *m_pReceiver;

  public:
    CRunner() : IRunner(L"IrrOde Testbed") {
      m_pReceiver=NULL;
      m_iActive=0;
      m_iRet=0;

      m_aStates.push_back(new CMenuState      (m_pDevice,this,&m_aStates));
      m_aStates.push_back(new CTestHelloWorld (m_pDevice,this));
      m_aStates.push_back(new CTestCloneRemove(m_pDevice,this));
      m_aStates.push_back(new CTestBoxPile    (m_pDevice,this));
      m_aStates.push_back(new CTestTrigger    (m_pDevice,this));
      m_aStates.push_back(new CTestSlipstream (m_pDevice,this));
      //m_aStates.push_back(new CTestFastMoving (m_pDevice,this));
      m_aStates.push_back(new CTestHeli       (m_pDevice,this));
      m_aStates.push_back(new CTestPlane      (m_pDevice,this));
      m_aStates.push_back(new CTestCar        (m_pDevice,this));
      m_aStates.push_back(new CTestTank       (m_pDevice,this));

      m_pDevice->setEventReceiver(this);
    }

    virtual ~CRunner() {
    }

    virtual irr::s32 update() {
      return m_iRet;
    }

    virtual void run() {
      m_pOdeMngr->install(m_pDevice);

      irr::ode::CIrrOdeSceneNodeFactory *pFactory=new irr::ode::CIrrOdeSceneNodeFactory(m_pSmgr);
      m_pSmgr->registerSceneNodeFactory(pFactory);

      m_aStates[m_iActive]->activate();

      //let's run the loop
      while(m_pDevice->run()) {
        irr::s32 iRet=m_aStates[m_iActive]->update();
        if (iRet!=0) {
          m_aStates[m_iActive]->deactivate();
          m_iActive=iRet;
          m_aStates[m_iActive]->activate();
        }

        //now for the normal Irrlicht stuff ... begin, draw and end scene and update window caption
        m_pDrv->beginScene(true, true, irr::video::SColor(0,128,128,160));

        m_pSmgr->drawAll();
        m_pGui->drawAll();

        m_pDrv->endScene();
      }

      m_aStates[m_iActive]->deactivate();
    }

    virtual bool OnEvent(const irr::SEvent &event) {
      bool bRet=false;

      if (event.EventType==irr::EET_KEY_INPUT_EVENT) {
        if (!event.KeyInput.PressedDown && event.KeyInput.Key==irr::KEY_ESCAPE && m_iActive!=0) {
          m_aStates[m_iActive]->deactivate();
          m_iActive=0;
          m_aStates[m_iActive]->activate();
        }
      }

      if (!bRet && m_pReceiver!=NULL) bRet=m_pReceiver->OnEvent(event);
      return bRet;
    }

    virtual void setEventReceiver(IEventReceiver *pReceiver) {
      m_pReceiver=pReceiver;
    }

    virtual const wchar_t *getTestName   () { return NULL; }
    virtual const wchar_t *getDescription() { return NULL; }
};

int main(int argc, char** argv) {
  IRunner *pRunner=new CRunner();
  pRunner->run();
  delete pRunner;
  return 0;
}


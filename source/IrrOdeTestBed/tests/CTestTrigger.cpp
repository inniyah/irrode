  #include <tests/CTestTrigger.h>

CTestTrigger::CTestTrigger(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Trigger Test";
  m_sDescr=L"Test of the trigger message sending.";
}

void CTestTrigger::activate() {
  m_lblTrigger[0]=m_pGui->addStaticText(L"Trigger1",irr::core::rect<irr::s32>(10,10,110,30),true,true,NULL,-1,true);
  m_lblTrigger[1]=m_pGui->addStaticText(L"Trigger2",irr::core::rect<irr::s32>(10,40,110,60),true,true,NULL,-1,true);
  m_lblTrigger[2]=m_pGui->addStaticText(L"Trigger3",irr::core::rect<irr::s32>(10,70,110,90),true,true,NULL,-1,true);
  m_pSmgr->loadScene("../../data/scenes/Testbed_trigger.xml");

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();
  m_pOdeMngr->getQueue()->addEventListener(this);

  irr::scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNode();
  cam->setPosition(irr::core::vector3df(-20,10,-20));
  cam->setTarget(irr::core::vector3df(0,0,0));

  m_bTrigger[0]=false;
  m_bTrigger[1]=false;
  m_bTrigger[2]=false;
}

void CTestTrigger::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
  m_pOdeMngr->getQueue()->removeEventListener(this);
  m_pGui->clear();
}

irr::s32 CTestTrigger::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}

bool CTestTrigger::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    for (irr::u32 i=0; i<3; i++) {
      if (m_bTrigger[i])
        m_lblTrigger[i]->setBackgroundColor(irr::video::SColor(0xFF,0,192,0));
      else
        m_lblTrigger[i]->setBackgroundColor(irr::video::SColor(0xFF,192,0,0));

      m_bTrigger[i]=false;
    }
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventTrigger) {
    irr::ode::CIrrOdeEventTrigger *p=(irr::ode::CIrrOdeEventTrigger *)pEvent;
    m_bTrigger[p->getTriggerId()-1]=true;
  }

  return false;
}

bool CTestTrigger::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventTrigger || pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

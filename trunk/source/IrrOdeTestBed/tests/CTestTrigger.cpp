  #include <tests/CTestTrigger.h>

CTestTrigger::CTestTrigger(IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Trigger Test";
  m_sDescr=L"Test of the trigger message sending.";
}

void CTestTrigger::activate() {
  m_lblTrigger[0]=m_pGui->addStaticText(L"Trigger1",core::rect<s32>(10,10,110,30),true,true,NULL,-1,true);
  m_lblTrigger[1]=m_pGui->addStaticText(L"Trigger2",core::rect<s32>(10,40,110,60),true,true,NULL,-1,true);
  m_lblTrigger[2]=m_pGui->addStaticText(L"Trigger3",core::rect<s32>(10,70,110,90),true,true,NULL,-1,true);
  m_pSmgr->loadScene("../../data/scenes/Testbed_trigger.xml");

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();
  m_pOdeMngr->getQueue()->addEventListener(this);

  scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNode();
  cam->setPosition(vector3df(-20,10,-20));
  cam->setTarget(vector3df(0,0,0));

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

s32 CTestTrigger::update() {
  s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}

bool CTestTrigger::onEvent(ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==ode::eIrrOdeEventStep) {
    for (u32 i=0; i<3; i++) {
      if (m_bTrigger[i])
        m_lblTrigger[i]->setBackgroundColor(video::SColor(0xFF,0,192,0));
      else
        m_lblTrigger[i]->setBackgroundColor(video::SColor(0xFF,192,0,0));

      m_bTrigger[i]=false;
    }
  }

  if (pEvent->getType()==ode::eIrrOdeEventTrigger) {
    ode::CIrrOdeEventTrigger *p=(ode::CIrrOdeEventTrigger *)pEvent;
    m_bTrigger[p->getTriggerId()-1]=true;
  }

  return false;
}

bool CTestTrigger::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==ode::eIrrOdeEventTrigger || pEvent->getType()==ode::eIrrOdeEventStep;
}

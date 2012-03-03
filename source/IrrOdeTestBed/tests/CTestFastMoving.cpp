  #include <tests/CTestFastMoving.h>

CTestFastMoving::CTestFastMoving(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Fast Moving";
  m_sDescr=L"Test with a fast moving cannonball.";
}

void CTestFastMoving::activate() {
  m_pSmgr->loadScene("../../data/scenes/Testbed_fast.xml");

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  irr::scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNode();
  cam->setPosition(irr::core::vector3df(20.0f,15.0f,20.0f));
  cam->setTarget(irr::core::vector3df(0,0,0));

  m_pFire    =m_pGui->addButton(irr::core::rect<irr::s32>(10,10,120,30),NULL,1,L"Fire"     );
  m_pFireFast=m_pGui->addButton(irr::core::rect<irr::s32>(10,35,120,55),NULL,1,L"Fire Fast");

  m_pTemplate=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("CannonBall"));

  irr::core::list<irr::ode::CIrrOdeWorld *> lWorlds=m_pOdeMngr->getWorlds();
  m_pWorld=*(lWorlds.begin());

  printf("cannonball template: %i\nworld: %i\n",(int)m_pTemplate,(int)m_pWorld);

  m_pOdeMngr->getQueue()->addEventListener(this);
  m_pRunner->setEventReceiver(this);
}

void CTestFastMoving::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
  m_pOdeMngr->getQueue()->removeEventListener(this);

  m_pRunner->setEventReceiver(NULL);
}

irr::s32 CTestFastMoving::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}

void CTestFastMoving::fire(bool bFast) {
  SCannonBall *pBall=new SCannonBall();
  pBall->pBody=(irr::ode::CIrrOdeBody *)m_pOdeMngr->cloneTree(m_pTemplate,m_pWorld,m_pSmgr);
  pBall->pBody->setPosition(irr::core::vector3df(20.0f,5.0f,0.0f));
  pBall->pBody->setLinearVelocity(irr::core::vector3df(-150.0f,2.0f,0.0f));
  if (bFast) pBall->pBody->setIsFastMoving(true);
  pBall->pBody->initPhysics();
  pBall->iTtl=300;
  m_aBalls.push_back(pBall);
}

bool CTestFastMoving::OnEvent(const irr::SEvent &event) {
  if (event.EventType==irr::EET_GUI_EVENT) {
    if (event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED) {
      if (event.GUIEvent.Caller==m_pFire    ) fire(false);
      if (event.GUIEvent.Caller==m_pFireFast) fire(true );
    }
  }
  return false;
}

bool CTestFastMoving::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    for (irr::s32 i=m_aBalls.size()-1; i>=0; i--) {
      SCannonBall *p=m_aBalls[i];
      p->iTtl--;
      if (p->iTtl<=0) {
        m_aBalls.erase(i);
        //m_pOdeMngr->removeTreeFromPhysics(p->pBody);
        p->pBody->removeFromPhysics();
        m_pSmgr->addToDeletionQueue(p->pBody);
        delete p;
      }
    }
  }
  return false;
}

bool CTestFastMoving::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

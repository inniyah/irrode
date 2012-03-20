  #include <tests/CTestSlipstream.h>

CTestSlipstream::CTestSlipstream(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Slipstream Test";
  m_sDescr=L"Test of slipstream functionality.";
}

void CTestSlipstream::activate() {
  m_pSmgr->loadScene("../../data/scenes/Testbed_slipstream.xml");

  m_pWorld = reinterpret_cast<irr::ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("world"));
  printf("world=%i\n",(int)m_pWorld);

  m_pMarble=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("marble"));
  printf("marble=%i\n",(int)m_pMarble);

  irr::core::list<irr::ode::CIrrOdeSceneNode *> lNodes=m_pWorld->getIrrOdeNodes();
  irr::core::list<irr::ode::CIrrOdeSceneNode *>::Iterator it;
  for (it=lNodes.begin(); it!=lNodes.end(); it++) {
    if ((*it)->getType()==irr::ode::IRR_ODE_BODY_ID) {
      m_lBodies.push_back(reinterpret_cast<irr::ode::CIrrOdeBody *>(*it));
    }
  }

  printf("%i ode bodies found\n",m_lBodies.size());

  m_iCurrent=0;
  for (irr::u32 i=0; i<30; i++)
    m_aSlipStream[i]=new CSlipStream(m_pMarble,30,0.0125f,0.55f,m_lBodies);

  //init the ODE
  m_pOdeMngr->getQueue()->addEventListener(this);
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  irr::scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNodeFPS();
  cam->setPosition(irr::core::vector3df(-20,10,-20));
  cam->setTarget(irr::core::vector3df(0,0,0));
}

void CTestSlipstream::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->closeODE();
  m_pOdeMngr->getQueue()->removeEventListener(this);
  m_pGui->clear();
}

irr::s32 CTestSlipstream::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}

bool CTestSlipstream::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    m_aSlipStream[m_iCurrent++]->init();
    if (m_iCurrent>=30) m_iCurrent=0;
    for (irr::u32 i=0; i<30; i++) {
      m_aSlipStream[i]->step();
    }
  }

  return false;
}

bool CTestSlipstream::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventBodyMoved || pEvent->getType()==irr::ode::eIrrOdeEventStep;
}


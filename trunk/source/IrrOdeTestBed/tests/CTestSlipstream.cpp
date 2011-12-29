  #include <tests/CTestSlipstream.h>

CTestSlipstream::CTestSlipstream(IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Slipstream Test";
  m_sDescr=L"Test of slipstream functionality.";
}

void CTestSlipstream::activate() {
  m_pSmgr->loadScene("../../data/scenes/Testbed_slipstream.xml");

  m_pMarble=reinterpret_cast<ode::CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("marble"));
  printf("marble=%i\n",(int)m_pMarble);

  core::list<ode::CIrrOdeSceneNode *> lNodes=m_pOdeMngr->getIrrOdeNodes();
  core::list<ode::CIrrOdeSceneNode *>::Iterator it;
  for (it=lNodes.begin(); it!=lNodes.end(); it++) {
    if ((*it)->getType()==ode::IRR_ODE_BODY_ID) {
      m_lBodies.push_back(reinterpret_cast<ode::CIrrOdeBody *>(*it));
    }
  }

  printf("%i ode bodies found\n",m_lBodies.size());

  m_iCurrent=0;
  for (u32 i=0; i<30; i++)
    m_aSlipStream[i]=new CSlipStream(m_pMarble,30,0.0125f,0.55f,m_lBodies);

  //init the ODE
  m_pOdeMngr->getQueue()->addEventListener(this);
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNodeFPS();
  cam->setPosition(vector3df(-20,10,-20));
  cam->setTarget(vector3df(0,0,0));
}

void CTestSlipstream::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
  m_pOdeMngr->getQueue()->removeEventListener(this);
  m_pGui->clear();
}

s32 CTestSlipstream::update() {
  s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}

bool CTestSlipstream::onEvent(ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==ode::eIrrOdeEventStep) {
    m_aSlipStream[m_iCurrent++]->init();
    if (m_iCurrent>=30) m_iCurrent=0;
    for (u32 i=0; i<30; i++) {
      m_aSlipStream[i]->step();
    }
  }

  return false;
}

bool CTestSlipstream::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==ode::eIrrOdeEventBodyMoved || pEvent->getType()==ode::eIrrOdeEventStep;
}


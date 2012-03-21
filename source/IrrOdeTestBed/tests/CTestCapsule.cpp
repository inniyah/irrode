  #include <tests/CTestCapsule.h>

CTestCapsule::CTestCapsule(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Capsule";
  m_sDescr=L"This is a test with a capsule.";
}

void CTestCapsule::activate() {
  m_pSmgr->loadScene("../../data/scenes/Testbed_capsule.xml");

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  irr::scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNode();
  cam->setPosition(irr::core::vector3df(10.0f,5.0f,10.0f));
  cam->setTarget(irr::core::vector3df(0,0,0));
}

void CTestCapsule::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
}

irr::s32 CTestCapsule::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}




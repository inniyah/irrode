  #include <tests/CTestBoxPile.h>

CTestBoxPile::CTestBoxPile(IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Pile of Boxes";
  m_sDescr=L"This is a load test with a pile of boxes.";
}

void CTestBoxPile::activate() {
  m_pSmgr->loadScene("../../data/scenes/Testbed_boxpile.xml");

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNode();
  cam->setPosition(core::vector3df(10.0f,5.0f,10.0f));
  cam->setTarget(core::vector3df(0,0,0));
}

void CTestBoxPile::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
}

s32 CTestBoxPile::update() {
  s32 iRet=0;
  
  //step the simulation
  m_pOdeMngr->step();
  
  return iRet;
}



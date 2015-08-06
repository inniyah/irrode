  #include <tests/CTestBoxPile.h>

CTestBoxPile::CTestBoxPile(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Pile of Boxes";
  m_sDescr=L"This is a load test with a pile of boxes.";
}

void CTestBoxPile::activate() {
  m_pSmgr->loadScene(DATADIR "/scenes/Testbed_boxpile.xml");

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  irr::scene::ICameraSceneNode *cam=m_pSmgr->addCameraSceneNode();
  cam->setPosition(irr::core::vector3df(10.0f,5.0f,10.0f));
  cam->setTarget(irr::core::vector3df(0,0,0));
}

void CTestBoxPile::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->closeODE();
}

irr::s32 CTestBoxPile::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}



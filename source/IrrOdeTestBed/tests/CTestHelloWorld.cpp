  #include <tests/CTestHelloWorld.h>

CTestHelloWorld::CTestHelloWorld(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Hello World";
  m_sDescr=L"Just a simple test with a sphere bouncing of a box (formerly this was the \"HelloOdeWorld\" application).";
}

void CTestHelloWorld::activate() {
  m_pSmgr->loadScene(DATADIR "/IrrOdEdit_example.xml");

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  irr::scene::ICameraSceneNode *cam=reinterpret_cast<irr::scene::ICameraSceneNode *>(m_pSmgr->getSceneNodeFromName("theCamera"));
  cam->setTarget(irr::core::vector3df(0,0,0));
}

void CTestHelloWorld::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->closeODE();
}

irr::s32 CTestHelloWorld::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  return iRet;
}


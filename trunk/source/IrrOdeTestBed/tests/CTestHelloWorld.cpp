  #include <tests/CTestHelloWorld.h>

CTestHelloWorld::CTestHelloWorld(IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Hello World";
  m_sDescr=L"Just a simple test with a sphere bouncing of a box (formerly this was the \"HelloOdeWorld\" application).";
}

void CTestHelloWorld::activate() {
  m_pSmgr->loadScene("../../data/IrrOdEdit_example.xml");

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();

  scene::ICameraSceneNode *cam=reinterpret_cast<ICameraSceneNode *>(m_pSmgr->getSceneNodeFromName("theCamera"));
  cam->setTarget(vector3df(0,0,0));
}

void CTestHelloWorld::deactivate() {
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
}

s32 CTestHelloWorld::update() {
  s32 iRet=0;
  
  //step the simulation
  m_pOdeMngr->step();
  
  return iRet;
}


  #include <CController.h>
  #include <irrCC.h>

CController::CController(IrrlichtDevice *pDevice, CIrrCC *pCtrl) : CIrrOdeCarState(pDevice,L"Controller Setup","../../data/irrOdeCarControllerHelp.txt",pCtrl) {
  m_pTabCtrl=pDevice->getGUIEnvironment()->addTabControl(rect<s32>(5,5,635,595),NULL,true,true,-1);
  IGUITab *pTab[4];

  pTab[0]=m_pTabCtrl->addTab(L"Car");
  pTab[1]=m_pTabCtrl->addTab(L"Tank");
  pTab[2]=m_pTabCtrl->addTab(L"Helicopter/Plane");

  m_pTabCtrl->setVisible(false);

  pCtrl->addGui(0,pTab[0],position2di(15,15));
  pCtrl->addGui(1,pTab[1],position2di(15,15));
  pCtrl->addGui(2,pTab[2],position2di(15,15));
}

CController::~CController() {
}

void CController::activate() {
  m_bActive=true;
  m_pDevice->setEventReceiver(this);
  m_pDevice->getCursorControl()->setVisible(false);
  m_bSwitchToMenu=false;
  m_pTabCtrl->setVisible(true);
  m_pDevice->getCursorControl()->setVisible(true);
}

void CController::deactivate() {
  m_bActive=false;
  m_pTabCtrl->setVisible(false);
  m_pDevice->getCursorControl()->setVisible(false);
}

u32 CController::update() {
  u32 iRet=CIrrOdeCarState::update();
  return iRet;
}

bool CController::OnEvent(const SEvent &event) {
  return CIrrOdeCarState::OnEvent(event) || m_pController->ConfigEvent(event);
}


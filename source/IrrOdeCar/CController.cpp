  #include <CController.h>
  #include <irrCC.h>

CController::CController(irr::IrrlichtDevice *pDevice, CIrrCC *pCtrl) : CIrrOdeCarState(pDevice,L"Controller Setup","../../data/irrOdeCarControllerHelp.txt",pCtrl) {
  m_pTabCtrl=pDevice->getGUIEnvironment()->addTabControl(irr::core::rect<irr::s32>(5,5,635,595),NULL,true,true,-1);
  irr::gui::IGUITab *pTab[4];

  pTab[0]=m_pTabCtrl->addTab(L"Car");
  pTab[1]=m_pTabCtrl->addTab(L"Tank");
  pTab[2]=m_pTabCtrl->addTab(L"Helicopter/Plane");

  m_pTabCtrl->setVisible(false);

  pCtrl->addGui(0,pTab[0],irr::core::position2di(15,15));
  pCtrl->addGui(1,pTab[1],irr::core::position2di(15,15));
  pCtrl->addGui(2,pTab[2],irr::core::position2di(15,15));
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

irr::u32 CController::update() {
  irr::u32 iRet=CIrrOdeCarState::update();
  return iRet;
}

bool CController::OnEvent(const irr::SEvent &event) {
  return CIrrOdeCarState::OnEvent(event) || m_pController->ConfigEvent(event);
}


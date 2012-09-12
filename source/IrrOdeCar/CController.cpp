  #include <CController.h>
  #include <irrCC.h>

CController::CController(irr::IrrlichtDevice *pDevice, CIrrCC *pCtrl) : CIrrOdeCarState(pDevice,L"Controller Setup") {
  m_pTabCtrl=pDevice->getGUIEnvironment()->addTabControl(irr::core::rect<irr::s32>(5,5,635,595),NULL,true,true,-1);
  irr::gui::IGUITab *pTab[4];

  pTab[0]=m_pTabCtrl->addTab(L"Car");
  pTab[1]=m_pTabCtrl->addTab(L"Tank");
  pTab[2]=m_pTabCtrl->addTab(L"Helicopter/Plane");
  pTab[3]=m_pTabCtrl->addTab(L"Camera");

  m_pTabCtrl->setVisible(false);

  pCtrl->addGui(0,pTab[0],irr::core::position2di(15,15));
  pCtrl->addGui(1,pTab[1],irr::core::position2di(15,15));
  pCtrl->addGui(2,pTab[2],irr::core::position2di(15,15));
  pCtrl->addGui(3,pTab[3],irr::core::position2di(15,15));

  m_pController = pCtrl;
}

CController::~CController() {
}

void CController::activate() {
  m_bActive=true;
  m_bSwitchToMenu=false;
  m_pTabCtrl->setVisible(true);
}

void CController::deactivate() {
  m_bActive=false;
  m_pTabCtrl->setVisible(false);
}

irr::u32 CController::update() {
  irr::u32 iRet=CIrrOdeCarState::update();
  return iRet;
}

bool CController::OnEvent(const irr::SEvent &event) {
  return CIrrOdeCarState::OnEvent(event) || m_pController->ConfigEvent(event);
}


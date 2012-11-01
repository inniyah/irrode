  #include "CIrrOdeCarState.h"

CIrrOdeCarState::CIrrOdeCarState(irr::IrrlichtDevice *pDevice, const wchar_t *sVehicleName) {
  //initialize the members
  m_pDevice=pDevice;
  m_pSmgr=m_pDevice->getSceneManager();
  m_pGuiEnv=m_pDevice->getGUIEnvironment();

  m_iHitsScored=0;
  m_iHitsTaken=0;
  m_iControllerBy = -1;
  m_bInitialized=false;

  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getInputQueue()->addEventListener(this);
}

CIrrOdeCarState::~CIrrOdeCarState() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getInputQueue()->removeEventListener(this);
}

bool CIrrOdeCarState::OnEvent(const irr::SEvent &event) {
  return false;
}

irr::u32 CIrrOdeCarState::update() {
  return 0;
}

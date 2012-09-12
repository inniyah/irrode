  #include "CIrrOdeCarState.h"

CIrrOdeCarState::CIrrOdeCarState(irr::IrrlichtDevice *pDevice, const wchar_t *sVehicleName) {
  //initialize the members
  m_pDevice=pDevice;
  m_pSmgr=m_pDevice->getSceneManager();
  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_bActive=false;

  //hide the help text and don't switch back to menu ... for now
  m_bSwitchToMenu=false;

  m_iHitsScored=0;
  m_iHitsTaken=0;
  m_bInitialized=false;
}

bool CIrrOdeCarState::OnEvent(const irr::SEvent &event) {
  return false;
}

irr::u32 CIrrOdeCarState::update() {
  irr::u32 iRet=0;

  //If the menu should be activated we return 1 (index of the menu+1)
  if (m_bSwitchToMenu && m_bActive) iRet=1;
  return iRet;
}

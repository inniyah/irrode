  #include "CIrrOdeCarState.h"

CIrrOdeCarState::CIrrOdeCarState(irr::IrrlichtDevice *pDevice, const wchar_t *sVehicleName, const irr::c8 *sHelpFile, CIrrCC *pCtrl) {
  //initialize the members
  m_pDevice=pDevice;
  m_pSmgr=m_pDevice->getSceneManager();
  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_bActive=false;

  m_pController=pCtrl;

  //create the help text
  m_pHelp=pDevice->getGUIEnvironment()->addStaticText(L"Help",irr::core::rect<irr::s32>(5,5,635,475),true);
  m_pHelp->setDrawBackground(true);
  m_pHelp->setBackgroundColor(irr::video::SColor(0x80,0xFF,0xFF,0xFF));
  m_pHelp->setVisible(false);

  strcpy(m_sHelpFile,sHelpFile);
  loadHelpFile();

  //hide the help text and don't switch back to menu ... for now
  m_bHelp=false;
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

  //If the help shoudl be shown we deactivate the current state and show the help screen...
  if (m_bHelp) {
    deactivate();
    m_pHelp->setVisible(true);
  }
  else
    //...on hiding we activate the current state and hide the help screen
    if (m_pHelp->isVisible()) {
      m_pHelp->setVisible(false);
      activate();
    }

  return iRet;
}

void CIrrOdeCarState::loadHelpFile() {
  //load the text of the help text
  char sHelp[10000];
  FILE *f=fopen(m_sHelpFile,"rt");
  if (f) {
    fread(sHelp,1,10000,f);
    fclose(f);
  }
  else sprintf(sHelp,"Can't open \"%s\".",m_sHelpFile);

  memset(sHelp,10000,0);
  irr::core::stringw sw(sHelp);
  m_pHelp->setText(sw.c_str());
}

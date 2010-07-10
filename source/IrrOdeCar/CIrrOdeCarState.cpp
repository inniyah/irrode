  #include "CIrrOdeCarState.h"

CIrrOdeCarState::CIrrOdeCarState(IrrlichtDevice *pDevice, const wchar_t *sVehicleName, const c8 *sHelpFile, CIrrCC *pCtrl) {
  //initialize the members
  m_pDevice=pDevice;
  m_pSmgr=m_pDevice->getSceneManager();
  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_bActive=false;

  m_pController=pCtrl;

  //create the help text
  m_pHelp=pDevice->getGUIEnvironment()->addStaticText(L"Help",rect<s32>(5,5,635,475),true);
  m_pHelp->setDrawBackground(true);
  m_pHelp->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
  m_pHelp->setVisible(false);

  strcpy(m_sHelpFile,sHelpFile);
  loadHelpFile();

  //hide the help text and don't switch back to menu ... for now
  m_bHelp=false;
  m_bSwitchToMenu=false;
  m_pFps=NULL;

  m_bInitialized=false;
}

bool CIrrOdeCarState::OnEvent(const SEvent &event) {
  if (event.EventType==EET_KEY_INPUT_EVENT) {
    if (event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        //if F1 is pressed the help text should be toggled
        case KEY_F1:
          m_bHelp=!m_bHelp;
          return true;
          break;

        //if TAB is pressed the program shall return to the vehicle selection menu
        case KEY_TAB:
          m_bSwitchToMenu=true;
          return true;
          break;

        default:
          break;
      }
    }
  }

  return false;
}

u32 CIrrOdeCarState::update() {
  int iRet=0;

  //If the menu should be activated we return 1 (index of the menu+1)
  if (m_bSwitchToMenu && m_bActive) iRet=1;

  //If the help shoudl be shown we deactivate the current state and show the help screen...
  if (m_bHelp) {
    deactivate();
    if (m_pFps) m_pFps->setVisible(false);
    m_pHelp->setVisible(true);
  }
  else
    //...on hiding we activate the current state and hide the help screen
    if (m_pHelp->isVisible()) {
      m_pHelp->setVisible(false);
      if (m_pFps) m_pFps->setVisible(true);
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
  stringw sw(sHelp);
  m_pHelp->setText(sw.c_str());
}

void CIrrOdeCarState::setFpsInfo(gui::IGUIStaticText *pFps) {
  m_pFps=pFps;
}

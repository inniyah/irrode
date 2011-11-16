  #include "CIrrOdeCarState.h"

static IGUIStaticText *g_pRecording=NULL,
                      *g_pSaveFile=NULL;
static ode::CIrrOdeRecorder *g_pRecorder=NULL;
static u32 g_iCount=0;

CIrrOdeCarState::CIrrOdeCarState(IrrlichtDevice *pDevice, const wchar_t *sVehicleName, const c8 *sHelpFile, CIrrCC *pCtrl, irrklang::ISoundEngine *pSndEngine) {
  //initialize the members
  m_pDevice=pDevice;
  m_pSmgr=m_pDevice->getSceneManager();
  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_bActive=false;

  m_vCamVelocity=core::vector3df(0.0f,0.0f,0.0f);

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

  if (g_pRecording==NULL) {
    dimension2d<u32> cSize=m_pDevice->getVideoDriver()->getScreenSize();
    rect<s32> cRect=rect<s32>(cSize.Width/2-100,5,cSize.Width/2+100,20);
    g_pRecording=m_pGuiEnv->addStaticText(L"Recording",cRect,true,true,0,-1,true);
    g_pRecording->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);
    g_pRecording->setVisible(false);
  }

  if (g_pSaveFile==NULL) {
    dimension2d<u32> cSize=m_pDevice->getVideoDriver()->getScreenSize();
    rect<s32> cRect=rect<s32>(cSize.Width/2-100,25,cSize.Width/2+100,40);
    g_pSaveFile=m_pGuiEnv->addStaticText(L"Replay file saved.",cRect,true,true,0,-1,true);
    g_pSaveFile->setTextAlignment(EGUIA_CENTER,EGUIA_CENTER);
    g_pSaveFile->setVisible(false);
  }

  m_iHitsScored=0;
  m_iHitsTaken=0;
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

        case KEY_F2:
          if (g_pRecorder==NULL) {
            printf("starting recording...\n");
            g_pRecorder=new ode::CIrrOdeRecorder(m_pDevice,"IrrOdeCar");
            g_pRecorder->startRecording();
            g_pRecording->setVisible(true);
          }
          else {
            printf("stopping recording...\n");
            g_pRecorder->stopRecording();
            g_pRecorder->saveRecording("../../data/replay/car.rec");
            delete g_pRecorder;
            g_pRecorder=NULL;
            g_pRecording->setVisible(false);
            g_pSaveFile->setVisible(true);
            g_iCount=m_pDevice->getTimer()->getTime()+3000;
          }
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

  if (g_pSaveFile->isVisible() && m_pDevice->getTimer()->getTime()>g_iCount) g_pSaveFile->setVisible(false);

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

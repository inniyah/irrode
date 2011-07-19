  #include <irrlicht.h>
  #include <CCockpitPlane.h>
  #include <nrpneedleindicator.h>

using namespace irr;

CCockpitPlane::CCockpitPlane(IrrlichtDevice *pDevice, scene::ISceneManager *pRttSmgr) : IRenderToTexture(pDevice) {
  m_pGuienv =pDevice->getGUIEnvironment();
  m_pDrv    =pDevice->getVideoDriver();
  m_pSmgr   =pDevice->getSceneManager();
  m_pRttSmgr=pRttSmgr;

  m_pElement=m_pDrv->addRenderTargetTexture(core::dimension2d<u32>(128,128));
  m_pTarget =m_pDrv->addRenderTargetTexture(core::dimension2d<u32>(512,512));

  m_pCam=m_pRttSmgr->addCameraSceneNode();
  m_pCam->setPosition(core::vector3df(0,0,-1.75f));
  m_pCam->setTarget(core::vector3df(0,0,0));
  m_pCam->setAspectRatio(-1.0f);
  m_pCam->setNearValue(0.1f);
  m_pRttSmgr->setActiveCamera(m_pCam);

  scene::IMesh *p=m_pRttSmgr->getMesh("../../data/horizon.obj");

  if (p) {
    m_pHorizon=m_pRttSmgr->addMeshSceneNode(p);
    if (m_pHorizon) {
      m_pHorizon->getMaterial(0).setFlag(video::EMF_LIGHTING,false);
      m_pHorizon->setPosition(core::vector3df(0.0f,0.0f,0.0f));
    }
  }

  m_pTab=m_pGuienv->addTab(core::rect<s32>(0,0,512,512));
  m_pGuienv->addImage(m_pElement,core::position2di(286,10),true,m_pTab);
  m_pGuienv->addImage(m_pDrv->getTexture("../../data/instruments/horizon_mask.png"),core::position2di(286,10),true,m_pTab); //424

  char sNames[][0xFF]={ "autopilot", "alt", "brakes", "stall" };

  for (u32 i=0; i<4; i++) {
    char sExts[][0xFF]={ "grey", "green", "yellow", "red" };
    for (u32 j=0; j<4; j++) {
      char s[0xFF];
      sprintf(s,"../../data/warnlights/%s_%s.png",sNames[i],sExts[j]);
      m_pWarnTex[i][j]=m_pDrv->getTexture(s);
    }
    m_pWarnImg[i]=m_pGuienv->addImage(m_pWarnTex[i][0],core::position2di(424,10+32*i),true,m_pTab);
  }

  char sInstruments[][0xFF]={
    "../../data/instruments/speed.png",
    "../../data/instruments/altitude.png",
    "../../data/instruments/heading.png",
    "../../data/instruments/power.png"
  };
  core::recti cRect[]={
    core::recti(core::position2di( 10, 10),core::dimension2di(128,128)),
    core::recti(core::position2di(148, 10),core::dimension2di(128,128)),
    core::recti(core::position2di(10 ,148),core::dimension2di( 64, 64)),
    core::recti(core::position2di(148,148),core::dimension2di(128,128))
  };

  for (u32 i=0; i<4; i++) {
    m_pInstruments[i]=new gui::CNrpNeedleIndicator(L"",m_pGuienv,m_pTab,-1,cRect[i]);
    m_pInstruments[i]->SetMajorTicks(0);
    m_pInstruments[i]->SetMinorTicks(0);
    m_pInstruments[i]->SetAnimated(false);
    m_pInstruments[i]->SetBackgroundTexture(m_pDrv->getTexture(sInstruments[i]));
    m_pInstruments[i]->setNeedleColor(video::SColor(0xFF,0,0,0));
    switch (i) {
      case 0: m_pInstruments[i]->SetRange(0.0f, 200.0f); break;
      case 1: m_pInstruments[i]->SetRange(0.0f,1000.0f); break;
      case 2:
        m_pInstruments[i]->SetRange(0.0f, 360.0f);
        m_pInstruments[i]->SetGapAngle(0.0f);
        m_pInstruments[i]->setDrawLastTick(false);
        break;
      case 3: m_pInstruments[i]->SetRange(0.0f,125.0f); m_pInstruments[i]->SetValue(25.0f); break;
    }
  }

  gui::IGUITabControl *pTab=m_pGuienv->addTabControl(core::rect<s32>(core::position2di(286,148),core::dimension2du(202,128)),m_pTab,true,true);

  gui::IGUIFont *pFont=m_pGuienv->getFont("../../data/bigfont.png");

  m_pGuienv->addStaticText(L"Target:"  ,core::rect<s32>(10,10,80, 30),false,false,pTab,-1,false)->setOverrideFont(pFont);
  m_pGuienv->addStaticText(L"Distance:",core::rect<s32>(10,35,80, 55),false,false,pTab,-1,false)->setOverrideFont(pFont);
  m_pGuienv->addStaticText(L"Shots:"   ,core::rect<s32>(10,60,80, 80),false,false,pTab,-1,false)->setOverrideFont(pFont);
  m_pGuienv->addStaticText(L"Hits:"    ,core::rect<s32>(10,85,80,105),false,false,pTab,-1,false)->setOverrideFont(pFont);

  m_pLblTgtName=m_pGuienv->addStaticText(L"Tg Name",core::rect<s32>(85,10,185, 30),true,true,pTab,-1,true);
  m_pLblTgtDist=m_pGuienv->addStaticText(L"Tg Dist",core::rect<s32>(85,35,185, 55),true,true,pTab,-1,true);
  m_pLblShots  =m_pGuienv->addStaticText(L"Shots"  ,core::rect<s32>(85,60,165, 80),true,true,pTab,-1,true);
  m_pLblHits   =m_pGuienv->addStaticText(L"Hits"   ,core::rect<s32>(85,85,165,105),true,true,pTab,-1,true);

  m_pLblTgtName->setOverrideFont(pFont);
  m_pLblTgtDist->setOverrideFont(pFont);
  m_pLblShots  ->setOverrideFont(pFont);
  m_pLblHits   ->setOverrideFont(pFont);

  m_pGuienv->addImage(m_pDrv->getTexture("../../data/dustbin.png"),core::position2di(169,96),true,pTab);
  m_pTab->setVisible(false);

  m_fAltitude=0.0f;
  m_fSpeed   =0.0f;
  m_fPower   =0.0f;
  m_fHeading =0.0f;
}

CCockpitPlane::~CCockpitPlane() {
}

video::ITexture *CCockpitPlane::getTexture() { return m_pTarget; }

void CCockpitPlane::update() {
  f32 fDummy=m_fHeading+180.0f;

  while (fDummy<  0.0f) fDummy+=360.0f;
  while (fDummy>360.0f) fDummy-=360.0f;

  m_pInstruments[0]->SetValue(m_fSpeed);
  m_pInstruments[1]->SetValue(m_fAltitude);
  m_pInstruments[2]->SetValue(fDummy);
  m_pInstruments[3]->SetValue(m_fPower+25.0f);

  m_pDrv->setRenderTarget(m_pElement,true,true,video::SColor(0xFF,0xFF,0x80,0x80));
  m_pRttSmgr->drawAll();
  m_pDrv->setRenderTarget(m_pTarget,true,true,video::SColor(0xFF,0xFF,0xD0,0x80));
  m_pTab->setVisible(true);
  m_pGuienv->drawAll();
  m_pTab->setVisible(false);

  m_pDrv->setRenderTarget(video::ERT_FRAME_BUFFER);
}

void CCockpitPlane::setWarnState(u32 iWarn, u32 iState) {
  m_pWarnImg[iWarn]->setImage(m_pWarnTex[iWarn][iState]);
}

void CCockpitPlane::setHorizon(core::vector3df vRot, core::vector3df vUp) {
  core::vector3df v=vRot.rotationToDirection(core::vector3df(0.0f,0.0f,1.75f));
  m_pCam->setPosition(v);

  vUp.X=-vUp.X;
  vUp.Z=-vUp.Z;

  m_pCam->setUpVector(vUp);
}

void CCockpitPlane::setTargetName(const wchar_t *sName) {
  m_pLblTgtName->setText(sName);
}

void CCockpitPlane::setTargetDist(f32 fDist) {
  wchar_t s[0xFF];
  swprintf(s,0xFF,L"%.2f",fDist);
  m_pLblTgtDist->setText(s);
}

void CCockpitPlane::setShotsFired(s32 iShots) {
  wchar_t s[0xFF];
  swprintf(s,0xFF,L"%i",iShots);
  m_pLblShots->setText(s);
}

void CCockpitPlane::setHits(s32 iHits) {
  wchar_t s[0xFF];
  swprintf(s,0xFF,L"%i",iHits);
  m_pLblHits->setText(s);
}

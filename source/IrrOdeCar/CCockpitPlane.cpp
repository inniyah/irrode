  #include <irrlicht.h>
  #include <CCockpitPlane.h>
  #include <CGUINeedleIndicator.h>

using namespace irr;

CCockpitPlane::CCockpitPlane(IrrlichtDevice *pDevice, const char *sName) : IRenderToTexture(pDevice,sName,core::dimension2d<u32>(512,512)) {
  m_pRttSmgr=m_pSmgr->createNewSceneManager();
	scene::ICameraSceneNode *pCam=m_pRttSmgr->addCameraSceneNode();

	pCam->setPosition(core::vector3df(0,0,20));
	pCam->setTarget(core::vector3df(0,0,0));
	pCam->setFOV(10.0f);
	m_pRttSmgr->setActiveCamera(pCam);

  m_pElement=m_pDrv->addRenderTargetTexture(core::dimension2d<u32>(128,128));

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
  m_pGuienv->addImage(m_pElement,core::position2di(224,10),true,m_pTab);
  m_pGuienv->addImage(m_pDrv->getTexture("../../data/instruments/horizon_mask.png"),core::position2di(224,10),true,m_pTab); //424

  //plane warnlights
  {
    char sNames[][0xFF]={ "autopilot", "alt", "brakes", "stall" };

    m_pPlaneWarnings=m_pGuienv->addTab(core::rect<s32>(10,74,74,136),m_pTab);
    core::position2di cWarnPos[]={
      core::position2di( 0, 0),
      core::position2di(32, 0),
      core::position2di( 0,32),
      core::position2di(32,32)
    };

    for (u32 i=0; i<4; i++) {
      char sExts[][0xFF]={ "grey", "green", "yellow", "red" };
      for (u32 j=0; j<4; j++) {
        char s[0xFF];
        sprintf(s,"../../data/warnlights/%s_%s.png",sNames[i],sExts[j]);
        m_pWarnTexPlane[i][j]=m_pDrv->getTexture(s);
      }
      m_pWarnImgPlane[i]=m_pGuienv->addImage(m_pWarnTexPlane[i][0],cWarnPos[i],true,m_pPlaneWarnings);
    }
  }

  //helicopter warnlights
  {
    char sNames[][0xFF]={ "autopilot", "alt" };
    m_pHeliWarnings=m_pGuienv->addTab(core::rect<s32>(10,74,74,136),m_pTab);
    core::position2di cWarnPos[]={
      core::position2di(0, 0),
      core::position2di(0,32)
    };

    for (u32 i=0; i<2; i++) {
      char sExts[][0xFF]={ "grey", "green", "yellow", "red" };
      for (u32 j=0; j<4; j++) {
        char s[0xFF];
        sprintf(s,"../../data/warnlights/heli_%s_%s.png",sNames[i],sExts[j]);
        m_pWarnTexHeli[i][j]=m_pDrv->getTexture(s);
      }
      m_pWarnImgHeli[i]=m_pGuienv->addImage(m_pWarnTexHeli[i][0],cWarnPos[i],true,m_pHeliWarnings);
    }
  }

  char sInstruments[][0xFF]={
    "../../data/instruments/speed.png",
    "../../data/instruments/altitude.png",
    "../../data/instruments/heading.png",
    "../../data/instruments/power.png",
    "../../data/instruments/vario.png",
    "../../data/instruments/speed_heli.png"
  };
  core::recti cRect[]={
    core::recti(core::position2di( 84, 10),core::dimension2di(128,128)),
    core::recti(core::position2di(362, 10),core::dimension2di(128,128)),
    core::recti(core::position2di(224,148),core::dimension2di(128,128)),
    core::recti(core::position2di( 10, 10),core::dimension2di( 64, 64)),
    core::recti(core::position2di(362,148),core::dimension2di(128,128)),
    core::recti(core::position2di( 84, 10),core::dimension2di(128,128))
  };

  for (u32 i=0; i<6; i++) {
    m_pInstruments[i]=new gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,cRect[i]);
    m_pInstruments[i]->setBackground(m_pDrv->getTexture(sInstruments[i]));
    m_pInstruments[i]->addNeedle(video::SColor(0xFF,0,0,0),0.9f,0.05f,1.0f);

    switch (i) {
      case 0:
      case 5:
        m_pInstruments[i]->setRange(0.0f, 200.0f);
        m_pInstruments[i]->setAngleRange(0.0f,270.0f);
        m_pInstruments[i]->setAngleOffset(45.0f);
        break;

      case 1:
        m_pInstruments[i]->setRange(0.0f,1000.0f);
        m_pInstruments[i]->addNeedle(video::SColor(0xFF,0x80,0,0),0.5f,0.03f,10.0f);
        break;

      case 2:
        m_pInstruments[i]->setRange(0.0f, 360.0f);
        m_pInstruments[i]->setAngleOffset(-90.0f);
        break;

      case 3:
        m_pInstruments[i]->setRange(-25.0f,100.0f);
        m_pInstruments[i]->setAngleRange(-45.0f,240.0f);
        m_pInstruments[i]->setAngleOffset(90.0f);
        break;

      case 4:
        m_pInstruments[i]->setRange(-50.0f,50.0f);
        m_pInstruments[i]->setAngleRange(-135.0f,135.0f);
        m_pInstruments[i]->setAngleOffset(90.0f);
        break;
    }
  }

  gui::IGUITabControl *pTab=m_pGuienv->addTabControl(core::rect<s32>(core::position2di(10,148),core::dimension2du(202,128)),m_pTab,true,true);

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

  u32 iReplace=processTextureReplace(m_pSmgr->getRootSceneNode());
  printf("**** CockpitPlane: replaced %i texture.\n",iReplace);
}

CCockpitPlane::~CCockpitPlane() {
  m_pElement->drop();
}

void CCockpitPlane::update(bool bPlane) {
  f32 fDummy=m_fHeading+180.0f;

  while (fDummy<  0.0f) fDummy+=360.0f;
  while (fDummy>360.0f) fDummy-=360.0f;

  m_pInstruments[0]->setValue(m_fSpeed);
  m_pInstruments[1]->setValue(m_fAltitude);
  m_pInstruments[2]->setValue(fDummy);
  m_pInstruments[3]->setValue(m_fPower);
  m_pInstruments[4]->setValue(m_fVelVert);
  m_pInstruments[5]->setValue(m_fSpeed);

  if (bPlane) {
    m_pPlaneWarnings->setVisible(true);
    m_pHeliWarnings->setVisible(false);
    m_pInstruments[0]->setVisible(true);
    m_pInstruments[5]->setVisible(false);
  }
  else {
    m_pPlaneWarnings->setVisible(false);
    m_pHeliWarnings->setVisible(true);
    m_pInstruments[0]->setVisible(false);
    m_pInstruments[5]->setVisible(true);
  }

  m_pDrv->setRenderTarget(m_pElement,true,true,video::SColor(0xFF,0xFF,0x80,0x80));
  m_pRttSmgr->drawAll();
  startRttUpdate();
  m_pTab->setVisible(true);
  m_pGuienv->drawAll();
  m_pTab->setVisible(false);
  endRttUpdate();
}

void CCockpitPlane::setWarnStatePlane(u32 iWarn, u32 iState) {
  m_pWarnImgPlane[iWarn]->setImage(m_pWarnTexPlane[iWarn][iState]);
}

void CCockpitPlane::setWarnStateHeli(u32 iWarn, u32 iState) {
  m_pWarnImgHeli[iWarn]->setImage(m_pWarnTexHeli[iWarn][iState]);
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

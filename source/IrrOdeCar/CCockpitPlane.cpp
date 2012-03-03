  #include <irrlicht.h>
  #include <CCockpitPlane.h>
  #include <CGUINeedleIndicator.h>
  #include <CEventVehicleState.h>
  #include <CIrrOdeManager.h>
  #include <event/IIrrOdeEventQueue.h>

CCockpitPlane::CCockpitPlane(irr::IrrlichtDevice *pDevice, const char *sName) : IRenderToTexture(pDevice,sName,irr::core::dimension2d<irr::u32>(512,512)) {
  m_bLapStarted = false;
  m_iTime = 0;
  m_iLapStart = 0;

  m_pRttSmgr=m_pSmgr->createNewSceneManager();
	irr::scene::ICameraSceneNode *pCam=m_pRttSmgr->addCameraSceneNode();

	pCam->setPosition(irr::core::vector3df(0,0,20));
	pCam->setTarget(irr::core::vector3df(0,0,0));
	pCam->setFOV(10.0f);
	m_pRttSmgr->setActiveCamera(pCam);

  m_pElement=m_pDrv->addRenderTargetTexture(irr::core::dimension2d<irr::u32>(128,128));

  m_pCam=m_pRttSmgr->addCameraSceneNode();
  m_pCam->setPosition(irr::core::vector3df(0,0,-1.75f));
  m_pCam->setTarget(irr::core::vector3df(0,0,0));
  m_pCam->setAspectRatio(-1.0f);
  m_pCam->setNearValue(0.1f);
  m_pRttSmgr->setActiveCamera(m_pCam);

  irr::scene::IMesh *p=m_pRttSmgr->getMesh("../../data/models/horizon.obj");

  if (p) {
    m_pHorizon=m_pRttSmgr->addMeshSceneNode(p);
    if (m_pHorizon) {
      m_pHorizon->getMaterial(0).setFlag(irr::video::EMF_LIGHTING,false);
      m_pHorizon->setPosition(irr::core::vector3df(0.0f,0.0f,0.0f));
    }
  }

  m_pTab=m_pGuienv->addTab(irr::core::rect<irr::s32>(0,0,512,512));
  m_pGuienv->addImage(m_pElement,irr::core::position2di(224,10),true,m_pTab);
  m_pGuienv->addImage(m_pDrv->getTexture("../../data/instruments/horizon_mask.png"),irr::core::position2di(224,10),true,m_pTab); //424

  //plane warnlights
  {
    char sNames[][0xFF]={ "autopilot", "alt", "brakes", "stall" };

    m_pPlaneWarnings=m_pGuienv->addTab(irr::core::rect<irr::s32>(10,74,74,136),m_pTab);
    irr::core::position2di cWarnPos[]={
      irr::core::position2di( 0, 0),
      irr::core::position2di(32, 0),
      irr::core::position2di( 0,32),
      irr::core::position2di(32,32)
    };

    for (irr::u32 i=0; i<4; i++) {
      char sExts[][0xFF]={ "grey", "green", "yellow", "red" };
      for (irr::u32 j=0; j<4; j++) {
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
    m_pHeliWarnings=m_pGuienv->addTab(irr::core::rect<irr::s32>(10,74,74,136),m_pTab);
    irr::core::position2di cWarnPos[]={
      irr::core::position2di(0, 0),
      irr::core::position2di(0,32)
    };

    for (irr::u32 i=0; i<2; i++) {
      char sExts[][0xFF]={ "grey", "green", "yellow", "red" };
      for (irr::u32 j=0; j<4; j++) {
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
  irr::core::recti cRect[]={
    irr::core::recti(irr::core::position2di( 84, 10),irr::core::dimension2di(128,128)),
    irr::core::recti(irr::core::position2di(362, 10),irr::core::dimension2di(128,128)),
    irr::core::recti(irr::core::position2di(224,148),irr::core::dimension2di(128,128)),
    irr::core::recti(irr::core::position2di( 10, 10),irr::core::dimension2di( 64, 64)),
    irr::core::recti(irr::core::position2di(362,148),irr::core::dimension2di(128,128)),
    irr::core::recti(irr::core::position2di( 84, 10),irr::core::dimension2di(128,128))
  };

  for (irr::u32 i=0; i<6; i++) {
    m_pInstruments[i]=new irr::gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,cRect[i]);
    m_pInstruments[i]->setBackground(m_pDrv->getTexture(sInstruments[i]));
    m_pInstruments[i]->addNeedle(irr::video::SColor(0xFF,0,0,0),0.9f,0.05f,1.0f);

    switch (i) {
      case 0:
      case 5:
        m_pInstruments[i]->setRange(0.0f, 200.0f);
        m_pInstruments[i]->setAngleRange(0.0f,270.0f);
        m_pInstruments[i]->setAngleOffset(45.0f);
        break;

      case 1:
        m_pInstruments[i]->setRange(0.0f,1000.0f);
        m_pInstruments[i]->addNeedle(irr::video::SColor(0xFF,0x80,0,0),0.5f,0.03f,10.0f);
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

  irr::gui::IGUITabControl *pTab=m_pGuienv->addTabControl(irr::core::rect<irr::s32>(irr::core::position2di(10,148),irr::core::dimension2du(202,128)),m_pTab,true,true);

  irr::gui::IGUIFont *pFont=m_pGuienv->getFont("../../data/bigfont.png");

  m_pWeaponInfo = m_pGuienv->addTab(irr::core::rect<irr::s32>(10, 10, 185, 125), pTab);

  m_pGuienv->addStaticText(L"Target:"  ,irr::core::rect<irr::s32>(0,  0,80, 20),false,false,m_pWeaponInfo,-1,false)->setOverrideFont(pFont);
  m_pGuienv->addStaticText(L"Distance:",irr::core::rect<irr::s32>(0, 25,80, 45),false,false,m_pWeaponInfo,-1,false)->setOverrideFont(pFont);
  m_pGuienv->addStaticText(L"Shots:"   ,irr::core::rect<irr::s32>(0, 50,80, 70),false,false,m_pWeaponInfo,-1,false)->setOverrideFont(pFont);
  m_pGuienv->addStaticText(L"Hits Sc:" ,irr::core::rect<irr::s32>(0, 75,80, 95),false,false,m_pWeaponInfo,-1,false)->setOverrideFont(pFont);
  m_pGuienv->addStaticText(L"Hits Tk:" ,irr::core::rect<irr::s32>(0,100,80,115),false,false,m_pWeaponInfo,-1,false)->setOverrideFont(pFont);

  m_pLblTgtName   =m_pGuienv->addStaticText(L"Tg Name",irr::core::rect<irr::s32>(75,  0,175, 20),true,true,m_pWeaponInfo,-1,true);
  m_pLblTgtDist   =m_pGuienv->addStaticText(L"Tg Dist",irr::core::rect<irr::s32>(75, 25,175, 45),true,true,m_pWeaponInfo,-1,true);
  m_pLblShots     =m_pGuienv->addStaticText(L"Shots"  ,irr::core::rect<irr::s32>(75, 50,175, 70),true,true,m_pWeaponInfo,-1,true);
  m_pLblHitsScored=m_pGuienv->addStaticText(L"Hits Sc",irr::core::rect<irr::s32>(75, 75,155, 95),true,true,m_pWeaponInfo,-1,true);
  m_pLblHitsTaken =m_pGuienv->addStaticText(L"Hits Tk",irr::core::rect<irr::s32>(75,100,155,115),true,true,m_pWeaponInfo,-1,true);

  m_pLblTgtName   ->setOverrideFont(pFont);
  m_pLblTgtDist   ->setOverrideFont(pFont);
  m_pLblShots     ->setOverrideFont(pFont);
  m_pLblHitsScored->setOverrideFont(pFont);
  m_pLblHitsTaken ->setOverrideFont(pFont);

  m_pLapInfo = m_pGuienv->addTab(irr::core::rect<irr::s32>(0,0,195,135), pTab);

  m_stCurLap =m_pGuienv->addStaticText(L"Current Lap",irr::core::rect<irr::s32>(irr::core::position2di(5,10),irr::core::dimension2di(190,20)),false,true,m_pLapInfo);
  m_stSplit  =m_pGuienv->addStaticText(L"Split Time" ,irr::core::rect<irr::s32>(irr::core::position2di(5,35),irr::core::dimension2di(190,20)),false,true,m_pLapInfo);
  m_stLastLap=m_pGuienv->addStaticText(L"Last Lap"   ,irr::core::rect<irr::s32>(irr::core::position2di(5,60),irr::core::dimension2di(190,20)),false,true,m_pLapInfo);

  m_stCurLap ->setOverrideFont(pFont);
  m_stSplit  ->setOverrideFont(pFont);
  m_stLastLap->setOverrideFont(pFont);

  m_pWeaponInfo->setVisible(false);
  m_pLapInfo->setVisible(false);

  m_pGuienv->addImage(m_pDrv->getTexture("../../data/dustbin.png"),irr::core::position2di(169,96),true,pTab);
  m_pTab->setVisible(false);

  m_fAltitude=0.0f;
  m_fSpeed   =0.0f;
  m_fPower   =0.0f;
  m_fHeading =0.0f;

  m_iInfoMode = 0;

  irr::u32 iReplace=processTextureReplace(m_pSmgr->getRootSceneNode());
  printf("**** CockpitPlane: replaced %i texture.\n",iReplace);

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CCockpitPlane::~CCockpitPlane() {
  m_pElement->drop();
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CCockpitPlane::update(bool bPlane) {
  irr::f32 fDummy=m_fHeading+180.0f;

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

  m_pDrv->setRenderTarget(m_pElement,true,true,irr::video::SColor(0xFF,0xFF,0x80,0x80));
  m_pRttSmgr->drawAll();
  startRttUpdate();
  m_pTab->setVisible(true);

  switch (m_iInfoMode) {
    case 0:
      m_pWeaponInfo->setVisible(true);
      m_pLapInfo->setVisible(false);
      break;

    case 1:
      m_pWeaponInfo->setVisible(false);
      m_pLapInfo->setVisible(true);
      break;
  }

  m_pGuienv->drawAll();
  m_pTab->setVisible(false);
  endRttUpdate();
}

void CCockpitPlane::setWarnStatePlane(irr::u32 iWarn, irr::u32 iState) {
  m_pWarnImgPlane[iWarn]->setImage(m_pWarnTexPlane[iWarn][iState]);
}

void CCockpitPlane::setWarnStateHeli(irr::u32 iWarn, irr::u32 iState) {
  m_pWarnImgHeli[iWarn]->setImage(m_pWarnTexHeli[iWarn][iState]);
}

void CCockpitPlane::setHorizon(irr::core::vector3df vRot, irr::core::vector3df vUp) {
  irr::core::vector3df v=vRot.rotationToDirection(irr::core::vector3df(0.0f,0.0f,1.75f));
  m_pCam->setPosition(v);

  vUp.X=-vUp.X;
  vUp.Z=-vUp.Z;

  m_pCam->setUpVector(vUp);
}

void CCockpitPlane::setTargetName(const wchar_t *sName) {
  m_pLblTgtName->setText(sName);

  m_pLapInfo->setVisible(false);
  m_pWeaponInfo->setVisible(true);
}

void CCockpitPlane::setTargetDist(irr::f32 fDist) {
  wchar_t s[0xFF];
  swprintf(s,0xFF,L"%.2f",fDist);
  m_pLblTgtDist->setText(s);
}

void CCockpitPlane::setShotsFired(irr::s32 iShots) {
  wchar_t s[0xFF];
  swprintf(s,0xFF,L"%i",iShots);
  m_pLblShots->setText(s);
  m_iInfoMode = 0;
}

void CCockpitPlane::setHitsScored(irr::s32 iHits) {
  wchar_t s[0xFF];
  swprintf(s,0xFF,L"%i",iHits);
  m_pLblHitsScored->setText(s);
  m_iInfoMode = 0;
}

void CCockpitPlane::setHitsTaken(irr::s32 iHits) {
  wchar_t s[0xFF];
  swprintf(s,0xFF,L"%i",iHits);
  m_pLblHitsTaken->setText(s);
  m_iInfoMode = 0;
}

bool CCockpitPlane::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  wchar_t s[0xFF];

  if (pEvent->getType() == EVENT_LAP_TIME_ID) {
    m_iInfoMode = 1;
    CEventLapTime *p = (CEventLapTime *)pEvent;
    if (p->getBodyId() == m_iBodyId) {
      switch (p->getCpId()) {
        case 0:
          m_bLapStarted = true;
          m_iLapStart = m_iTime;

          if (m_bLapStarted) {
            swprintf(s,0xFF,L"Last Lap: %.2f sec",p->getTime());
            m_stLastLap->setText(s);
          }
          break;

        case 1:
        case 2:
          swprintf(s,0xFF,L"Split Time: %.2f sec",p->getTime());
          m_stSplit->setText(s);
          break;

        case 3:
          m_stCurLap->setText(L"Current Lap: -----");
          m_stSplit->setText(L"Split Time: -----");
          m_bLapStarted = false;
          break;
      }
    }
  }

  if (pEvent->getType() == irr::ode::eIrrOdeEventStep) {
    if (m_bLapStarted) {
      swprintf(s,0xFF,L"Current Lap: %.2f sec",((float)(m_iTime - m_iLapStart) * 0.016f));
      m_stCurLap->setText(s);
    }
    m_iTime++;
  }

  return true;
}

bool CCockpitPlane::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == EVENT_LAP_TIME_ID || pEvent->getType() == irr::ode::eIrrOdeEventStep;
}

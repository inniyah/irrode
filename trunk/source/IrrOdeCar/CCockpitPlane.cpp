  #include <irrlicht.h>
  #include <CCockpitPlane.h>
  #include <CGUINeedleIndicator.h>
  #include <CEventVehicleState.h>
  #include <CIrrOdeManager.h>
  #include <CAutoPilot.h>
  #include <event/IIrrOdeEventQueue.h>
  #include <event/CIrrOdeEventBodyMoved.h>
  #include <thread/IThread.h>

CCockpitPlane::CCockpitPlane(irr::IrrlichtDevice *pDevice, const char *sName, irr::ode::CIrrOdeBody *pObject, irr::ode::CIrrOdeManager *pOdeMgr) : IRenderToTexture(pDevice,sName,irr::core::dimension2d<irr::u32>(512,512)) {
  m_bLapStarted = false;
  m_iTime       = 0;
  m_iLapStart   = 0;
  m_iBodyId     = pObject->getID();
  m_pObject     = pObject;
  m_pTarget     = NULL;
  m_pApTarget   = NULL;
  m_bPlane      = pObject->getOdeClassname() == "plane";

  m_pRttSmgr=m_pSmgr->createNewSceneManager();

  m_pElement=m_pDrv->addRenderTargetTexture(irr::core::dimension2d<irr::u32>(128,128));

  m_pCam=m_pRttSmgr->addCameraSceneNode();
  m_pCam->setPosition(irr::core::vector3df(0,0,-1.75f));
  m_pCam->setTarget(irr::core::vector3df(0,0,0));
  m_pCam->setAspectRatio(-1.0f);
  m_pCam->setNearValue(0.1f);
  m_pCam->setProjectionMatrix(m_pCam->getProjectionMatrix(), true);
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

  m_stTgtName   =m_pGuienv->addStaticText(L"Tg Name",irr::core::rect<irr::s32>(75,  0,175, 20),true,true,m_pWeaponInfo,-1,true);
  m_stTgtDist   =m_pGuienv->addStaticText(L"Tg Dist",irr::core::rect<irr::s32>(75, 25,175, 45),true,true,m_pWeaponInfo,-1,true);
  m_stShots     =m_pGuienv->addStaticText(L"Shots"  ,irr::core::rect<irr::s32>(75, 50,175, 70),true,true,m_pWeaponInfo,-1,true);
  m_stHitsScored=m_pGuienv->addStaticText(L"Hits Sc",irr::core::rect<irr::s32>(75, 75,155, 95),true,true,m_pWeaponInfo,-1,true);
  m_stHitsTaken =m_pGuienv->addStaticText(L"Hits Tk",irr::core::rect<irr::s32>(75,100,155,115),true,true,m_pWeaponInfo,-1,true);

  m_stTgtName   ->setOverrideFont(pFont);
  m_stTgtDist   ->setOverrideFont(pFont);
  m_stShots     ->setOverrideFont(pFont);
  m_stHitsScored->setOverrideFont(pFont);
  m_stHitsTaken ->setOverrideFont(pFont);

  m_pLapInfo = m_pGuienv->addTab(irr::core::rect<irr::s32>(0,0,195,135), pTab);

  m_stCurLap =m_pGuienv->addStaticText(L"Current Lap",irr::core::rect<irr::s32>(irr::core::position2di(5,10),irr::core::dimension2di(190,20)),false,true,m_pLapInfo);
  m_stSplit  =m_pGuienv->addStaticText(L"Split Time" ,irr::core::rect<irr::s32>(irr::core::position2di(5,35),irr::core::dimension2di(190,20)),false,true,m_pLapInfo);
  m_stLastLap=m_pGuienv->addStaticText(L"Last Lap"   ,irr::core::rect<irr::s32>(irr::core::position2di(5,60),irr::core::dimension2di(190,20)),false,true,m_pLapInfo);

  m_stCurLap ->setOverrideFont(pFont);
  m_stSplit  ->setOverrideFont(pFont);
  m_stLastLap->setOverrideFont(pFont);

  m_pApInfo = m_pGuienv->addTab(irr::core::rect<irr::s32>(0,0,195,135), pTab);
  m_stAutoPilot = m_pGuienv->addStaticText(L"Auto Pilot", irr::core::rect<irr::s32>(irr::core::position2di(5,10), irr::core::dimension2di(190,20)),false,true,m_pApInfo);
  m_stApNextCp  = m_pGuienv->addStaticText(L"Auto Pilot", irr::core::rect<irr::s32>(irr::core::position2di(5,35), irr::core::dimension2di(190,20)),false,true,m_pApInfo);
  m_stApState   = m_pGuienv->addStaticText(L"Auto Pilot", irr::core::rect<irr::s32>(irr::core::position2di(5,60), irr::core::dimension2di(190,20)),false,true,m_pApInfo);

  m_stAutoPilot->setOverrideFont(pFont);
  m_stApState  ->setOverrideFont(pFont);
  m_stApNextCp ->setOverrideFont(pFont);

  printf("1\n");
  m_pWeaponInfo->setVisible(false);
  m_pLapInfo   ->setVisible(false);
  m_pApInfo    ->setVisible(false);

  m_pGuienv->addImage(m_pDrv->getTexture("../../data/dustbin.png"),irr::core::position2di(169,96),true,pTab);
  m_pTab->setVisible(false);

  m_fAltitude=0.0f;
  m_fSpeed   =0.0f;
  m_fPower   =0.0f;
  m_fHeading =0.0f;

  m_iInfoMode = 0;

  irr::u32 iReplace=processTextureReplace(pObject);
  printf("**** CockpitPlane: replaced %i texture.\n",iReplace);

  m_pOdeMgr = pOdeMgr;
  m_pOdeMgr->getIrrThread()->getInputQueue()->addEventListener(this);
}

CCockpitPlane::~CCockpitPlane() {
  m_pElement->drop();
  m_pOdeMgr->getIrrThread()->getInputQueue()->removeEventListener(this);
}

void CCockpitPlane::update() {
  irr::f32 fDummy=m_fHeading+180.0f;

  while (fDummy<  0.0f) fDummy+=360.0f;
  while (fDummy>360.0f) fDummy-=360.0f;

  m_pInstruments[0]->setValue(m_fSpeed);
  m_pInstruments[1]->setValue(m_fAltitude);
  m_pInstruments[2]->setValue(fDummy);
  m_pInstruments[3]->setValue(100.0f*m_fPower);
  m_pInstruments[4]->setValue(m_fVelVert);
  m_pInstruments[5]->setValue(m_fSpeed);

  if (m_bPlane) {
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
      m_pLapInfo   ->setVisible(false);
      m_pApInfo    ->setVisible(false);
      break;

    case 1:
      m_pWeaponInfo->setVisible(false);
      m_pLapInfo   ->setVisible(true);
      m_pApInfo    ->setVisible(false);
      break;

    case 2:
      m_pWeaponInfo->setVisible(false);
      m_pLapInfo   ->setVisible(false);
      m_pApInfo    ->setVisible(true);
      break;
  }

  if (m_pObject != NULL) {
    if (m_pApTarget != NULL) {
      irr::f32 f = m_pApTarget->getAbsolutePosition().getDistanceFrom(m_pObject->getAbsolutePosition());
      wchar_t s[0xFF];
      swprintf(s,0xFF,L"Target: %i (%.2f)", m_pApTarget->getID()-99999, f);
      m_stApNextCp->setText(s);
    }
    else m_stApNextCp->setText(L"");

    if (m_pTarget != NULL) {
      irr::core::vector3df v = m_pObject->getPosition() - m_pTarget->getPosition();
      wchar_t s[0xFF];
      swprintf(s,0xFF,L"%.2f", v.getLength());
      m_stTgtDist->setText(s);
    }
  }

  m_pGuienv->drawAll();
  m_pTab->setVisible(false);
  endRttUpdate();
}

void CCockpitPlane::setHorizon(irr::core::vector3df vRot, irr::core::vector3df vUp) {
  irr::core::vector3df v=vRot.rotationToDirection(irr::core::vector3df(0.0f,0.0f,1.75f));
  m_pCam->setPosition(v);

  vUp.X=-vUp.X;
  vUp.Z=-vUp.Z;

  m_pCam->setUpVector(vUp);
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

  if (pEvent->getType() == EVENT_AUTOPILOT_ID) {
    CEventAutoPilot *p=(CEventAutoPilot *)pEvent;

    if (p->getObject() == m_iBodyId) {
      m_iInfoMode = 2;
      m_pApTarget = p->getNextCp()!=-1 ? m_pSmgr->getSceneNodeFromId(p->getNextCp()) : NULL;
      m_stAutoPilot->setText(p->isActive()?L"Autopilot enabled":L"Autopilot disabled");

      updateApState(p->getState());
    }
  }

  if (pEvent->getType() == irr::ode::eIrrOdeEventBodyMoved) {
    irr::ode::CIrrOdeEventBodyMoved *p=(irr::ode::CIrrOdeEventBodyMoved *)pEvent;
    if (p->getBodyId() == m_iBodyId) {
      irr::ode::CIrrOdeBody *pBody = p->getBody();
      if (pBody != NULL) {
        irr::core::vector3df vPos = pBody->getAbsolutePosition(),
                             vVel = pBody->getLinearVelocity(),
                             vRot = pBody->getRotation(),
                             vFor = vRot.rotationToDirection(irr::core::vector3df(0.0f,0.0f,-1.0f)),
                             vNor = vVel;

        m_fAltitude = vPos.Y;
        m_fVelVert  = vVel.Y;

        irr::core::vector2df vDir=irr::core::vector2df(vFor.X,vFor.Z);

        if (vFor.getLength()>0.01f) m_fHeading = vDir.getAngle();

        vNor.normalize();

        m_fSpeed = vNor.dotProduct(vVel);

        setHorizon(vRot, vRot.rotationToDirection(irr::core::vector3df(0.0f, 1.0f, 0.0f)));

        m_pWarnImgPlane[1]->setImage(m_pWarnTexPlane[1][vPos.Y<300.0f?3:vPos.Y<550.0f?2:1]);
        m_pWarnImgPlane[3]->setImage(m_pWarnTexPlane[3][m_fSpeed<5.0f?0:m_fSpeed<30.0f?3:m_fSpeed<45.0f?2:1]);

        m_pWarnImgHeli[1]->setImage(m_pWarnTexHeli[1][vPos.Y<200.0f?3:vPos.Y<400.0f?2:1]);
      }
      else printf("body == NULL @ CCockpitPlane\n");
    }
  }

  if (pEvent->getType() == EVENT_PLANE_STATE_ID) {
    CEventPlaneState *p=(CEventPlaneState *)pEvent;
    if (p->getNodeId() == m_iBodyId) {
      m_pWarnImgPlane[0]->setImage(m_pWarnTexPlane[0][p->isAutoPilotOn()?1:0]);
      m_pWarnImgPlane[2]->setImage(m_pWarnTexPlane[2][p->isBrakesOn   ()?2:1]);
      m_fPower = p->getThrust();
    }
  }

  if (pEvent->getType() == EVENT_HELI_STATE_ID) {
    CEventHeliState *p=(CEventHeliState *)pEvent;
    if (p->getNodeId() == m_iBodyId) {
      m_fPower = p->getThrust();
    }
  }

  if (pEvent->getType() == EVENT_SELECT_TARGET_ID) {
    CEventSelectTarget *p = (CEventSelectTarget *)pEvent;

    if (p->getBody() == m_iBodyId) {
      m_pTarget = p->getTarget()!=-1?m_pSmgr->getSceneNodeFromId(p->getTarget()):NULL;
      if (m_pTarget) {
        m_iInfoMode = 0;
        m_stTgtName->setText(irr::core::stringw(m_pTarget->getName()).c_str());
      }
      else {
        m_stTgtName->setText(L"<no target>");
        m_stTgtDist->setText(L"<no target>");
      }
    }
  }

  if (pEvent->getType() == EVENT_SHOTS_ID) {
    CEventShots *p = (CEventShots *)pEvent;
    if (p->getBody() == m_iBodyId) {
      wchar_t s[0xFF];
      swprintf(s,0xFF,L"%i",p->getShotsFired()); m_stShots     ->setText(s);
      swprintf(s,0xFF,L"%i",p->getHitsScored()); m_stHitsScored->setText(s);
      swprintf(s,0xFF,L"%i",p->getHitsTaken ()); m_stHitsTaken ->setText(s);
      m_iInfoMode = 0;
    }
  }

  return true;
}

void CCockpitPlane::updateApState(irr::s32 iApState) {
  switch (iApState) {
    case CAutoPilot::eApPlaneLowAlt: m_stApState->setText(L"\"State: PlaneLowAlt\"\n"); break;
    case CAutoPilot::eApPlaneCruise: m_stApState->setText(L"\"State: PlaneCruise\"\n"); break;
    case CAutoPilot::eApHeliLowAlt : m_stApState->setText(L"\"State: HeliLowAlt\"\n" ); break;
    case CAutoPilot::eApHeliCruise : m_stApState->setText(L"\"State: HeliCruise\"\n" ); break;
  }
}

bool CCockpitPlane::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == irr::ode::eIrrOdeEventStep      ||
         pEvent->getType() == irr::ode::eIrrOdeEventBodyMoved ||
         pEvent->getType() == EVENT_AUTOPILOT_ID              ||
         pEvent->getType() == EVENT_LAP_TIME_ID               ||
         pEvent->getType() == EVENT_PLANE_STATE_ID            ||
         pEvent->getType() == EVENT_HELI_STATE_ID             ||
         pEvent->getType() == EVENT_SELECT_TARGET_ID          ||
         pEvent->getType() == EVENT_SHOTS_ID;
}

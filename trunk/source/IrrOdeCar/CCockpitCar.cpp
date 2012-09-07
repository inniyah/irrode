  #include <irrlicht.h>
  #include <CCockpitCar.h>
  #include <CGUINeedleIndicator.h>
  #include <event/IIrrOdeEvent.h>
  #include <event/IIrrOdeEventQueue.h>
  #include <CEventVehicleState.h>
  #include <CIrrOdeManager.h>

CCockpitCar::CCockpitCar(irr::IrrlichtDevice *pDevice, const char *sName, irr::scene::ISceneNode *pBody) : IRenderToTexture(pDevice,sName,irr::core::dimension2d<irr::u32>(512,512)) {
  m_pTab=m_pGuienv->addTab(irr::core::rect<irr::s32>(0,0,512,512));

  m_iBodyId = -1;
  m_bLapStarted = false;
  m_iLapStart = 0;
  m_iTime = 0;
  m_iBodyId = pBody!=NULL?pBody->getID():-1;

  m_pGuienv->addStaticText(L"The Car",irr::core::rect<irr::s32>(5,400,105,25),false,true,m_pTab);

  m_pMeter=new irr::gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,irr::core::rect<irr::s32>(irr::core::position2di(128,128),irr::core::dimension2di(256,256)));
  m_pMeter->setBackground(m_pDrv->getTexture("../../data/car/speed_meter.png"));
  m_pMeter->addNeedle(irr::video::SColor(0xFF,0,0,0),0.7f,0.02f,1.0f);
  m_pMeter->setRange(0.0f,100.0f);
  m_pMeter->setAngleRange(0.0f,270.0f);
  m_pMeter->setAngleOffset(90.0f);

  m_pRpm=new irr::gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,irr::core::rect<irr::s32>(irr::core::position2di(0,0),irr::core::dimension2di(128,128)));
  m_pRpm->setBackground(m_pDrv->getTexture("../../data/car/rpm.png"));
  m_pRpm->addNeedle(irr::video::SColor(0xFF,0,0,0),0.7f,0.02f,1.0f);
  m_pRpm->setRange(0.25f,1.75f);
  m_pRpm->setAngleRange(0.0f,270.0f);
  m_pRpm->setAngleOffset(90.0f);

  m_pDiff=new irr::gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,irr::core::rect<irr::s32>(irr::core::position2di(128,0),irr::core::dimension2di(128,128)));
  m_pDiff->addNeedle(irr::video::SColor(0xFF,0,0,0),0.7f,0.02f,1.0f);
  m_pDiff->setBackground(m_pDrv->getTexture("../../data/car/differential.png"));
  m_pDiff->setRange(-150.0f,150.0f);
  m_pDiff->setAngleRange(-85.0f,85.0f);
  m_pDiff->setAngleOffset(180.0f);

  m_pAdaptRed =m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/adapt_red.png" ),irr::core::position2di(138,300),true,m_pTab);
  m_pAdaptGray=m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/adapt_gray.png"),irr::core::position2di(138,300),true,m_pTab);

  m_pShiftRed =m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/shift_red.png" ),irr::core::position2di(170,300),true,m_pTab);
  m_pShiftGray=m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/shift_gray.png"),irr::core::position2di(170,300),true,m_pTab);

  m_pBoostYellow=m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_yellow.png"),irr::core::position2di(202,300),true,m_pTab);
  m_pBoostGray  =m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_gray.png"  ),irr::core::position2di(202,300),true,m_pTab);
  m_pBoostGreen =m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_green.png" ),irr::core::position2di(202,300),true,m_pTab);
  m_pBoostRed   =m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_red.png"   ),irr::core::position2di(202,300),true,m_pTab);

  for (irr::u32 i = 0; i < 7; i++) {
    irr::c8 s[0xFF];
    sprintf(s, "../../data/car/gear_%i.png",i);
    m_pGear[i] = m_pGuienv->addImage(m_pDrv->getTexture(s), irr::core::position2di(323,323), true, m_pTab);
    m_pGear[i]->setVisible(false);
  }

  m_pTab->setVisible(false);

  m_stDifferential=m_pGuienv->addStaticText(L"Active",irr::core::rect<irr::s32>(irr::core::position2di(160,96),irr::core::dimension2du(64,13)),false,true,m_pTab);
  m_stDifferential->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);

  m_bDifferential=true;

  m_stCurLap =m_pGuienv->addStaticText(L"Current Lap",irr::core::rect<irr::s32>(irr::core::position2di(400, 5),irr::core::dimension2di(100,13)),false,true,m_pTab);
  m_stSplit  =m_pGuienv->addStaticText(L"Split Time" ,irr::core::rect<irr::s32>(irr::core::position2di(400,20),irr::core::dimension2di(100,13)),false,true,m_pTab);
  m_stLastLap=m_pGuienv->addStaticText(L"Last Lap"   ,irr::core::rect<irr::s32>(irr::core::position2di(400,35),irr::core::dimension2di(100,13)),false,true,m_pTab);

  irr::u32 iReplace=processTextureReplace(pBody);
  printf("**** CCockpitCar: replaced %i texture (%i, \"%s\").\n",iReplace,(int)pBody,pBody->getName());

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CCockpitCar::~CCockpitCar() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CCockpitCar::update() {
  m_pMeter->setValue(m_fSpeed);
  m_pDiff->setValue(m_fDiff);
  m_pRpm->setValue(m_fRpm);
  m_stDifferential->setBackgroundColor(m_bDifferential?irr::video::SColor(0xFF,0,0xFF,0):irr::video::SColor(0xFF,0xD0,0xD0,0xD0));
  startRttUpdate();
  m_pTab->setVisible(true);
  m_pGuienv->drawAll();
  m_pTab->setVisible(false);
  endRttUpdate();
}

bool CCockpitCar::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  wchar_t s[0xFF];

  if (pEvent->getType() == EVENT_LAP_TIME_ID) {
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

  if (pEvent->getType() == EVENT_CAR_STATE_ID) {
    CEventCarState *p=(CEventCarState *)pEvent;

    if (p->getNodeId() == m_iBodyId) {
      m_fRpm = -p->getRpm();
      m_fDiff = p->getDiff();
      m_bDifferential = p->getFlags() & CEventCarState::eCarFlagDifferential;

      bool b = p->getFlags() & CEventCarState::eCarFlagSmoke;
      m_pShiftGray->setVisible(!b);
      m_pShiftRed ->setVisible( b);

      b = p->getFlags() & CEventCarState::eCarFlagAdapt;
      m_pAdaptGray->setVisible(!b);
      m_pAdaptRed ->setVisible( b);

      b = p->getFlags() & CEventCarState::eCarFlagBoost;
      irr::s32 iBoost = p->getBoost();

      wchar_t s[0xFF];
      swprintf(s, 0xFF, L"%.0f %%", 100.0f * ((irr::f32)iBoost) / 1800.0f);

      m_pBoostGray  ->setVisible(!b   );
      m_pBoostRed   ->setVisible(false);
      m_pBoostGreen ->setVisible(false);
      m_pBoostYellow->setVisible(false);

      if (iBoost > 900) {
        if (b) m_pBoostGreen->setVisible(true);
      }
      else
        if (iBoost > 120) {
          if (b) m_pBoostYellow->setVisible(true);
        }
        else {
          if (b) m_pBoostRed->setVisible(true);
        }

      m_fSpeed = p->getSpeed();

      for (irr::u32 i = 0; i < 7; i++) m_pGear[i]->setVisible(false);
      m_pGear[p->getGear()+1]->setVisible(true);
    }
  }

  return true;
}

bool CCockpitCar::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == EVENT_LAP_TIME_ID          ||
         pEvent->getType() == irr::ode::eIrrOdeEventStep ||
         pEvent->getType() == EVENT_CAR_STATE_ID;
}

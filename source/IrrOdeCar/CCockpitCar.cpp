  #include <irrlicht.h>
  #include <CCockpitCar.h>
  #include <CGUINeedleIndicator.h>
  #include <event/IIrrOdeEvent.h>
  #include <event/IIrrOdeEventQueue.h>
  #include <CEventVehicleState.h>
  #include <CIrrOdeManager.h>

CCockpitCar::CCockpitCar(irr::IrrlichtDevice *pDevice, const char *sName) : IRenderToTexture(pDevice,sName,irr::core::dimension2d<irr::u32>(512,512)) {
  m_pTab=m_pGuienv->addTab(irr::core::rect<irr::s32>(0,0,512,512));

  m_iBodyId = -1;
  m_bLapStarted = false;
  m_iLapStart = 0;
  m_iTime = 0;

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
  m_pRpm->setRange(0.0f,175.0f);
  m_pRpm->setAngleRange(0.0f,270.0f);
  m_pRpm->setAngleOffset(90.0f);

  m_pDiff=new irr::gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,irr::core::rect<irr::s32>(irr::core::position2di(128,0),irr::core::dimension2di(128,128)));
  m_pDiff->addNeedle(irr::video::SColor(0xFF,0,0,0),0.7f,0.02f,1.0f);
  m_pDiff->setBackground(m_pDrv->getTexture("../../data/car/differential.png"));
  m_pDiff->setRange(-150.0f,150.0f);
  m_pDiff->setAngleRange(-85.0f,85.0f);
  m_pDiff->setAngleOffset(180.0f);

  m_pBoostRed =m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_red.png" ),irr::core::position2di(164,328),true,m_pTab);
  m_pBoostGray=m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_gray.png"),irr::core::position2di(164,328),true,m_pTab);
  m_pTab->setVisible(false);

  m_stDifferential=m_pGuienv->addStaticText(L"Active",irr::core::rect<irr::s32>(irr::core::position2di(160,96),irr::core::dimension2du(64,13)),false,true,m_pTab);
  m_stDifferential->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);

  m_bDifferential=true;

  m_stCurLap =m_pGuienv->addStaticText(L"Current Lap",irr::core::rect<irr::s32>(irr::core::position2di(400, 5),irr::core::dimension2di(100,13)),false,true,m_pTab);
  m_stSplit  =m_pGuienv->addStaticText(L"Split Time" ,irr::core::rect<irr::s32>(irr::core::position2di(400,20),irr::core::dimension2di(100,13)),false,true,m_pTab);
  m_stLastLap=m_pGuienv->addStaticText(L"Last Lap"   ,irr::core::rect<irr::s32>(irr::core::position2di(400,35),irr::core::dimension2di(100,13)),false,true,m_pTab);

  irr::u32 iReplace=processTextureReplace(m_pSmgr->getRootSceneNode());
  printf("**** CCockpitCar: replaced %i texture.\n",iReplace);

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CCockpitCar::~CCockpitCar() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CCockpitCar::update(bool b) {
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

void CCockpitCar::setBoost(bool b) {
  m_pBoostGray->setVisible(!b);
  m_pBoostRed ->setVisible( b);
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

  return true;
}

bool CCockpitCar::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == EVENT_LAP_TIME_ID || pEvent->getType() == irr::ode::eIrrOdeEventStep;
}

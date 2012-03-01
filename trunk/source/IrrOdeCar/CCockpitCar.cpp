  #include <irrlicht.h>
  #include <CCockpitCar.h>
  #include <CGUINeedleIndicator.h>
  #include <event/IIrrOdeEvent.h>
  #include <event/IIrrOdeEventQueue.h>
  #include <CEventVehicleState.h>
  #include <CIrrOdeManager.h>

using namespace irr;

CCockpitCar::CCockpitCar(IrrlichtDevice *pDevice, const char *sName) : IRenderToTexture(pDevice,sName,core::dimension2d<u32>(512,512)) {
  m_pTab=m_pGuienv->addTab(core::rect<s32>(0,0,512,512));

  m_iBodyId = -1;
  m_bLapStarted = false;
  m_iLapStart = 0;
  m_iTime = 0;

  m_pGuienv->addStaticText(L"The Car",core::rect<s32>(5,400,105,25),false,true,m_pTab);

  m_pMeter=new gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,core::rect<s32>(core::position2di(128,128),core::dimension2di(256,256)));
  m_pMeter->setBackground(m_pDrv->getTexture("../../data/car/speed_meter.png"));
  m_pMeter->addNeedle(video::SColor(0xFF,0,0,0),0.7f,0.02f,1.0f);
  m_pMeter->setRange(0.0f,100.0f);
  m_pMeter->setAngleRange(0.0f,270.0f);
  m_pMeter->setAngleOffset(90.0f);

  m_pRpm=new gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,core::rect<s32>(core::position2di(0,0),core::dimension2di(128,128)));
  m_pRpm->setBackground(m_pDrv->getTexture("../../data/car/rpm.png"));
  m_pRpm->addNeedle(video::SColor(0xFF,0,0,0),0.7f,0.02f,1.0f);
  m_pRpm->setRange(0.0f,175.0f);
  m_pRpm->setAngleRange(0.0f,270.0f);
  m_pRpm->setAngleOffset(90.0f);

  m_pDiff=new gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,core::rect<s32>(core::position2di(128,0),core::dimension2di(128,128)));
  m_pDiff->addNeedle(video::SColor(0xFF,0,0,0),0.7f,0.02f,1.0f);
  m_pDiff->setBackground(m_pDrv->getTexture("../../data/car/differential.png"));
  m_pDiff->setRange(-150.0f,150.0f);
  m_pDiff->setAngleRange(-85.0f,85.0f);
  m_pDiff->setAngleOffset(180.0f);

  m_pBoostRed =m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_red.png" ),core::position2di(164,328),true,m_pTab);
  m_pBoostGray=m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_gray.png"),core::position2di(164,328),true,m_pTab);
  m_pTab->setVisible(false);

  m_stDifferential=m_pGuienv->addStaticText(L"Active",core::rect<s32>(core::position2di(160,96),core::dimension2du(64,13)),false,true,m_pTab);
  m_stDifferential->setTextAlignment(gui::EGUIA_CENTER,gui::EGUIA_CENTER);

  m_bDifferential=true;

  m_stCurLap =m_pGuienv->addStaticText(L"Current Lap",core::rect<s32>(core::position2di(260,260),core::dimension2di(100,13)),false,true,m_pTab);
  m_stSplit  =m_pGuienv->addStaticText(L"Split Time" ,core::rect<s32>(core::position2di(260,274),core::dimension2di(100,13)),false,true,m_pTab);
  m_stLastLap=m_pGuienv->addStaticText(L"Last Lap"   ,core::rect<s32>(core::position2di(260,288),core::dimension2di(100,13)),false,true,m_pTab);

  u32 iReplace=processTextureReplace(m_pSmgr->getRootSceneNode());
  printf("**** CCockpitCar: replaced %i texture.\n",iReplace);

  ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CCockpitCar::~CCockpitCar() {
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CCockpitCar::update(bool b) {
  m_pMeter->setValue(m_fSpeed);
  m_pDiff->setValue(m_fDiff);
  m_pRpm->setValue(m_fRpm);
  m_stDifferential->setBackgroundColor(m_bDifferential?video::SColor(0xFF,0,0xFF,0):video::SColor(0xFF,0xD0,0xD0,0xD0));

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

void CCockpitCar::setCurrentLapTime(f32 fTime) {
}

void CCockpitCar::setLastLapTime(f32 fLastLap) {
}

void CCockpitCar::setSplitTime(f32 fTime) {
}

void CCockpitCar::cancelLap() {
}

bool CCockpitCar::onEvent(ode::IIrrOdeEvent *pEvent) {
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

  if (pEvent->getType() == ode::eIrrOdeEventStep) {
    if (m_bLapStarted) {
      swprintf(s,0xFF,L"Current Lap: %.2f sec",((float)(m_iTime - m_iLapStart) * 0.016f));
      m_stCurLap->setText(s);
    }
    m_iTime++;
  }

  return true;
}

bool CCockpitCar::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == EVENT_LAP_TIME_ID || pEvent->getType() == ode::eIrrOdeEventStep;
}

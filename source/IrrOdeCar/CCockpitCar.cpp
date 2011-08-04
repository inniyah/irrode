  #include <irrlicht.h>
  #include <CCockpitCar.h>
  #include <CGUINeedleIndicator.h>

using namespace irr;

CCockpitCar::CCockpitCar(IrrlichtDevice *pDevice, const char *sName) : IRenderToTexture(pDevice,sName,core::dimension2d<u32>(512,512)) {
  m_pTab=m_pGuienv->addTab(core::rect<s32>(0,0,512,512));

  m_pGuienv->addStaticText(L"The Car",core::rect<s32>(5,5,105,25),false,true,m_pTab);

  m_pMeter=new gui::CGUINeedleIndicator(m_pGuienv,m_pTab,-1,core::rect<s32>(core::position2di(128,128),core::dimension2di(256,256)));
  m_pMeter->setBackground(m_pDrv->getTexture("../../data/car/speed_meter.png"));
  m_pMeter->addNeedle(video::SColor(0xFF,0,0,0),0.7f,0.02f,1.0f);
  m_pMeter->setRange(0.0f,100.0f);
  m_pMeter->setAngleRange(0.0f,270.0f);
  m_pMeter->setAngleOffset(90.0f);
  m_pBoostRed =m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_red.png" ),core::position2di(164,328),true,m_pTab);
  m_pBoostGray=m_pGuienv->addImage(m_pDrv->getTexture("../../data/car/boost_gray.png"),core::position2di(164,328),true,m_pTab);
  m_pTab->setVisible(false);

  u32 iReplace=processTextureReplace(m_pSmgr->getRootSceneNode());
  printf("**** CCockpitCar: replaced %i texture.\n",iReplace);
}

CCockpitCar::~CCockpitCar() {
}

void CCockpitCar::update(bool b) {
  m_pMeter->setValue(m_fSpeed);

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

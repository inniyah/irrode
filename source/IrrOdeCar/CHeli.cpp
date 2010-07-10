  #include "CHeli.h"
  #include <CProjectile.h>
  #include <irrCC.h>

CHeli::CHeli(IrrlichtDevice *pDevice, CIrrCC *pCtrl) : CIrrOdeCarState(pDevice,L"Helicopter","../../data/irrOdeHeliHelp.txt",pCtrl) {
  m_pWorld=reinterpret_cast<CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode"));
  m_pHeliBody=reinterpret_cast<CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("theHeli"));
  if (m_pHeliBody!=NULL) {
    m_pTerrain=reinterpret_cast<ITerrainSceneNode *>(m_pSmgr->getSceneNodeFromName("terrain"));

    m_pCam=m_pSmgr->addCameraSceneNode();
    m_bLeft=true;
    m_bMissileCam=false;

    CIrrOdeManager::getSharedInstance()->addEventListener(this);

    m_pInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,5,150,65),true);
    m_pInfo->setDrawBackground(true);
    m_pInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
    m_pInfo->setVisible(false);

    m_pMotor=reinterpret_cast<irr::ode::CIrrOdeImpulseMotor *>(m_pSmgr->getSceneNodeFromName("heli_motor"));
    m_pTorque=reinterpret_cast<irr::ode::CIrrOdeTorqueMotor *>(m_pSmgr->getSceneNodeFromName("heli_torque"));
    m_pAero=reinterpret_cast<irr::ode::CIrrOdeAeroDrag *>(m_pSmgr->getSceneNodeFromName("heli_aero"));

    m_fThrust=0;
    m_fPitch=0;
    m_fRoll=0;
    m_fYaw=0;
    m_bInitialized=true;
  }
}

CHeli::~CHeli() {
}

void CHeli::activate() {
  m_pSmgr->setActiveCamera(m_pCam);
  m_pDevice->setEventReceiver(this);
  m_pDevice->getCursorControl()->setVisible(false);
  m_pInfo->setVisible(true);
  m_bSwitchToMenu=false;
  m_bActive=true;

  vector3df pos=m_pHeliBody->getRotation().rotationToDirection(vector3df(15,5,0)),
            up =m_pHeliBody->getRotation().rotationToDirection(vector3df(0,0.1,0)),
            tgt=m_pHeliBody->getRotation().rotationToDirection(vector3df(0,1  ,0));

  m_pCam->setPosition(m_pHeliBody->getPosition()+pos);
  m_pCam->setUpVector(up);
  m_pCam->setTarget(m_pHeliBody->getPosition()+tgt);

  loadHelpFile();
  wchar_t s[1024];
  swprintf(s,1023,m_pHelp->getText(),m_pController->getSettingsText(2));
  m_pHelp->setText(s);
}

void CHeli::deactivate() {
  m_pInfo->setVisible(false);
  m_bActive=false;
}

u32 CHeli::update() {
  u32 iRet=CIrrOdeCarState::update();
  static u32 iLastTime=0,iStep=0;

  if (!iLastTime)
    iLastTime=m_pDevice->getTimer()->getTime();
  else {
    u32 iTime=m_pDevice->getTimer()->getTime();
    iStep+=iTime-iLastTime;
    iLastTime=iTime;
  }

  vector3df rot=m_pHeliBody->getRotation();

  vector3df pos=rot.rotationToDirection(vector3df(15,5,0)),
            up =m_pHeliBody->getRotation().rotationToDirection(vector3df(0,0.1,0)),
            tgt=m_pHeliBody->getRotation().rotationToDirection(vector3df(0,1  ,0));

  CProjectileManager *ppm=CProjectileManager::getSharedInstance();

  if (m_bMissileCam && ppm->getLast()!=NULL) {
    pos=ppm->getLast()->getBody()->getRotation().rotationToDirection(vector3df(10,2,0));
    m_pCam->setPosition(ppm->getLast()->getBody()->getPosition()+pos);
    m_pCam->setUpVector(vector3df(0,1,0));
    m_pCam->setTarget(ppm->getLast()->getBody()->getPosition());
  }
  else {
    m_pCam->setPosition(m_pHeliBody->getPosition()+pos);
    m_pCam->setUpVector(up);
    m_pCam->setTarget(m_pHeliBody->getPosition()+tgt);
  }

  wchar_t dummy[0xFF];
  pos=m_pHeliBody->getAbsolutePosition();
  rot=m_pHeliBody->getRotation();
  vector3df v=m_pHeliBody->getLinearVelocity();
  swprintf(dummy,0xFE,L"pos: (%.0f, %.0f, %.0f)\nvelocity: (%.0f, %.0f, %.0f)=%.2f",pos.X,pos.Y,pos.Z,v.X,v.Y,v.Z,v.getLength());
  swprintf(dummy,0xFE,L"%s\nrotation: (%.0f, %.0f, %.0f)\ncontrols: ",dummy,rot.X,rot.Y,rot.Z);

  swprintf(dummy,0xFE,L"%s (T=%.0f%%",dummy,m_fThrust*100);

  swprintf(dummy,0xFE,L"%s, Y=%.0f",dummy,m_fYaw);
  swprintf(dummy,0xFE,L"%s, P=%.0f",dummy,m_fPitch);
  swprintf(dummy,0xFE,L"%s, R=%.0f",dummy,m_fRoll);

  swprintf(dummy,0xFE,L"%s)",dummy);
  f32 fHeight=pos.Y-m_pTerrain->getHeight(pos.X,pos.Z);
  swprintf(dummy,0xFE,L"%s\nAltitude: %.0f",dummy,fHeight);
  if (m_bMissileCam) swprintf(dummy,0xFE,L"%s\nMissile Cam active",dummy);

  m_pInfo->setText(dummy);

  return iRet;
}

bool CHeli::OnEvent(const SEvent &event) {
  bool bRet=m_pController->OnEvent(event);
  bRet|=CIrrOdeCarState::OnEvent(event);
  return bRet;
}

bool CHeli::onEvent(IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    vector3df rot=m_pHeliBody->getRotation();
    f32 alt=m_pHeliBody->getAbsolutePosition().Y,f=alt<=0?1:(2000-alt)/2000;

    if (m_bActive) {
      m_fThrust=m_pController->get(m_pCtrls[eHeliPowerUp]);

      m_fPitch=5*m_pController->get(m_pCtrls[eHeliPitchDown]);
      m_fRoll =3*m_pController->get(m_pCtrls[eHeliRollRight]);
      m_fYaw  =5*m_pController->get(m_pCtrls[eHeliYawLeft  ]);

      if (m_fThrust> 1.0f) m_fThrust= 1.0f;
      if (m_fThrust<-0.1f) m_fThrust=-0.1f;

      m_pMotor->setPower(m_fThrust*f);
      m_pTorque->setPitch(m_fPitch);
      m_pTorque->setRoll(m_fRoll);
      m_pTorque->setYaw(-m_fYaw);

      if (m_pController->get(m_pCtrls[eHeliFire])!=0.0f) {
        m_pController->set(m_pCtrls[eHeliFire],0.0f);
        //We add a new missile...
        vector3df pos=m_pHeliBody->getAbsolutePosition()+m_pHeliBody->getRotation().rotationToDirection(vector3df(-2.0f,-0.6f,m_bLeft?-3.0f:3.0f)),
                  rot=m_pHeliBody->getRotation(),vel=m_pHeliBody->getLinearVelocity();

        new CProjectile(m_pSmgr,pos,rot,vel,"../../data/missile.3ds",600,vector3df(-500.0f,0.0f,0.0f),m_pWorld,1.2f,true);
        m_bLeft=!m_bLeft;
      }

      if (m_pController->get(m_pCtrls[eHeliToggleCam])) {
        m_pController->set(m_pCtrls[eHeliToggleCam],0.0f);
        m_bMissileCam=!m_bMissileCam;
      }
    }
  }

  return false;
}

bool CHeli::handlesEvent(IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}


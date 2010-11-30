  #include "CTank.h"
  #include <CProjectile.h>
  #include <irrCC.h>
  #include <CEventRudderPositions.h>
  #include <CCustomEventReceiver.h>

CTank::CTank(IrrlichtDevice *pDevice, CIrrCC *pCtrl) : CIrrOdeCarState(pDevice,L"Tank","../../data/irrOdeTankHelp.txt",pCtrl) {
  m_pWorld=reinterpret_cast<CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode"));
  m_pTankBody=reinterpret_cast<CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("theTank"));

  if (m_pTankBody!=NULL) {
    CCustomEventReceiver::getSharedInstance()->addTank(m_pTankBody);
    m_pTurret=reinterpret_cast<CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("turret"));

    m_pMotor[0]=reinterpret_cast<CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("motorRR"));
    m_pMotor[1]=reinterpret_cast<CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("motorFR"));
    m_pMotor[2]=reinterpret_cast<CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("motorRL"));
    m_pMotor[3]=reinterpret_cast<CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("motorFL"));

    m_pTurretMotor=reinterpret_cast<CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("turretMotor"));
    m_pCannonMotor=reinterpret_cast<CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("cannonMotor"));

    stringc aAxesNames[]={"tankAxisRR","tankAxisFR","tankAxisRL","tankAxisFL"};

    for (u32 i=0; i<4; i++) {
      ISceneNode *pNode=m_pSmgr->getSceneNodeFromName(aAxesNames[i].c_str());
      if (pNode!=NULL && pNode->getType()==irr::ode::IRR_ODE_JOINT_HINGE_ID) {
        m_lAxes.push_back((irr::ode::CIrrOdeJointHinge *)pNode);
        m_aAxesAngles[i]=0.0f;
      }
    }

    m_pCam=m_pSmgr->addCameraSceneNode();

    m_bFire=false;
    m_bFollowTurret=false;
    m_bFollowBullet=false;
    m_bFastCollision=true;

    m_pCannon=reinterpret_cast<CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("cannon"));

    m_pTextures[0]=m_pDevice->getVideoDriver()->getTexture("../../data/target.jpg");
    m_pTextures[1]=m_pDevice->getVideoDriver()->getTexture("../../data/target1.jpg");

    m_pInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,5,150,65),true);
    m_pInfo->setDrawBackground(true);
    m_pInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
    m_pInfo->setVisible(false);
    irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
    m_bInitialized=true;
  }
}

CTank::~CTank() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CTank::activate() {
  m_pSmgr->setActiveCamera(m_pCam);
  m_pDevice->setEventReceiver(this);
  m_pDevice->getCursorControl()->setVisible(false);
  m_pInfo->setVisible(true);
  m_bSwitchToMenu=false;
  m_bActive=true;

  vector3df rot=!m_bFollowTurret?m_pTankBody->getRotation():m_pTurret->getRotation();

  vector3df pos=rot.rotationToDirection(vector3df(7.5,2.5,0)),
            up =m_pTankBody->getRotation().rotationToDirection(vector3df(0,0.1,0)),
            tgt=m_pTankBody->getRotation().rotationToDirection(vector3df(0,1  ,0));

  m_pCam->setPosition(m_pTankBody->getPosition()+pos);
  m_pCam->setUpVector(up);
  m_pCam->setTarget(m_pTankBody->getPosition()+tgt);

  loadHelpFile();
  wchar_t s[1024];
  swprintf(s,1023,m_pHelp->getText(),m_pController->getSettingsText(1));
  m_pHelp->setText(s);
}

void CTank::deactivate() {
  m_pInfo->setVisible(false);
  m_pController->reset();
  for (int i=0; i<4; i++) {
    m_pMotor[i]->setVelocity(0);
    m_pMotor[i]->setForce(20);
  }
  m_bActive=false;
}

u32 CTank::update() {
  u32 iRet=CIrrOdeCarState::update();
  static u32 iLastTime=0,iStep=0;

  if (!iLastTime)
    iLastTime=m_pDevice->getTimer()->getTime();
  else {
    u32 iTime=m_pDevice->getTimer()->getTime();
    iStep+=iTime-iLastTime;
    iLastTime=iTime;
  }

  if (m_bFollowBullet && CProjectileManager::getSharedInstance()->getLast()!=NULL) {
    CProjectile *p=CProjectileManager::getSharedInstance()->getLast();
    vector3df pos=p->getBody()->getPosition()-3*p->getBody()->getLinearVelocity().normalize();
    m_pCam->setPosition(pos);
    m_pCam->setUpVector(vector3df(0,1,0));
    m_pCam->setTarget(p->getBody()->getPosition());
  }
  else {
    vector3df rot=!m_bFollowTurret?m_pTankBody->getRotation():m_pTurret->getRotation();

    vector3df pos=rot.rotationToDirection(vector3df(7.5,2.5,0)),
              up =m_pTankBody->getRotation().rotationToDirection(vector3df(0,0.1,0)),
              tgt=m_pTankBody->getRotation().rotationToDirection(vector3df(0,1  ,0));

    m_pCam->setPosition(m_pTankBody->getPosition()+pos);
    m_pCam->setUpVector(up);
    m_pCam->setTarget(m_pTankBody->getPosition()+tgt);
  }

  wchar_t dummy[0xFF];
  vector3df pos=m_pTankBody->getAbsolutePosition();
  swprintf(dummy,0xFE,L"pos: (%.0f, %.0f, %.0f)\n",pos.X,pos.Y,pos.Z);
  if (m_bFollowTurret) swprintf(dummy,0xFF,L"%s\ncamera follows turret",dummy);
  if (m_bFollowBullet) swprintf(dummy,0xFF,L"%s\ncamera follows bullet",dummy);
  if (m_bFastCollision) swprintf(dummy,0xFF,L"%s\nfast collision detection on",dummy);
  m_pInfo->setText(dummy);

  return iRet;
}

bool CTank::OnEvent(const SEvent &event) {
  bool bRet=m_pController->OnEvent(event) ||  CIrrOdeCarState::OnEvent(event);

  return bRet;
}

bool CTank::onEvent(IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep && m_bActive) {
    if (m_pController->get(m_pCtrls[eTankFire])!=0.0f) {
      m_pController->set(m_pCtrls[eTankFire],0);
      //We add a new bullet...
      vector3df pos=m_pCannon->getPosition()+m_pCannon->getRotation().rotationToDirection(vector3df(-3.0f,0,0)),
                vel=m_pTankBody->getLinearVelocity()+m_pCannon->getRotation().rotationToDirection(vector3df(-350.0f,0.0f,0.0f)),
                rot=m_pCannon->getRotation();

      new CProjectile(m_pSmgr,pos,rot,vel,"shell",600,m_pWorld,m_bFastCollision);
    }

    f32 fVel[4]={ 0.0f, 0.0f, 0.0f, 0.0f },
        fAcc=m_pController->get(m_pCtrls[eTankBackward]),
        fSteer=m_pController->get(m_pCtrls[eTankRight]);

    if (fAcc!=0.0f) {
      for (int i=0; i<4; i++) fVel[i]=fAcc*25.0f;
    }
    else {
      for (int i=0; i<4; i++) fVel[i]=0.0f;
    }

    if (fSteer!=0.0f) {
      fVel[0]+=fSteer*25.0f;
      fVel[1]+=fSteer*25.0f;
      fVel[2]-=fSteer*25.0f;
      fVel[3]-=fSteer*25.0f;
    }

    m_pTurretMotor->setVelocity(0.25f*m_pController->get(m_pCtrls[eTankCannonLeft]));
    m_pCannonMotor->setVelocity(0.20f*m_pController->get(m_pCtrls[eTankCannonUp  ]));

    for (int i=0; i<4; i++) {
      m_pMotor[i]->setVelocity(fVel[i]);
      m_pMotor[i]->setForce(20);
    }

    if (m_pController->get(m_pCtrls[eTankFlip])!=0.0f) {
      m_pTankBody->addForceAtPosition(m_pTankBody->getPosition()+vector3df(0.0f,0.2f,0.0f),vector3df(0.0f,80.0f,0.0f));
    }

    if (m_pController->get(m_pCtrls[eTankCamBullet])) {
      m_pController->set(m_pCtrls[eTankCamBullet],0.0f);
      m_bFollowBullet=!m_bFollowBullet;
    }

    if (m_pController->get(m_pCtrls[eTankCamTurret])) {
      m_pController->set(m_pCtrls[eTankCamTurret],0.0f);
      m_bFollowTurret=!m_bFollowTurret;
    }

    if (m_pController->get(m_pCtrls[eTankFastCollision])) {
      m_pController->set(m_pCtrls[eTankFastCollision],0.0f);
      m_bFastCollision=!m_bFastCollision;
    }

    list<irr::ode::CIrrOdeJointHinge *>::Iterator it;
    bool bMoved=false;
    u32 i=0;
    for (it=m_lAxes.begin(); it!=m_lAxes.end() && i<4; it++) {
      irr::ode::CIrrOdeJointHinge *p=*it;
      irr::s8 iAngle=(irr::s8)(p->getHingeAngle()*60/M_PI);
      if (m_aAxesAngles[i]!=iAngle) {
        bMoved=true;
        m_aAxesAngles[i]=iAngle;
      }
      i++;
    }
    if (bMoved) {
      CEventTankState *p=new CEventTankState(m_pTankBody->getID(),m_aAxesAngles);
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
    }
  }
  return false;
}

bool CTank::handlesEvent(IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

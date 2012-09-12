  #include "CTank.h"
  #include <CProjectile.h>
  #include <irrCC.h>
  #include <CEventVehicleState.h>
  #include <CCustomEventReceiver.h>
  #include <irrKlang.h>

irr::ode::CIrrOdeBody *getChildBodyFromName(irr::ode::CIrrOdeBody *pBody, const irr::c8 *sName) {
  printf("%s .. %s\n",pBody->getName(),sName);

  irr::core::list<irr::ode::CIrrOdeBody *> children=pBody->getChildBodies();
  irr::core::list<irr::ode::CIrrOdeBody *>::Iterator it;
  printf("children: %i\n",children.size());

  for (it=children.begin(); it!=children.end(); it++) {
    irr::ode::CIrrOdeBody *pBody=NULL;
    pBody=getChildBodyFromName(*it,sName);
    if (pBody!=NULL) return pBody;
  }

  return NULL;
}

CTank::CTank(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, irr::ode::IIrrOdeEventQueue *pInputQueue) : CIrrOdeCarState(pDevice,L"Tank", pInputQueue) {
  irr::ode::IIrrOdeEventWriter::setWorld(reinterpret_cast<irr::ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode")));
  m_pTankBody=reinterpret_cast<irr::ode::CIrrOdeBody *>(pNode);
  m_iLastShot=0;
  m_fSound=0.5f;

  if (m_pTankBody!=NULL) {
    m_pTankBody->setUserData(this);
    CCustomEventReceiver::getSharedInstance()->addTank(m_pTankBody);

    m_pTurret=m_pTankBody->getChildBodyFromName("turret");

    m_pMotor[0]=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("motorRR");
    m_pMotor[1]=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("motorFR");
    m_pMotor[2]=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("motorRL");
    m_pMotor[3]=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("motorFL");

    m_pTurretMotor=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("turretMotor");
    m_pCannonServo=(irr::ode::CIrrOdeServo *)m_pTankBody->getMotorFromName("cannonServo");

    printf("\t\tturret motor: %i\n\t\tcannon servo: %i\n",(int)m_pTurretMotor,(int)m_pCannonServo);

    irr::ode::CIrrOdeJointHinge *pAxis=NULL;

    pAxis=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("tankAxisRR"); if (pAxis!=NULL) m_lAxes.push_back(pAxis);
    pAxis=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("tankAxisFR"); if (pAxis!=NULL) m_lAxes.push_back(pAxis);
    pAxis=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("tankAxisRL"); if (pAxis!=NULL) m_lAxes.push_back(pAxis);
    pAxis=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("tankAxisFL"); if (pAxis!=NULL) m_lAxes.push_back(pAxis);

    m_pCannonHinge=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("cannonAxis");
    m_pTurretHinge=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("turretAxis");

    m_pCannon=m_pTankBody->getChildBodyFromName("cannon");

    printf("\ntank state:\n\n");
    printf("axes: ");
    irr::core::list<irr::ode::CIrrOdeJointHinge *>::Iterator it;
    for (it=m_lAxes.begin(); it!=m_lAxes.end(); it++) printf("%i  ",(int)(*it));
    printf("\n");
    printf("motors: ");
    for (irr::u32 i=0; i<4; i++) printf("%i  ",(int)m_pMotor[i]);
    printf("\n");

    printf("turret: %i -- axis=%i, motor=%i\n",(int)m_pTurret,(int)m_pTurretHinge,(int)m_pTurretMotor);
    printf("cannon: %i -- axis=%i, motor=%i\n",(int)m_pCannon,(int)m_pCannonHinge,(int)m_pCannonServo);

    m_bFollowTurret=false;
    m_bFollowBullet=false;
    m_bFastCollision=true;

    m_fCannonAngle=0.0f;
    m_fTurretAngle=0.0f;

    m_pTextures[0]=m_pDevice->getVideoDriver()->getTexture("../../data/target.jpg");
    m_pTextures[1]=m_pDevice->getVideoDriver()->getTexture("../../data/target1.jpg");

    irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
    m_bInitialized=true;
  }
}

CTank::~CTank() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CTank::activate() {
  m_bSwitchToMenu=false;
  m_bActive=true;

  irr::core::vector3df rot=!m_bFollowTurret?m_pTankBody->getRotation():m_pTurret->getRotation();

  irr::core::vector3df pos=rot.rotationToDirection(irr::core::vector3df(7.5,2.5,0)),
                           up =m_pTankBody->getRotation().rotationToDirection(irr::core::vector3df(0,0.1,0)),
                           tgt=m_pTankBody->getRotation().rotationToDirection(irr::core::vector3df(0,1  ,0));

}

void CTank::deactivate() {
  //m_pController->reset();
  for (int i=0; i<4; i++) {
    m_pMotor[i]->setVelocity(0);
    m_pMotor[i]->setForce(13);
  }
  m_bActive=false;
}

irr::u32 CTank::update() {
  irr::u32 iRet=CIrrOdeCarState::update();
  static irr::u32 iLastTime=0,iStep=0;

  if (!iLastTime)
    iLastTime=m_pDevice->getTimer()->getTime();
  else {
    irr::u32 iTime=m_pDevice->getTimer()->getTime();
    iStep+=iTime-iLastTime;
    iLastTime=iTime;
  }

  if (m_bFollowBullet && CProjectileManager::getSharedInstance()->getLast()!=NULL) {
    CProjectile *p=CProjectileManager::getSharedInstance()->getLast();
    irr::core::vector3df pos=p->getBody()->getPosition()-3*p->getBody()->getLinearVelocity().normalize();
  }
  else {
    irr::core::vector3df rot=!m_bFollowTurret?m_pTankBody->getRotation():m_pTurret->getAbsoluteTransformation().getRotationDegrees();

    irr::core::vector3df pos=rot.rotationToDirection(irr::core::vector3df(7.5,2.5,0)),
                         up =m_pTankBody->getRotation().rotationToDirection(irr::core::vector3df(0,0.1,0)),
                         tgt=m_pTankBody->getRotation().rotationToDirection(irr::core::vector3df(0,1  ,0));
  }

  wchar_t dummy[0xFF];
  irr::core::vector3df pos=m_pTankBody->getAbsolutePosition();
  swprintf(dummy,0xFE,L"pos: (%.0f, %.0f, %.0f)\n",pos.X,pos.Y,pos.Z);
  if (m_bFollowTurret) swprintf(dummy,0xFF,L"%s\ncamera follows turret",dummy);
  if (m_bFollowBullet) swprintf(dummy,0xFF,L"%s\ncamera follows bullet",dummy);
  if (m_bFastCollision) swprintf(dummy,0xFF,L"%s\nfast collision detection on",dummy);

  return iRet;
}

bool CTank::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    if (m_bActive) {
      //irr::ode::CIrrOdeEventStep *pStep=(irr::ode::CIrrOdeEventStep *)pEvent;
      //if (m_pController->get(m_pCtrls[eTankFire])!=0.0f && pStep->getStepNo()-m_iLastShot>45) {
      //  m_iLastShot=pStep->getStepNo();
      //  //We add a new bullet...
      //  irr::core::vector3df rot=m_pCannon->getAbsoluteTransformation().getRotationDegrees(),
      //                       pos=m_pCannon->getAbsolutePosition()+rot.rotationToDirection(irr::core::vector3df(-3.0f,0,0)),
      //                       vel=m_pTankBody->getLinearVelocity()+rot.rotationToDirection(irr::core::vector3df(-350.0f,0.0f,0.0f));

      //  new CProjectile(m_pSmgr,pos,rot,vel,"shell",600,m_pWorld,m_bFastCollision,this);
      //}

      //irr::f32 fVel[4]={ 0.0f, 0.0f, 0.0f, 0.0f },
      //         fAcc=m_pController->get(m_pCtrls[eTankBackward]),
      //         fSteer=m_pController->get(m_pCtrls[eTankRight]);

      //if (fAcc!=0.0f) {
      //  for (irr::u32 i=0; i<4; i++) fVel[i]=fAcc*25.0f;
      //}
      //else {
      //  for (irr::u32 i=0; i<4; i++) fVel[i]=0.0f;
      //}

      //if (fSteer!=0.0f) {
      //  fVel[0]+=fSteer*25.0f;
      //  fVel[1]+=fSteer*25.0f;
      //  fVel[2]-=fSteer*25.0f;
      //  fVel[3]-=fSteer*25.0f;
      //}

      //irr::f32 cPos=m_pCannonServo->getServoPos(),
      //         fElev=m_pController->get(m_pCtrls[eTankCannonUp  ]);

      //cPos+=0.5f*fElev;

      //if (cPos< -10) cPos= -10;
      //if (cPos>  80) cPos=  80;

      //m_pCannonServo->setServoPos(cPos);

      //m_pTurretMotor->setVelocity(0.25f*m_pController->get(m_pCtrls[eTankCannonLeft]));

      //for (irr::u32 i=0; i<4; i++) {
      //  m_pMotor[i]->setVelocity(fVel[i]);
      //  m_pMotor[i]->setForce(13);
      //}

      //if (m_pController->get(m_pCtrls[eTankFlip])!=0.0f) {
      //  m_pTankBody->addForceAtPosition(m_pTankBody->getPosition()+irr::core::vector3df(0.0f,0.2f,0.0f),irr::core::vector3df(0.0f,80.0f,0.0f));
      //}

      //if (m_pController->get(m_pCtrls[eTankFastCollision])) {
      //  m_pController->set(m_pCtrls[eTankFastCollision],0.0f);
      //  m_bFastCollision=!m_bFastCollision;
      //}

      irr::core::list<irr::ode::CIrrOdeJointHinge *>::Iterator it;
      bool bMoved=false;
      irr::u32 i=0;
      for (it=m_lAxes.begin(); it!=m_lAxes.end() && i<4; it++) {
        irr::ode::CIrrOdeJointHinge *p=*it;
        irr::s8 iAngle=(irr::s8)(p->getHingeAngle()*60/M_PI);
        if (m_aAxesAngles[i]!=iAngle) {
          bMoved=true;
          m_aAxesAngles[i]=iAngle;
        }
        i++;
      }

      if (m_fCannonAngle!=m_pCannonHinge->getHingeAngle()) {
        bMoved=true;
        m_fCannonAngle=m_pCannonHinge->getHingeAngle();
      }

      if (m_fTurretAngle!=m_pTurretHinge->getHingeAngle()) {
        bMoved=true;
        m_fTurretAngle=m_pTurretHinge->getHingeAngle();
      }

      if (bMoved) dataChanged();
    }

    irr::f32 fRot=0.0f,fSound=0.5f;

    irr::core::list<irr::ode::CIrrOdeJointHinge *>::Iterator it;

    for (it=m_lAxes.begin(); it!=m_lAxes.end(); it++) {
      irr::ode::CIrrOdeJointHinge *p=*it;
      if (p->getHingeAngleRate()<0) fRot-=p->getHingeAngleRate(); else fRot+=p->getHingeAngleRate();
    }

    fRot/=4.0f;

    if (fRot>1.0f) {
      if (fRot<35.0f) {
        fSound=0.5f+(fRot-1.0f)/34.0f;
      }
      else {
        fSound=1.5f;
      }

      if (m_fSound<fSound) {
        m_fSound+=0.01f;
        if (m_fSound>fSound) m_fSound=fSound;
      }

      if (m_fSound>fSound) {
        m_fSound-=0.01f;
        if (m_fSound<fSound) m_fSound=fSound;
      }
    }
  }
  return false;
}

bool CTank::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

irr::ode::IIrrOdeEvent *CTank::writeEvent() {
  CEventTankState *p=new CEventTankState(m_pTankBody->getID(),m_aAxesAngles,m_fCannonAngle,m_fTurretAngle,m_fSound);
  return p;
}

irr::ode::eEventWriterType CTank::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

irr::ode::CIrrOdeBody *CTank::getBody() {
  return m_pTankBody;
}

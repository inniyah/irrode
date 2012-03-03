  #include "CHeli.h"
  #include <CProjectile.h>
  #include <CAutoPilot.h>
  #include <irrCC.h>
  #include <CCockpitPlane.h>
  #include <CTargetSelector.h>
  #include <irrKlang.h>
  #include <CRearView.h>
  #include <CEventVehicleState.h>
  #include <CCustomEventReceiver.h>

CHeli::CHeli(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, CRearView *pRView) : CAeroVehicle(pDevice,pNode,pCtrl,pCockpit,pRView) {
  m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor,m_pRay);

  m_pTargetSelector=new CTargetSelector(m_pBody,m_pDevice,m_pAero->getForeward());

  m_bLeft=false;
  m_fSound=0.0f;
  m_iNodeId=pNode->getID();

  CCustomEventReceiver::getSharedInstance()->addHeli(pNode);

  m_iOldHitsScored = -1;
  m_iOldHitsTaken = -1;
}

CHeli::~CHeli() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

irr::u32 CHeli::update() {
  irr::u32 iRet=CIrrOdeCarState::update();

  irr::core::vector3df rot=m_pBody->getRotation();

  //get the parameters for the camera
  irr::core::vector3df pos,tgt,
                       up =m_pBody->getRotation().rotationToDirection(irr::core::vector3df(0,0.1,0));

  if (m_bInternal) {
    irr::core::vector2df lookAt=irr::core::vector2df(0,-5).rotateBy(m_fCamAngleH),
                         lookUp=irr::core::vector2df(5, 0).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(irr::core::vector3df(0,-0.15,-1.3));
    tgt=rot.rotationToDirection(irr::core::vector3df(lookAt.X,-0.15+lookUp.Y,lookAt.Y));
  }
  else {
    irr::core::vector2df lookAt=irr::core::vector2df(  0,15).rotateBy(m_fCamAngleH),
                         lookUp=irr::core::vector2df(-15, 0).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(irr::core::vector3df(lookAt.X,5+lookUp.Y,lookAt.Y)),
    tgt=m_pBody->getRotation().rotationToDirection(irr::core::vector3df(0,5  ,0));
  }

  CProjectileManager *ppm=CProjectileManager::getSharedInstance();

  //if we follow a bomb we focus the last dropped bomb ...
  if (m_bWeaponCam && ppm->getLast()!=NULL) {
    pos=ppm->getLast()->getBody()->getRotation().rotationToDirection(irr::core::vector3df(0,5,10));
    m_pCam->setPosition(ppm->getLast()->getBody()->getPosition()+pos);
    m_pCam->setUpVector(irr::core::vector3df(0,1,0));
    m_pCam->setTarget(ppm->getLast()->getBody()->getPosition());
  }
  else {  //... otherwise we focus the plane
    m_pCam->setPosition(m_pBody->getPosition()+pos);
    m_pCam->setUpVector(up);
    m_pCam->setTarget(m_pBody->getPosition()+tgt);
  }

  return iRet;
}

void CHeli::odeStep(irr::u32 iStep) {
  irr::core::vector3df rot=m_pBody->getRotation();

  if (m_bActive) {
    if (m_bFirePrimary) {
      m_bFirePrimary=false;
      m_iLastShot1=iStep;
      //We add a new missile...
      irr::core::vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(m_bLeft?-3.0f:3.0f,-0.6f,-2.0f)),
                           rot=m_pBody->getAbsoluteTransformation().getRotationDegrees(),
                           vel=m_pBody->getLinearVelocity();

      CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true,this);
      p->setTarget(m_pTargetSelector->getTarget());
      m_bLeft=!m_bLeft;
      m_iShotsFired++;
      if (m_bActive) m_pCockpit->setShotsFired(m_iShotsFired);
    }

    if (m_pCockpit!=NULL) {
      irr::core::vector3df vPos=m_pBody->getPosition();

      m_pCockpit->setAltitude(vPos.Y);
      m_pCockpit->setSpeed(m_pAero->getForewardVel());
      m_pCockpit->setPower(100.0f*m_fThrust);
      m_pCockpit->setVelVert(m_pBody->getLinearVelocity().Y);

      irr::core::vector3df v=m_pBody->getRotation().rotationToDirection(m_pAero->getForeward());
      irr::core::vector2df vDir=irr::core::vector2df(v.X,v.Z);

      if (v.getLength()>0.01f) m_pCockpit->setHeading(vDir.getAngle());

      m_pCockpit->setWarnStateHeli(0,m_pAutoPilot->isEnabled()?m_pAutoPilot->getState()==CAutoPilot::eApHeliLowAlt?2:1:0);
      m_pCockpit->setWarnStateHeli(1,vPos.Y<200.0f?3:vPos.Y<400.0f?2:1);

      v=m_pBody->getAbsoluteTransformation().getRotationDegrees();
      m_pCockpit->setHorizon(v,v.rotationToDirection(irr::core::vector3df(0.0f,1.0f,0.0f)));

      irr::ode::CIrrOdeBody *pTarget=m_pTargetSelector->getTarget();

      if (pTarget!=NULL) {
        irr::core::stringw s=irr::core::stringw(pTarget->getName());
        m_pCockpit->setTargetName(s.c_str());
        m_pCockpit->setTargetDist((vPos-pTarget->getPosition()).getLength());
      }
      else {
        m_pCockpit->setTargetName(L"<no target>");
        m_pCockpit->setTargetDist(0.0f);
      }

      if (m_bActive) {
        if (m_iHitsScored != m_iOldHitsScored) m_pCockpit->setHitsScored(m_iHitsScored);
        if (m_iHitsTaken  != m_iOldHitsTaken ) m_pCockpit->setHitsTaken (m_iHitsTaken );
      }

      m_iOldHitsTaken  = m_iHitsTaken ;
      m_iOldHitsScored = m_iHitsScored;


      m_pTab->setVisible(false);
      m_pCockpit->update(false);
      m_pTab->setVisible(true);
    }

    irr::core::vector3df cRot=m_pBody->getAbsoluteTransformation().getRotationDegrees(),
                         cPos=m_pBody->getAbsolutePosition()+cRot.rotationToDirection(irr::core::vector3df(1.0f,1.35f,2.5f)),
                         cTgt=cPos+cRot.rotationToDirection(irr::core::vector3df(0.0f,0.0f,1.0f)),
                         cUp=cRot.rotationToDirection(irr::core::vector3df(0.0f,1.0f,0.0f));

    if (m_pRView) {
      m_pRView->setCameraParameters(cPos,cTgt,cUp);
      m_pRView->update(true);
    }
  }

  m_fSound=0.75f+0.5*m_pMotor->getPower();
  if (m_bDataChanged) {
    dataChanged();
    m_bDataChanged=false;
  }
}

void CHeli::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

irr::ode::IIrrOdeEvent *CHeli::writeEvent() {
  return new CEventHeliState(m_iNodeId,m_fSound);
}

irr::ode::eEventWriterType CHeli::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

void CHeli::activate() {
  CAeroVehicle::activate();
  dataChanged();
}

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

CHeli::CHeli(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, CRearView *pRView) : CAeroVehicle(pDevice,pNode,pCtrl,pCockpit,pRView) {
  m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor,m_pRay);

  m_pAutoPilot->setState(CAutoPilot::eApHeliLowAlt);
  m_pAutoPilot->setAutoPilotInfo(m_pApInfo);

  m_pTargetSelector=new CTargetSelector(m_pBody,m_pDevice,m_pAero->getForeward());

  m_bLeft=false;
  m_fSound=0.0f;
  m_iNodeId=pNode->getID();

  CCustomEventReceiver::getSharedInstance()->addHeli(pNode);
}

CHeli::~CHeli() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

u32 CHeli::update() {
  u32 iRet=CIrrOdeCarState::update();

  vector3df rot=m_pBody->getRotation();

  //get the parameters for the camera
  vector3df pos,tgt,
            up =m_pBody->getRotation().rotationToDirection(vector3df(0,0.1,0));

  if (m_bInternal) {
    core::vector2df lookAt=core::vector2df(0,-5).rotateBy(m_fCamAngleH),
                    lookUp=core::vector2df(5, 0).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(core::vector3df(0,-0.15,-1.3));
    tgt=rot.rotationToDirection(core::vector3df(lookAt.X,-0.15+lookUp.Y,lookAt.Y));
  }
  else {
    core::vector2df lookAt=core::vector2df(0,15).rotateBy(m_fCamAngleH),
                    lookUp=core::vector2df(-15,0).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(vector3df(lookAt.X,5+lookUp.Y,lookAt.Y)),
    tgt=m_pBody->getRotation().rotationToDirection(vector3df(0,5  ,0));
  }

  CProjectileManager *ppm=CProjectileManager::getSharedInstance();

  //if we follow a bomb we focus the last dropped bomb ...
  if (m_bWeaponCam && ppm->getLast()!=NULL) {
    pos=ppm->getLast()->getBody()->getRotation().rotationToDirection(vector3df(0,5,10));
    m_pCam->setPosition(ppm->getLast()->getBody()->getPosition()+pos);
    m_pCam->setUpVector(vector3df(0,1,0));
    m_pCam->setTarget(ppm->getLast()->getBody()->getPosition());
  }
  else {  //... otherwise we focus the plane
    m_pCam->setPosition(m_pBody->getPosition()+pos);
    m_pCam->setUpVector(up);
    m_pCam->setTarget(m_pBody->getPosition()+tgt);
  }

  //now show some interesting information
  wchar_t dummy[0xFF];
  pos=m_pBody->getAbsolutePosition();
  rot=m_pBody->getRotation();
  vector3df v=m_pBody->getLinearVelocity();
  swprintf(dummy,0xFE,L"pos: (%.0f, %.0f, %.0f)\nvelocity: (%.0f, %.0f, %.0f) %.2f",pos.X,pos.Y,pos.Z,v.X,v.Y,v.Z,m_pAero->getForewardVel());
  swprintf(dummy,0xFE,L"%s\nrotation: (%.0f, %.0f, %.0f)",dummy,rot.X,rot.Y,rot.Z);
  swprintf(dummy,0xFE,L"%s\nrudder: (%.0f%%, %.0f%%, %.0f%%)\n",dummy,m_fPitch*100.0f,m_fRoll*100.0f,m_fYaw*100.0f);

  swprintf(dummy,0xFE,L"%sThrust=%.0f%%",dummy,m_fThrust*100.0f);

  f32 fHeight=pos.Y-m_pTerrain->getHeight(pos.X,pos.Z);
  swprintf(dummy,0xFE,L"%s\nAltitude: %.0f",dummy,fHeight);

  if (m_bWeaponCam) swprintf(dummy,0xFE,L"%s\nFollow Missile Cam",dummy);
  if (m_pAutoPilot->isEnabled()) swprintf(dummy,0xFE,L"%s\nAutopilot active (%i, %.0f)",dummy,m_iNextCp,m_fApDist);

  m_pInfo->setText(dummy);

  return iRet;
}

void CHeli::odeStep(u32 iStep) {
  vector3df rot=m_pBody->getRotation();

  if (m_bActive) {
    if (m_bFirePrimary) {
      m_bFirePrimary=false;
      m_iLastShot1=iStep;
      //We add a new missile...
      vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(vector3df(m_bLeft?-3.0f:3.0f,-0.6f,-2.0f)),
                rot=m_pBody->getAbsoluteTransformation().getRotationDegrees(),
                vel=m_pBody->getLinearVelocity();

      CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true,this);
      p->setTarget(m_pTargetSelector->getTarget());
      m_bLeft=!m_bLeft;
      m_iShotsFired++;
    }

    if (m_pCockpit!=NULL) {
      core::vector3df vPos=m_pBody->getPosition();

      m_pCockpit->setAltitude(vPos.Y);
      m_pCockpit->setSpeed(m_pAero->getForewardVel());
      m_pCockpit->setPower(100.0f*m_fThrust);
      m_pCockpit->setVelVert(m_pBody->getLinearVelocity().Y);

      core::vector3df v=m_pBody->getRotation().rotationToDirection(m_pAero->getForeward());
      core::vector2df vDir=core::vector2df(v.X,v.Z);

      if (v.getLength()>0.01f) m_pCockpit->setHeading(vDir.getAngle());

      m_pCockpit->setWarnStateHeli(0,m_pAutoPilot->isEnabled()?m_pAutoPilot->getState()==CAutoPilot::eApHeliLowAlt?2:1:0);
      m_pCockpit->setWarnStateHeli(1,vPos.Y<200.0f?3:vPos.Y<400.0f?2:1);

      v=m_pBody->getAbsoluteTransformation().getRotationDegrees();
      m_pCockpit->setHorizon(v,v.rotationToDirection(core::vector3df(0.0f,1.0f,0.0f)));

      ode::CIrrOdeBody *pTarget=m_pTargetSelector->getTarget();

      if (pTarget!=NULL) {
        core::stringw s=core::stringw(pTarget->getName());
        m_pCockpit->setTargetName(s.c_str());
        m_pCockpit->setTargetDist((vPos-pTarget->getPosition()).getLength());
      }
      else {
        m_pCockpit->setTargetName(L"<no target>");
        m_pCockpit->setTargetDist(0.0f);
      }

      m_pCockpit->setShotsFired(m_iShotsFired);
      m_pCockpit->setHitsScored(m_iHitsScored);
      m_pCockpit->setHitsTaken (m_iHitsTaken );


      m_pTab->setVisible(false);
      m_pCockpit->update(false);
      m_pTab->setVisible(true);
    }

    core::vector3df cRot=m_pBody->getAbsoluteTransformation().getRotationDegrees(),
                    cPos=m_pBody->getAbsolutePosition()+cRot.rotationToDirection(core::vector3df(1.0f,1.35f,2.5f)),
                    cTgt=cPos+cRot.rotationToDirection(core::vector3df(0.0f,0.0f,1.0f)),
                    cUp=cRot.rotationToDirection(core::vector3df(0.0f,1.0f,0.0f));

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

ode::IIrrOdeEvent *CHeli::writeEvent() {
  return new CEventHeliState(m_iNodeId,m_fSound);
}

ode::eEventWriterType CHeli::getEventWriterType() {
  return ode::eIrrOdeEventWriterUnknown;
}

void CHeli::activate() {
  CAeroVehicle::activate();
  dataChanged();
}

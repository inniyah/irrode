  #include "CPlane.h"
  #include "CCustomEventReceiver.h"
  #include <math.h>
  #include <CProjectile.h>
  #include <irrCC.h>
  #include <CAutoPilot.h>
  #include <CTargetSelector.h>
  #include <CCockpitPlane.h>
  #include <irrlicht.h>
  #include <irrKlang.h>
  #include <CRearView.h>
  #include <CIrrOdeCarTrack.h>

using namespace irr;

CPlane::CPlane(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, CRearView *pRView) : CAeroVehicle(pDevice,pNode,pCtrl,pCockpit,pRView) {

  CCustomEventReceiver::getSharedInstance()->addPlane(m_pBody);
  //get the visual rudders
  m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor);
  m_pAutoPilot->setLinkYawRoll(true);

  m_pTargetSelector=new CTargetSelector(m_pBody,m_pDevice,m_pAero->getForeward());

  printf("\nplane state\n\n");
  m_bLeftMissile=true;

  m_iNextCp=-1;
  ISceneNode *pCheckRoot=m_pSmgr->getSceneNodeFromName("planepoints");
  if (pCheckRoot!=NULL) {
    irr::core::list<irr::scene::ISceneNode *> lChildren=pCheckRoot->getChildren();
    irr::core::list<irr::scene::ISceneNode *>::Iterator it;

    for (it=lChildren.begin(); it!=lChildren.end(); it++) m_aCheckPoints.push_back(*it);
    printf("%i checkpoints for plane found!\n",m_aCheckPoints.size());
  }
  else printf("no checkpoints for plane found!\n");

  for (u32 i=0; i<2; i++) {
    c8 s[0xFF];
    sprintf(s,"axis%i",i+1);
    m_pAxes[i]=(irr::ode::CIrrOdeJointHinge *)m_pBody->getChildByName(s,m_pBody);
    printf("%s: %i\n",s,(int)m_pAxes[i]);
    m_fAngleRate[i]=0.0f;
  }
  m_fAngleRate[2]=0.0f;

  m_pSteerAxis=(irr::ode::CIrrOdeJointHinge2 *)m_pBody->getChildByName("axisSteer",m_pBody);
  printf("steer axis: %i\n",(int)m_pSteerAxis);

  m_iOldHitsScored = -1;
  m_iOldHitsTaken = -1;

  m_pLap = new CIrrOdeCarTrack(m_pBody);
}

CPlane::~CPlane() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

u32 CPlane::update() {
  //call the superclass's update method
  u32 iRet=CIrrOdeCarState::update();

  vector3df rot=m_pBody->getRotation();

  //get the parameters for the camera
  vector3df pos,tgt,up=m_pBody->getRotation().rotationToDirection(vector3df(0,0.1,0));

  if (m_bInternal) {
    core::vector2df lookAt=core::vector2df(0.0f,-5.0f).rotateBy(m_fCamAngleH),
                    lookUp=core::vector2df(5.0f, 0.0f).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(vector3df(0,1.1,-0.6)),
    up =rot.rotationToDirection(vector3df(0,0.1,0));
    tgt=rot.rotationToDirection(vector3df(lookAt.X,1.1+lookUp.Y,lookAt.Y));
  }
  else {
    core::vector2df lookAt=core::vector2df(  0.0f,15.0f).rotateBy(m_fCamAngleH),
                    lookUp=core::vector2df(-15.0f, 0.0f).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(vector3df(lookAt.X,5.0f+lookUp.Y,lookAt.Y)),
    up =rot.rotationToDirection(vector3df(0,0.1,0));
    tgt=rot.rotationToDirection(vector3df(0,5,0));
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

  return iRet;
}

void CPlane::odeStep(u32 iStep) {
  if (m_bActive) {
    if (m_bFirePrimary) {
      m_iLastShot1=iStep;
      //We add a new bomb...
      vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(vector3df(m_bLeftMissile?4.0f:-4.0f,-1.0f,-2.0f)),
                rot=m_pBody->getRotation(),vel=m_pBody->getLinearVelocity();

      m_bLeftMissile=!m_bLeftMissile;

      CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true,this);
      p->setTarget(m_pTargetSelector->getTarget());
      m_iShotsFired++;
      m_pCockpit->setShotsFired(m_iShotsFired);
      m_bFirePrimary=false;
    }

    if (m_bFireSecondary) {
      m_iLastShot2=iStep;
      //We add a new bullet...
      vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(vector3df(0.2f,1.5f,-11.0f)),
                rot=m_pBody->getRotation(),
                vel=m_pBody->getLinearVelocity().getLength()*m_pBody->getRotation().rotationToDirection(vector3df(0.0f,0.0f,1.0f))+m_pBody->getRotation().rotationToDirection(vector3df(0.0f,0.0f,-350.0f));

      new CProjectile(m_pSmgr,pos,rot,vel,"bullet",600,m_pWorld,true,this);
      m_iShotsFired++;
      m_bFireSecondary=false;
    }

    if (m_pCockpit!=NULL) {
      core::vector3df vPos=m_pBody->getPosition();
      f32 fSpeed=m_pBody->getLinearVelocity().getLength();

      m_pCockpit->setAltitude(vPos.Y);
      m_pCockpit->setSpeed(fSpeed);
      m_pCockpit->setPower(100.0f*m_fThrust);
      m_pCockpit->setVelVert(m_pBody->getLinearVelocity().Y);

      core::vector3df v=m_pBody->getRotation().rotationToDirection(m_pAero->getForeward());
      core::vector2df vDir=core::vector2df(v.X,v.Z);

      if (v.getLength()>0.01f) m_pCockpit->setHeading(vDir.getAngle());

      m_pCockpit->setWarnStatePlane(0,m_pAutoPilot->isEnabled()?m_pAutoPilot->getState()==CAutoPilot::eApPlaneLowAlt?2:1:0);
      m_pCockpit->setWarnStatePlane(1,vPos.Y<300.0f?3:vPos.Y<550.0f?2:1);
      m_pCockpit->setWarnStatePlane(2,m_pBrakes[0]->getForce()>20.0f?2:1);
      m_pCockpit->setWarnStatePlane(3,fSpeed<5.0f?0:fSpeed<30.0f?3:fSpeed<45.0f?2:1);

      v=m_pBody->getAbsoluteTransformation().getRotationDegrees();
      m_pCockpit->setHorizon(v,v.rotationToDirection(core::vector3df(0.0f,1.0f,0.0f)));

      irr::ode::CIrrOdeBody *pTarget=m_pTargetSelector->getTarget();

      if (pTarget!=NULL) {
        core::stringw s=core::stringw(pTarget->getName());
        m_pCockpit->setTargetName(s.c_str());
        m_pCockpit->setTargetDist((vPos-pTarget->getPosition()).getLength());
      }
      else {
        m_pCockpit->setTargetName(L"<no target>");
        m_pCockpit->setTargetDist(0.0f);
      }

      if (m_iHitsScored != m_iOldHitsScored) m_pCockpit->setHitsScored(m_iHitsScored);
      if (m_iHitsTaken  != m_iOldHitsTaken ) m_pCockpit->setHitsTaken (m_iHitsTaken );

      m_iOldHitsTaken  = m_iHitsTaken ;
      m_iOldHitsScored = m_iHitsScored;

      m_pTab->setVisible(false);
      m_pCockpit->update(true);
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

  if (m_bDataChanged) {
    dataChanged();
    m_bDataChanged=false;
  }

  for (u32 i=0; i<2; i++) {
    if (m_pAxes[i]) {
      f32 f=m_pAxes[i]->getHingeAngleRate(),fImpulse=m_fAngleRate[i]-f;
      if (fImpulse<0.0f) fImpulse=-fImpulse;
      if (fImpulse>15.0f) {
        f32 fVol=(fImpulse-15.0f)/75.0f;
        if (fVol>1.0f) fVol=1.0f;
        core::vector3df irrPos=m_pAxes[i]->getAbsolutePosition();
        CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndSkid,fVol,irrPos);
        irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
      }
      m_fAngleRate[i]=f;
    }
  }

  if (m_pSteerAxis) {
    f32 f=m_pSteerAxis->getHingeAngle2Rate(),fImpulse=m_fAngleRate[2]-f;;
    if (fImpulse<0.0f) fImpulse=-fImpulse;
    if (fImpulse>15.0f) {
      f32 fVol=(fImpulse-15.0f)/75.0f;
      if (fVol>1.0f) fVol=1.0f;
      core::vector3df irrPos=m_pSteerAxis->getAbsolutePosition();
      CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndSkid,fVol,irrPos);
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
    }
    m_fAngleRate[2]=f;
  }
}

void CPlane::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

irr::ode::IIrrOdeEvent *CPlane::writeEvent() {
  CEventPlaneState *p=new CEventPlaneState(m_pBody->getID(),m_fYaw,m_fPitch,m_fRoll,m_pMotor->getPower(),m_bThreeWheeler);
  return p;
}

irr::ode::eEventWriterType CPlane::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

void CPlane::activate() {
  CAeroVehicle::activate();
  dataChanged();
}

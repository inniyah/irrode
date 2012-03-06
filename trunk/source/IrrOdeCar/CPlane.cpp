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

CPlane::CPlane(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, CRearView *pRView) : CAeroVehicle(pDevice,pNode,pCtrl,pRView) {

  CCustomEventReceiver::getSharedInstance()->addPlane(m_pBody);
  //get the visual rudders
  m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor);
  m_pAutoPilot->setLinkYawRoll(true);

  m_pTargetSelector=new CTargetSelector(m_pBody,m_pDevice,m_pAero->getForeward());

  printf("\nplane state\n\n");
  m_bLeftMissile=true;

  m_iNextCp=-1;
  irr::scene::ISceneNode *pCheckRoot=m_pSmgr->getSceneNodeFromName("planepoints");
  if (pCheckRoot!=NULL) {
    irr::core::list<irr::scene::ISceneNode *> lChildren=pCheckRoot->getChildren();
    irr::core::list<irr::scene::ISceneNode *>::Iterator it;

    for (it=lChildren.begin(); it!=lChildren.end(); it++) m_aCheckPoints.push_back(*it);
    printf("%i checkpoints for plane found!\n",m_aCheckPoints.size());
  }
  else printf("no checkpoints for plane found!\n");

  for (irr::u32 i=0; i<2; i++) {
    irr::c8 s[0xFF];
    sprintf(s,"axis%i",i+1);
    m_pAxes[i]=(irr::ode::CIrrOdeJointHinge *)m_pBody->getChildByName(s,m_pBody);
    printf("%s: %i\n",s,(int)m_pAxes[i]);
    m_fAngleRate[i]=0.0f;
  }
  m_fAngleRate[2]=0.0f;

  m_pSteerAxis=(irr::ode::CIrrOdeJointHinge2 *)m_pBody->getChildByName("axisSteer",m_pBody);
  printf("steer axis: %i\n",(int)m_pSteerAxis);

  m_pLap = new CIrrOdeCarTrack(m_pBody);

  m_bAutoPilot = false;
  dataChanged();
}

CPlane::~CPlane() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CPlane::odeStep(irr::u32 iStep) {
  if (m_bActive) {
    if (m_bFirePrimary) {
      m_iLastShot1=iStep;
      //We add a new bomb...
      irr::core::vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(m_bLeftMissile?4.0f:-4.0f,-1.0f,-2.0f)),
                rot=m_pBody->getRotation(),vel=m_pBody->getLinearVelocity();

      m_bLeftMissile=!m_bLeftMissile;

      CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true,this);
      p->setTarget(m_pTargetSelector->getTarget());
      incShotsFired();
      //if (m_bActive) m_pCockpit->setShotsFired(m_iShotsFired);
      m_bFirePrimary=false;
    }

    if (m_bFireSecondary) {
      m_iLastShot2=iStep;
      //We add a new bullet...
      irr::core::vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(0.2f,1.5f,-11.0f)),
                rot=m_pBody->getRotation(),
                vel=m_pBody->getLinearVelocity().getLength()*m_pBody->getRotation().rotationToDirection(irr::core::vector3df(0.0f,0.0f,1.0f))+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(0.0f,0.0f,-350.0f));

      new CProjectile(m_pSmgr,pos,rot,vel,"bullet",600,m_pWorld,true,this);
      incShotsFired();
      m_bFireSecondary=false;
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

  if (m_bDataChanged) {
    dataChanged();
    m_bDataChanged=false;
  }

  for (irr::u32 i=0; i<2; i++) {
    if (m_pAxes[i]) {
      irr::f32 f=m_pAxes[i]->getHingeAngleRate(),fImpulse=m_fAngleRate[i]-f;
      if (fImpulse<0.0f) fImpulse=-fImpulse;
      if (fImpulse>15.0f) {
        irr::f32 fVol=(fImpulse-15.0f)/75.0f;
        if (fVol>1.0f) fVol=1.0f;
        irr::core::vector3df irrPos=m_pAxes[i]->getAbsolutePosition();
        CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndSkid,fVol,irrPos);
        irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
      }
      m_fAngleRate[i]=f;
    }
  }

  if (m_pSteerAxis) {
    irr::f32 f=m_pSteerAxis->getHingeAngle2Rate(),fImpulse=m_fAngleRate[2]-f;;
    if (fImpulse<0.0f) fImpulse=-fImpulse;
    if (fImpulse>15.0f) {
      irr::f32 fVol=(fImpulse-15.0f)/75.0f;
      if (fVol>1.0f) fVol=1.0f;
      irr::core::vector3df irrPos=m_pSteerAxis->getAbsolutePosition();
      CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndSkid,fVol,irrPos);
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
    }
    m_fAngleRate[2]=f;
  }

  if (m_bAutoPilot != m_pAutoPilot->isEnabled()) {
    m_bAutoPilot = m_pAutoPilot->isEnabled();
    dataChanged();
  }
}

void CPlane::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

irr::ode::IIrrOdeEvent *CPlane::writeEvent() {
  CEventPlaneState *p=new CEventPlaneState(m_pBody->getID(),m_fYaw,m_fPitch,m_fRoll,m_pMotor->getPower(),m_bThreeWheeler,m_pBrakes[0]->getForce()>20.0f,m_pAutoPilot->isEnabled(),m_fThrust);
  return p;
}

irr::ode::eEventWriterType CPlane::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

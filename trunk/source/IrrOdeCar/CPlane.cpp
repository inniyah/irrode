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

using namespace irr;

CPlane::CPlane(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, irrklang::ISoundEngine *pSndEngine) : CAeroVehicle(pDevice,pNode,pCtrl,pCockpit,pSndEngine) {

  CCustomEventReceiver::getSharedInstance()->addPlane(m_pBody);
  //get the visual rudders
  m_pRoll [0]=m_pSmgr->getSceneNodeFromName("roll1");
  m_pRoll [1]=m_pSmgr->getSceneNodeFromName("roll2");
  m_pPitch[0]=m_pSmgr->getSceneNodeFromName("pitch1");
  m_pPitch[1]=m_pSmgr->getSceneNodeFromName("pitch2");
  m_pYaw  [0]=m_pSmgr->getSceneNodeFromName("yaw");
  m_pYaw  [1]=m_pSmgr->getSceneNodeFromName("yaw2");

  m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor);
  m_pAutoPilot->setAutoPilotInfo(m_pApInfo);
  m_pAutoPilot->setLinkYawRoll(true);

  m_pTargetSelector=new CTargetSelector(m_pBody,m_pDevice,m_pAero->getForeward());

  printf("\nplane state\n\n");
  printf("rudders: (%i, %i), (%i, %i), (%i, %i)\n",(int)m_pRoll [0],(int)m_pRoll [1],
                                                   (int)m_pPitch[0],(int)m_pPitch[1],
                                                   (int)m_pYaw  [0],(int)m_pYaw  [1]);
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

  m_pSound=m_pSndEngine->play3D("../../data/sound/plane.ogg",irrklang::vec3df(0.0f,0.0f,0.0f),true,true);
  m_pSound->setMinDistance(10.0f);
  m_pSound->setMaxDistance(100.0f);
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

  if (m_bWeaponCam) swprintf(dummy,0xFE,L"%s\nFollow Weapon Cam",dummy);
  if (m_pAutoPilot->isEnabled()) swprintf(dummy,0xFE,L"%s\nAutopilot active (%i, %.0f)",dummy,m_iNextCp,m_fApDist);

  m_pInfo->setText(dummy);

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
      m_pCockpit->setHits(m_iHits);

      m_pTab->setVisible(false);
      m_pCockpit->update(true);
      m_pTab->setVisible(true);
    }
  }

  if (m_bRudderChanged) {
    CEventRudderPositions *p=new CEventRudderPositions(m_pBody->getID(),m_fYaw,m_fPitch,m_fRoll,m_bThreeWheeler);
    ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
    m_bRudderChanged=false;
  }

  if (m_pSndEngine!=NULL && m_pSound!=NULL) {
    core::vector3df irrPos=m_pBody->getPosition(),
                    irrVel=m_pBody->getLinearVelocity();

    irrklang::vec3df vPos=irrklang::vec3df(irrPos.X,irrPos.Y,irrPos.Z),
                     vVel=irrklang::vec3df(irrVel.X,irrVel.Y,irrVel.Z);

    m_pSound->setVelocity(vVel);
    m_pSound->setPosition(vPos);
    m_pSound->setPlaybackSpeed(0.75f+0.5*m_pMotor->getPower());
  }
}

void CPlane::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

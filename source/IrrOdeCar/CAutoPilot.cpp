  #include <CAutoPilot.h>
  #include <IrrOde.h>
  #include <irrlicht.h>

using namespace irr;

CAutoPilot::CAutoPilot(irr::ode::CIrrOdeBody         *pPlane,
                                 irr::ode::CIrrOdeAeroDrag     *pAero,
                                 irr::ode::CIrrOdeTorqueMotor  *pTorque,
                                 irr::ode::CIrrOdeImpulseMotor *pMotor) {
  m_pTarget=NULL;
  m_pTargetBody=NULL;

  m_pPlane=pPlane;
  m_pAero=pAero;
  m_pTorque=pTorque;
  m_pMotor=pMotor;

  m_iState=eApPlaneCruise;

  m_vArtificialHorizon=irr::core::vector2di(25.0f,0.0f);

  m_bEnabled=false;
  m_bLinkYawRoll=false;
}

void CAutoPilot::setTarget(irr::scene::ISceneNode *pTarget) {
  m_pTarget=pTarget;

  if (pTarget!=NULL) {
    if (pTarget->getType()==irr::ode::IRR_ODE_BODY_ID)
      m_pTargetBody=(irr::ode::CIrrOdeBody *)pTarget;
    else
      m_pTargetBody=NULL;
  }
}

irr::f32 CAutoPilot::getRollControl(f32 fYaw) {
  f32 fAngle=asin(m_vSideward2.Y)*180.0f/PI,fRoll=0.0f;
  if (m_vUpward.Y<0) fAngle=-180.0f-fAngle;

  while (fAngle<-180.0f) fAngle+=360.0f;
  while (fAngle> 180.0f) fAngle-=360.0f;

  if (m_bLinkYawRoll) fRoll=(fAngle-20.0f*fYaw)*0.01f; else fRoll=fAngle*0.01f;

  if (fRoll> 1.0f) fRoll= 1.0f;
  if (fRoll<-1.0f) fRoll=-1.0f;

  core::vector2df vAhf=25.0f*core::vector2df(1.0f,0.0f).rotateBy(fAngle);
  m_vArtificialHorizon.X=vAhf.X;
  m_vArtificialHorizon.Y=vAhf.Y;

  return fRoll;
}

irr::f32 CAutoPilot::getYawControl(bool b) {
  f32 fYaw=0.0f;

  core::vector2df vPlane=core::vector2df(m_vPosition.X,m_vPosition.Z),
                  vPoint=core::vector2df(m_vCheckPos.X,m_vCheckPos.Z),
                  vVeloc=core::vector2df(m_vVelocityLin.X,m_vVelocityLin.Z),
                  vForew=core::vector2df(m_vForeward.X,m_vForeward.Z);

  m_vCpVel=vPoint-vPlane;

  irr::core::vector2df vxy,vxz;

  if (b) {
    vxy=vVeloc+m_vCpVel;
    vxz=vVeloc-m_vCpVel;
  }
  else {
    vxy=vForew+m_vCpVel;
    vxz=vForew-m_vCpVel;
  }

  vForew.normalize();
  vVeloc.normalize();
  m_vCpVel.normalize();

  f32 fSteer=b?vVeloc.getAngleWith(m_vCpVel):vForew.getAngleWith(m_vCpVel);

  if (vxy.getLength()-vxz.getLength()<0.0f) fSteer=180-fSteer;
  f32 fOrient=b?(vVeloc.X*m_vCpVel.Y-vVeloc.Y*m_vCpVel.X):(vForew.X*m_vCpVel.Y-vForew.Y*m_vCpVel.X);
  if (fOrient>0.0f) fSteer=-fSteer;


  m_fApDist=(vPlane-vPoint).getLength();

  m_vVATransformed=m_vInvRot.rotationToDirection(m_vVelocityAng);

  if (b && (m_vVATransformed.Y>0.1f || m_vVATransformed.Y<-0.1f)) {
    f32 fSign=m_vVATransformed.Y>0.0f?1.0f:0.0f;
    f32 fMinus=50.0f*m_vVATransformed.Y;
    fMinus*=fSign*fMinus;
    if (fSteer>0 && fMinus>fSteer) fMinus=fSteer;
    if (fSteer<0 && fMinus<fSteer) fMinus=fSteer;
    fSteer-=fMinus;
  }

  fYaw=(b?0.1f:0.05f)*fSteer;
  if (fYaw> 1.0f) fYaw= 1.0f;
  if (fYaw<-1.0f) fYaw=-1.0f;

  return fYaw;
}

void CAutoPilot::step(irr::f32 &fYaw, irr::f32 &fPitch, irr::f32 &fRoll, irr::f32 &fThrust) {
  m_vInvRot=m_pPlane->getRotation();
  m_vRotation=m_pPlane->getRotation(),
  m_vPosition=m_pPlane->getPosition();

  m_vInvRot=m_vInvRot.invert();

  m_vUpward   =m_vInvRot.rotationToDirection(m_pAero->getUpward  ());
  m_vSideward =m_vInvRot.rotationToDirection(m_pAero->getSideward());
  m_vForeward =m_vRotation.rotationToDirection(m_pAero->getForeward());
  m_vSideward2=m_vRotation.rotationToDirection(m_pAero->getSideward()),
  m_vVelocityLin=m_pPlane->getLinearVelocity();
  m_vVelocityAng=m_pPlane->getAngularVelocity();
  m_vCheckPos=m_pTarget!=NULL?m_pTarget->getPosition():core::vector3df(0.0f,0.0f,0.0f);

  m_vSideward.Y=0.0f;

  m_vUpward  .normalize();
  m_vSideward.normalize();

  if (m_pTarget!=NULL) {
    switch (m_iState) {
      case eApPlaneCruise:
        if (m_bEnabled) {
          core::vector2df vPlane=core::vector2df(m_vPosition.X,m_vPosition.Z),
                          vPoint=core::vector2df(m_vCheckPos.X,m_vCheckPos.Z),
                          vVeloc=core::vector2df(m_vVelocityLin.X,m_vVelocityLin.Z);

          m_vCpVel=vPoint-vPlane;

          irr::core::vector2df vxy=vVeloc+m_vCpVel,vxz=vVeloc-m_vCpVel;

          vVeloc.normalize();
          m_vCpVel.normalize();

          f32 fDiffY=(950-m_vPosition.Y)/10.0f;

          if (fDiffY> m_vVelocityLin.getLength()/5.0f) fDiffY= m_vVelocityLin.getLength()/5.0f;
          if (fDiffY<-m_vVelocityLin.getLength()/5.0f) fDiffY=-m_vVelocityLin.getLength()/5.0f;

          fPitch=(-m_vVelocityLin.Y+fDiffY)*0.05f;
          if (fPitch> 1.0f) fPitch= 1.0f;
          if (fPitch<-1.0f) fPitch=-1.0f;

          fThrust=m_vVelocityLin.getLength()<50.0f?1.0f:1.0f-((m_vVelocityLin.getLength()-50.0f)/25.0f);
          if (fThrust<0.0f) fThrust=0.0f;

          fYaw=getYawControl(false);
          fRoll=getRollControl(fYaw);

          if (m_vPosition.Y<800.0f) {
            printf("Autopilot new state: eApPlaneLowAlt\n");
            m_iState=eApPlaneLowAlt;
          }
        }
        break;

      case eApPlaneLowAlt:
        if (m_bEnabled) {
          fThrust=1.0f;

          fRoll=getRollControl(fYaw);

          f32 fDiffY=(950-m_vPosition.Y)/10.0f;

          if (fDiffY> m_vVelocityLin.getLength()/2.5f) fDiffY= m_vVelocityLin.getLength()/2.5f;
          if (fDiffY<-m_vVelocityLin.getLength()/2.5f) fDiffY=-m_vVelocityLin.getLength()/2.5f;

          fPitch=(-m_vVelocityLin.Y+fDiffY)*0.05f;
          if (fPitch> 1.0f) fPitch= 1.0f;
          if (fPitch<-1.0f) fPitch=-1.0f;

          if (m_vPosition.Y>800.0f) {
            printf("Autopilot new state: eApPlaneCruise\n");
            m_iState=eApPlaneCruise;
          }
        }
        break;

      case eApHeliLowAlt:
        if (m_bEnabled) {
          if (m_vPosition.Y<550.0f) {
            fThrust=1.0f;
          }
          else
            if (m_vPosition.Y>600.0f) {
              fThrust=0.0f;
            }
            else {
              fThrust=1.0f-((m_vPosition.Y-550.0f)/50.0f)-m_vVelocityLin.Y/15.0f;
              if (fThrust>1.0f) fThrust=1.0f;
              if (fThrust<0.0f) fThrust=0.0f;
            }

          fRoll=-getRollControl(fYaw);
        }
        break;

      case eApMissile:
        if (m_bEnabled && m_pTarget!=NULL) {

          if (m_pTargetBody!=NULL) {
            f32 fFact=m_fApDist>1000.0f?3.0f:3.0f*(m_fApDist/1000.0f);
            m_vCheckPos+=fFact*m_pTargetBody->getLinearVelocity();
          }

          irr::core::line3df cLine=irr::core::line3df(m_pPlane->getPosition(),m_pPlane->getPosition()+3000.0f*m_vVelocityLin);
          irr::core::vector3df vNear=cLine.getClosestPoint(m_vCheckPos);

          f32 f=m_pTarget->getPosition().Y-vNear.Y;
          if ((f>0.0f && m_vVATransformed.X>0.01f) || (f<0.0f && m_vVATransformed.X<-0.01f))  f-=100.0f*m_vVATransformed.X;
          fPitch=(f)/50.0f;

          if (fPitch> 1.0f) fPitch= 1.0f;
          if (fPitch<-1.0f) fPitch=-1.0f;

          fYaw=getYawControl(true);
          fRoll=getRollControl(fYaw);
        }
        break;
    }
  }
}

void CAutoPilot::drawSpecifics(irr::video::IVideoDriver *pDrv) {
  core::vector3df vRot=m_pPlane->getRotation(),
                  vDir=vRot.rotationToDirection(m_pAero->getForeward()),
                  vPos=m_pPlane->getPosition(),
                  vVel=m_pPlane->getLinearVelocity(),
                  vFw=vRot.rotationToDirection(m_pAero->getForeward());

  vDir.normalize();
  vDir*=25.0f;
  vDir.Z=-vDir.Z;
  vVel.normalize();

  core::vector2di vCenter=core::vector2di(50,150);

  if (m_pTarget!=NULL && m_bEnabled) {
    core::vector2df vTarget=m_vCpVel;
    vTarget.normalize();
    vTarget*=25.0f;
    vTarget.Y=-vTarget.Y;
    pDrv->draw2DLine(vCenter,vCenter+core::vector2di(vTarget.X,vTarget.Y),video::SColor(0xFF,192,255,192));

    irr::core::CMatrix4<f32> mat=irr::core::CMatrix4<f32>();
    pDrv->setTransform(irr::video::ETS_WORLD,mat);
    irr::video::SMaterial cMat;
    cMat.Lighting=false;
    pDrv->setMaterial(cMat);

    irr::core::line3df cLine=irr::core::line3df(vPos,vPos+3000.0f*vVel);//vFw);
    pDrv->draw3DLine(cLine.start,cLine.end,video::SColor(0xFF,0xFF,0xFF,0xFF));
    irr::core::vector3df vNear=cLine.getClosestPoint(m_pTarget->getPosition());
    pDrv->draw3DLine(vNear,m_pTarget->getPosition(),video::SColor(0xFF,0xFF,0,0));
  }

  core::vector2di vHead=core::vector2di(vDir.X,vDir.Z);
  pDrv->draw2DLine(vCenter,vCenter+vHead,video::SColor(0xFF,0xFF,0,0));

  vCenter=core::vector2di(50,250);

  pDrv->draw2DLine(vCenter,vCenter-m_vArtificialHorizon,video::SColor(0xFF,0xFF,0,0));
  pDrv->draw2DLine(vCenter,vCenter+m_vArtificialHorizon,video::SColor(0xFF,0,0xFF,0));
}

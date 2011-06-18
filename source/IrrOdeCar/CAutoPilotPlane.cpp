  #include <CAutoPilotPlane.h>
  #include <IrrOde.h>

CAutoPilotPlane::CAutoPilotPlane(irr::ode::CIrrOdeBody         *pPlane,
                                 irr::ode::CIrrOdeAeroDrag     *pAero,
                                 irr::ode::CIrrOdeTorqueMotor  *pTorque,
                                 irr::ode::CIrrOdeImpulseMotor *pMotor) {
  m_pTarget=NULL;
  m_pTargetBody=NULL;

  m_pPlane=pPlane;
  m_pAero=pAero;
  m_pTorque=pTorque;
  m_pMotor=pMotor;

  m_iState=0;

  m_vArtificialHorizon=irr::core::vector2di(25.0f,0.0f);

  m_bEnabled=false;
  m_bLinkYawRoll=false;
}

void CAutoPilotPlane::setTarget(irr::scene::ISceneNode *pTarget) {
  m_pTarget=pTarget;

  if (pTarget!=NULL) {
    if (pTarget->getType()==irr::ode::IRR_ODE_BODY_ID)
      m_pTargetBody=(irr::ode::CIrrOdeBody *)pTarget;
    else
      m_pTargetBody=NULL;
  }
}

void CAutoPilotPlane::step(irr::f32 &fYaw, irr::f32 &fPitch, irr::f32 &fRoll, irr::f32 &fThrust) {
  core::vector3df vInv=m_pPlane->getRotation(),
                  vRot=m_pPlane->getRotation(),
                  vPos=m_pPlane->getPosition();

  vInv=vInv.invert();

  core::vector3df vUp=vInv.rotationToDirection(m_pAero->getUpward  ()),
                  vSi=vInv.rotationToDirection(m_pAero->getSideward()),
                  vFw=vRot.rotationToDirection(m_pAero->getForeward());

  vSi.Y=0.0f;

  vUp.normalize();
  vSi.normalize();

  if (m_pTarget!=NULL) {
    core::vector3df vCheckPos=m_pTarget->getPosition(),
                    vPlaneVel=m_pPlane->getLinearVelocity();

    core::vector2df vPlane=core::vector2df(vPos.X,vPos.Z),
                    vPoint=core::vector2df(vCheckPos.X,vCheckPos.Z),
                    vVeloc=core::vector2df(vPlaneVel.X,vPlaneVel.Z);

    m_vCpVel=vPoint-vPlane;

    irr::core::vector2df vxy=vVeloc+m_vCpVel,vxz=vVeloc-m_vCpVel;

    vVeloc.normalize();
    m_vCpVel.normalize();

    f32 fSteer=vVeloc.getAngleWith(m_vCpVel);

    if (vxy.getLength()-vxz.getLength()<0.0f) fSteer=180-fSteer;
    f32 fOrient=vVeloc.X*m_vCpVel.Y-vVeloc.Y*m_vCpVel.X;
    if (fOrient>0.0f) fSteer=-1*fSteer; else fSteer=fSteer;

    m_fApDist=(vPlane-vPoint).getLength();

    switch (m_iState) {
      case 0:
        if (m_bEnabled) {
          fYaw=0.02f*fSteer;

          if (fYaw> 1.0f) fYaw= 1.0f;
          if (fYaw<-1.0f) fYaw=-1.0f;

          f32 vy=m_pPlane->getLinearVelocity().Y,
              fDiffY=(950-vPos.Y)/10.0f;

          if (fDiffY> vPlaneVel.getLength()/5.0f) fDiffY= vPlaneVel.getLength()/5.0f;
          if (fDiffY<-vPlaneVel.getLength()/5.0f) fDiffY=-vPlaneVel.getLength()/5.0f;

          fPitch=(-vy+fDiffY)*0.05f;
          if (fPitch> 1.0f) fPitch= 1.0f;
          if (fPitch<-1.0f) fPitch=-1.0f;
        }
        break;

      case 1:
        if (m_bEnabled && m_pTarget!=NULL) {
          vRot=m_pPlane->getRotation();

          core::vector3df vAngVel=m_pPlane->getAngularVelocity(),
                          vInvRot=vRot.invert(),
                          vVel=m_pPlane->getLinearVelocity(),
                          vAng2=vInvRot.rotationToDirection(vAngVel),
                          vTgtPos=m_pTarget->getPosition();

          if (m_pTargetBody!=NULL) vTgtPos+=m_pTargetBody->getLinearVelocity();

          vVel.normalize();
          if (vAng2.Y>0.1f || vAng2.Y<-0.1f) fSteer-=5.0f*vAng2.Y;

          fYaw=0.1f*fSteer;
          if (fYaw> 1.0f) fYaw= 1.0f;
          if (fYaw<-1.0f) fYaw=-1.0f;

          irr::core::line3df cLine=irr::core::line3df(m_pPlane->getPosition(),m_pPlane->getPosition()+3000.0f*vVel);
          irr::core::vector3df vNear=cLine.getClosestPoint(vTgtPos);

          f32 f=m_pTarget->getPosition().Y-vNear.Y;
          if (vAng2.X>0.1f || vAng2.X<-0.1f) f-=5.0f*vAng2.X;
          fPitch=(f)/30.0f;

          if (fPitch> 1.0f) fPitch= 1.0f;
          if (fPitch<-1.0f) fPitch=-1.0f;
        }
        break;
    }

    if (m_bEnabled) {
      fThrust=vPlaneVel.getLength()<50.0f?1.0f:1.0f-((vPlaneVel.getLength()-50.0f)/25.0f);
      if (fThrust<0.0f) fThrust=0.0f;
    }
  }

  {
    core::vector3df vRot=m_pPlane->getRotation(),
                    vSid=vRot.rotationToDirection(m_pAero->getSideward()),
                    vUp=vRot.rotationToDirection(m_pAero->getUpward());

    f32 fAngle=asin(vSid.Y)*180.0f/PI;
    if (vUp.Y<0) fAngle=-180.0f-fAngle;

    while (fAngle<-180.0f) fAngle+=360.0f;
    while (fAngle> 180.0f) fAngle-=360.0f;

    if (m_bEnabled) {
      if (m_bLinkYawRoll) fRoll=(fAngle-30.0f*fYaw)*0.01f; else fRoll=fAngle*0.01f;
      if (fRoll> 1.0f) fRoll= 1.0f;
      if (fRoll<-1.0f) fRoll=-1.0f;
    }

    core::vector2df vAhf=25.0f*core::vector2df(1.0f,0.0f).rotateBy(fAngle);
    m_vArtificialHorizon.X=vAhf.X;
    m_vArtificialHorizon.Y=vAhf.Y;
  }
}

void CAutoPilotPlane::drawSpecifics(irr::video::IVideoDriver *pDrv) {
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

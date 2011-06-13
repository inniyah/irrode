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
                  vPos=m_pPlane->getPosition();

  vInv=vInv.invert();

  core::vector3df vUp=vInv.rotationToDirection(m_pAero->getUpward  ()),
                  vSi=vInv.rotationToDirection(m_pAero->getSideward());

  vSi.Y=0.0f;

  vUp.normalize();
  vSi.normalize();

  if (m_pTarget!=NULL) {
    core::vector3df vCheckPos=m_pTarget->getPosition(),
                    vPlaneVel=m_pPlane->getLinearVelocity();

    core::vector2df vPlane=core::vector2df(vPos.X,vPos.Z),
                    vPoint=core::vector2df(vCheckPos.X,vCheckPos.Z),
                    vVeloc=core::vector2df(vPlaneVel.X,vPlaneVel.Z),
                    vCpVel=vPoint-vPlane;

    irr::core::vector2df vxy=vVeloc+vCpVel,vxz=vVeloc-vCpVel;

    vVeloc.normalize();
    vCpVel.normalize();

    f32 fSteer=vVeloc.getAngleWith(vCpVel);

    if (vxy.getLength()-vxz.getLength()<0.0f) fSteer=180-fSteer;
    f32 fOrient=vVeloc.X*vCpVel.Y-vVeloc.Y*vCpVel.X;
    if (fOrient>0.0f) fSteer=-1*fSteer; else fSteer=fSteer;

    if (m_bEnabled) {
      fYaw=0.015f*fSteer;
      if (fYaw> 1.0f) fYaw= 1.0f;
      if (fYaw<-1.0f) fYaw=-1.0f;
    }

    m_fApDist=(vPlane-vPoint).getLength();

    switch (m_iState) {
      case 0:
        if (m_bEnabled) {
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
          irr::core::vector2df vPlaneY=irr::core::vector2df(vPlaneVel.getLength(),vPlaneVel.Y),
                               vTgt=vPlaneY-irr::core::vector2df((vCheckPos-vPos).getLength(),(vCheckPos-vPos).Y),
                               vTargtY=irr::core::vector2df(vTgt.getLength(),vTgt.Y);

          //vPlaneY.normalize();
          //vTargtY.normalize();

          f32 fDir=vPlaneY.X*vTargtY.Y-vPlaneY.Y*vTargtY.X,
              f=fDir<0?vPlaneY.getAngleWith(vTargtY):-vPlaneY.getAngleWith(vTargtY);

          fPitch=f/20.0f;
          if (fPitch> 1.0f) fPitch= 1.0f;
          if (fPitch<-1.0f) fPitch=-1.0f;

          printf("%.2f | %.2f\n",f,fPitch);
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
      fRoll=(fAngle-30.0f*fYaw)*0.01f;
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
                  vDir=vRot.rotationToDirection(m_pAero->getForeward());

  vDir.normalize();
  vDir*=25.0f;
  vDir.Z=-vDir.Z;

  core::vector2di vHead=core::vector2di(vDir.X,vDir.Z);
  pDrv->draw2DLine(core::vector2di(50,100),core::vector2di(50,100)+vHead,video::SColor(0xFF,0xFF,0,0));

  core::vector2di vCenter=core::vector2di(50,200);

  pDrv->draw2DLine(vCenter,vCenter-m_vArtificialHorizon,video::SColor(0xFF,0xFF,0,0));
  pDrv->draw2DLine(vCenter,vCenter+m_vArtificialHorizon,video::SColor(0xFF,0,0xFF,0));
}

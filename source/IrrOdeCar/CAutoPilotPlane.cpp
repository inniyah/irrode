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

  f32 fRollAngle=acosf(vUp.dotProduct(vSi))*180/M_PI;//  Dot(NormalizeEx(v1), NormalizeEx(v2)));
  f32 fARoll=fRollAngle-90.0f;

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
  fYaw=0.015f*fSteer;
  if (fYaw> 1.0f) fYaw= 1.0f;
  if (fYaw<-1.0f) fYaw=-1.0f;

  if (fYaw!=0.0f) fARoll+=5.0f*fYaw;

  m_fApDist=(vPlane-vPoint).getLength();

  f32 vy=m_pPlane->getLinearVelocity().Y;

  f32 fDiffY=(950-vPos.Y)/10.0f;

  if (fDiffY> 10.0f) fDiffY= 10.0f;
  if (fDiffY<-10.0f) fDiffY=-10.0f;

  fPitch=(-vy+fDiffY)*0.05f;
  if (fPitch> 1.0f) fPitch= 1.0f;
  if (fPitch<-1.0f) fPitch=-1.0f;

  fRoll=-fARoll*0.03f;
  if (fRoll> 1.0f) fRoll= 1.0f;
  if (fRoll<-1.0f) fRoll=-1.0f;

  fThrust=vPlaneVel.getLength()<50.0f?1.0f:1.0f-((vPlaneVel.getLength()-50.0f)/25.0f);
  if (fThrust<0.0f) fThrust=0.0f;
}


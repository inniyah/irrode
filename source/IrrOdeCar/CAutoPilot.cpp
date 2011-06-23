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

  setState(eApPlaneCruise);

  m_vArtificialHorizon=irr::core::vector2di(25.0f,0.0f);

  m_bEnabled=false;
  m_bLinkYawRoll=false;
  m_pRay=NULL;
}

CAutoPilot::CAutoPilot(irr::ode::CIrrOdeBody         *pPlane,
                       irr::ode::CIrrOdeAeroDrag     *pAero,
                       irr::ode::CIrrOdeTorqueMotor  *pTorque,
                       irr::ode::CIrrOdeImpulseMotor *pMotor,
                       irr::ode::CIrrOdeGeomRay      *pRay) {
  m_pTarget=NULL;
  m_pTargetBody=NULL;

  m_pPlane=pPlane;
  m_pAero=pAero;
  m_pTorque=pTorque;
  m_pMotor=pMotor;

  setState(eApHeliCruise);

  m_vArtificialHorizon=irr::core::vector2di(25.0f,0.0f);

  m_bEnabled=false;
  m_bLinkYawRoll=false;
  m_pRay=pRay;
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

void CAutoPilot::updateApDist() {
  core::vector2df vPlane=core::vector2df(m_vPosition.X,m_vPosition.Z),
                  vPoint=core::vector2df(m_vCheckPos.X,m_vCheckPos.Z);

  m_fApDist=(vPlane-vPoint).getLength();
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

irr::f32 CAutoPilot::getHeliPitch() {
  f32 f=asin(m_vForeward.Y)*180.0f/M_PI,fPitch=0.0f;

  if (m_iState==eApHeliLowAlt) {
    fPitch=-f/10.0f;
  }
  else {
    if (f<-15.0f)
      fPitch=0.25f;
    else
      if (f>0.0f)
        fPitch=-0.75f;
      else
        fPitch=(-15.0f-f)/20.0f;

  }
  if (fPitch> 1.0f) fPitch= 1.0f;
  if (fPitch<-1.0f) fPitch=-1.0f;

  return fPitch;
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

  updateApDist();

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

          if (m_vPosition.Y<800.0f) setState(eApPlaneLowAlt);
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

          if (m_vPosition.Y>800.0f) setState(eApPlaneCruise);
        }
        break;

      case eApHeliLowAlt:
          if (m_vPosition.Y<450.0f)
            fThrust=1.0f;
          else
            setState(eApHeliCruise);

          fRoll=getRollControl(fYaw);
          fPitch=getHeliPitch();
        break;

      case eApHeliCruise:
        if (m_bEnabled) {
          core::vector3df vRot=m_pPlane->getRotation(),
                          vDir=vRot.rotationToDirection(m_pAero->getForeward()),
                          vPos=m_pPlane->getPosition();

          if (m_vPosition.Y<450.0f) {
            setState(eApHeliLowAlt);
          }
          else
            if (m_vPosition.Y<550.0f)
              fThrust=1.0f;
            else
              if (m_vPosition.Y>600.0f) {
                fThrust=0.0f;
              }
              else {
                fThrust=1.0f-((m_vPosition.Y-550.0f)/50.0f)-m_vVelocityLin.Y/15.0f;
                if (fThrust>1.0f) fThrust=1.0f;
                if (fThrust<0.0f) fThrust=0.0f;
              }

          if (m_pRay!=NULL) {
            f32 fAngle[]={ -30.0f, -10.0f, 0.0f, 10.0f, 30.0f, 99.0f },
                fDist []={   1000,   1000,  1000, 1000, 1000 },
                fFact=3.0f,
                fLeft=0.0f,fRight=0.0f;

            vDir.normalize();

            core::vector2df vStart =core::vector2df( 10.0f*vDir.X, 10.0f*vDir.Z),
                            vCenter=core::vector2df(250.0f*vDir.X,250.0f*vDir.Z);

            for (u32 i=0; fAngle[i]!=99.0f; i++) {
              core::array<core::vector3df> aHits;
              core::vector2df v=vCenter,vEnd=v.rotateBy(fAngle[i]);
              core::vector3df v1=vPos+core::vector3df(vStart.X,0.0f,vStart.Y),
                              v2=     core::vector3df(  vEnd.X,0.0f,  vEnd.Y);

              m_pRay->set(v1,v2,1000.0f);
              m_pRay->checkWithWorld(aHits);

              for (u32 j=0; j<aHits.size(); j++) {
                core::vector3df vDist=m_vPosition-aHits[j];
                f32 f=vDist.getLength();
                if (f<fDist[i]) fDist[i]=f;
              }

              if (i<=2) fRight+=fFact*fDist[i];
              if (i>=2) fLeft +=fFact*fDist[i];

              if (i< 2) fFact-=1.0f;
              if (i>=2) fFact+=1.0f;
            }

            fYaw=getYawControl(false);
            if (fLeft<6000.0f || fRight<6000.0f) {
              if (fLeft>fRight)
                fYaw=-(6000.0f-fLeft )/6000.0f;
              else
                fYaw= (6000.0f-fRight)/6000.0f;
            }
            //printf("%.2f -- %.2f ==> %.2f\n",fLeft,fRight,fYaw);
          }

          fRoll=getRollControl(fYaw);
          fPitch=getHeliPitch();
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

  if (m_pTarget!=NULL && m_bEnabled) {
    irr::core::CMatrix4<f32> mat=irr::core::CMatrix4<f32>();
    pDrv->setTransform(irr::video::ETS_WORLD,mat);
    irr::video::SMaterial cMat;
    cMat.Lighting=false;
    pDrv->setMaterial(cMat);

    switch (m_iState) {
      case eApHeliLowAlt:
      case eApPlaneLowAlt:
        break;

      case eApHeliCruise:
        break;

      default: {
        vDir.normalize();
        vDir*=25.0f;
        vDir.Z=-vDir.Z;
        vVel.normalize();

        core::vector2di vCenter=core::vector2di(50,150);
        core::vector2df vTarget=m_vCpVel;
        vTarget.normalize();
        vTarget*=25.0f;
        vTarget.Y=-vTarget.Y;
        pDrv->draw2DLine(vCenter,vCenter+core::vector2di(vTarget.X,vTarget.Y),video::SColor(0xFF,192,255,192));

        irr::core::line3df cLine=irr::core::line3df(vPos,vPos+3000.0f*vVel);//vFw);
        pDrv->draw3DLine(cLine.start,cLine.end,video::SColor(0xFF,0xFF,0xFF,0xFF));
        irr::core::vector3df vNear=cLine.getClosestPoint(m_pTarget->getPosition());
        pDrv->draw3DLine(vNear,m_pTarget->getPosition(),video::SColor(0xFF,0xFF,0,0));

        core::vector2di vHead=core::vector2di(vDir.X,vDir.Z);
        pDrv->draw2DLine(vCenter,vCenter+vHead,video::SColor(0xFF,0xFF,0,0));

        vCenter=core::vector2di(50,250);

        pDrv->draw2DLine(vCenter,vCenter-m_vArtificialHorizon,video::SColor(0xFF,0xFF,0,0));
        pDrv->draw2DLine(vCenter,vCenter+m_vArtificialHorizon,video::SColor(0xFF,0,0xFF,0));
      }
      break;
    }
  }
}

void CAutoPilot::setState(eAutoPilotState iState) {
  printf("**** autopilot of \"%s\": new state ",m_pPlane->getName());
  switch (iState) {
    case eApPlaneLowAlt: printf("\"eApPlaneLowAlt\"\n"); break;
    case eApPlaneCruise: printf("\"eApPlaneCruise\"\n"); break;
    case eApHeliLowAlt : printf("\"eApHeliLowAlt\"\n" ); break;
    case eApHeliCruise : printf("\"eApHeliCruise\"\n" ); break;
    case eApMissile    : printf("\"eApMissile\"\n"    ); break;
  }
  m_iState=iState;
}

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

  m_bEnabled=false;
  m_bLinkYawRoll=false;
  m_pRay=NULL;

  m_fHeliDistLeft =NULL;
  m_fHeliDistRight=NULL;
  m_iHeliCheckCount=0;
  m_fHeliCheckMax=0.0f;

  m_pAutoPilotInfo=NULL;
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

  m_bEnabled=false;
  m_bLinkYawRoll=false;
  m_pRay=pRay;

  m_iHeliCheckCount=2;

  m_fHeliDistLeft =(f32 *)malloc(5*sizeof(f32));
  m_fHeliDistRight=(f32 *)malloc(5*sizeof(f32));

  m_fHeliDistLeft[0]=45.0f; m_fHeliDistRight[0]=-45.0f;
  m_fHeliDistLeft[1]=10.0f; m_fHeliDistRight[1]=-10.0f;

  m_fHeliCheckLength=500.0f;

  m_fHeliCheckMax=0.0f;
  for (u32 i=0; i<m_iHeliCheckCount; i++) m_fHeliCheckMax+=(i+1)*m_fHeliCheckLength;
  m_pAutoPilotInfo=NULL;
}

CAutoPilot::~CAutoPilot() {
  if (m_fHeliDistLeft !=NULL) free(m_fHeliDistLeft );
  if (m_fHeliDistRight!=NULL) free(m_fHeliDistRight);
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

irr::f32 CAutoPilot::getRollControl(f32 fYaw, wchar_t *sInfo) {
  f32 fAngle=asin(m_vSideward2.Y)*180.0f/PI,fRoll=0.0f;
  if (m_vUpward.Y<0) fAngle=-180.0f-fAngle;

  while (fAngle<-180.0f) fAngle+=360.0f;
  while (fAngle> 180.0f) fAngle-=360.0f;

  if (m_bLinkYawRoll) fRoll=(fAngle-20.0f*fYaw)*0.01f; else fRoll=fAngle*0.01f;

  if (fRoll> 1.0f) fRoll= 1.0f;
  if (fRoll<-1.0f) fRoll=-1.0f;

  if (sInfo!=NULL) swprintf(sInfo,4096,L"%s\nRoll angle: %.2f\napplied control: %.2f\n",sInfo,fAngle,fRoll);

  core::vector2df vAhf=25.0f*core::vector2df(1.0f,0.0f).rotateBy(fAngle);

  return fRoll;
}

void CAutoPilot::updateApDist() {
  core::vector2df vPlane=core::vector2df(m_vPosition.X,m_vPosition.Z),
                  vPoint=core::vector2df(m_vCheckPos.X,m_vCheckPos.Z);

  m_fApDist=(vPlane-vPoint).getLength();
}

irr::f32 CAutoPilot::getYawControl(bool b, wchar_t *sInfo) {
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

  if (sInfo!=NULL) swprintf(sInfo,4096,L"%s\nSteer to target: %.2f\n",sInfo,fSteer);

  m_vVATransformed=m_vInvRot.rotationToDirection(m_vVelocityAng);

  if (b && (m_vVATransformed.Y>0.1f || m_vVATransformed.Y<-0.1f)) {
    f32 fSign=m_vVATransformed.Y>0.0f?1.0f:0.0f;
    f32 fMinus=50.0f*m_vVATransformed.Y;
    fMinus*=fSign*fMinus;
    if (fSteer>0 && fMinus>fSteer) fMinus=fSteer;
    if (fSteer<0 && fMinus<fSteer) fMinus=fSteer;
    fSteer-=fMinus;
    if (sInfo!=NULL) swprintf(sInfo,4096,L"%s\nHigh yaw detected\ncompenation: %.2f\n",sInfo,fMinus);
  }

  fYaw=(b?0.1f:0.05f)*fSteer;
  if (fYaw> 1.0f) fYaw= 1.0f;
  if (fYaw<-1.0f) fYaw=-1.0f;

  return fYaw;
}

irr::f32 CAutoPilot::getHeliPitch(wchar_t *sInfo, f32 fPitchTarget) {
  f32 f=asin(m_vForeward.Y)*180.0f/M_PI,fPitch=0.0f;

  if (m_iState==eApHeliLowAlt) {
    fPitch=-f/10.0f;
  }
  else {
    if (f<fPitchTarget)
      fPitch=-(f-fPitchTarget)/-fPitchTarget;
    else
      if (f>0.0f)
        fPitch=-0.75f;
      else
        fPitch=(fPitchTarget-f)/20.0f;

  }
  if (fPitch> 1.0f) fPitch= 1.0f;
  if (fPitch<-1.0f) fPitch=-1.0f;

  if (sInfo!=NULL) swprintf(sInfo,4096,L"%s\nPitch target: %.2f\nCurrent Pitch: %.2f\nApplied pitch: %.2f\n",sInfo,fPitchTarget,f,fPitch);

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

  wchar_t sInfo[4096]=L"";

  if (m_pTarget!=NULL) {
    switch (m_iState) {
      case eApPlaneCruise:
        if (m_bEnabled) {
          swprintf(sInfo,4096,L"AutoPilot active\nState: \"PlaneCruise\"\n");

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

          swprintf(sInfo,4096,L"%s\nDiff for pitch: %.2f\n",sInfo,fDiffY);
          fPitch=(-m_vVelocityLin.Y+fDiffY)*0.05f;
          if (fPitch> 1.0f) fPitch= 1.0f;
          if (fPitch<-1.0f) fPitch=-1.0f;

          fThrust=m_vVelocityLin.getLength()<50.0f?1.0f:1.0f-((m_vVelocityLin.getLength()-50.0f)/25.0f);
          if (fThrust<0.0f) fThrust=0.0f;

          fYaw=getYawControl(false,sInfo);
          fRoll=getRollControl(fYaw,sInfo);

          if (m_vPosition.Y<800.0f) setState(eApPlaneLowAlt);
        }
        break;

      case eApPlaneLowAlt:
        if (m_bEnabled) {
          swprintf(sInfo,4096,L"AutoPilot active\nState:\"PlaneLowAlt\"\n");
          swprintf(sInfo,4096,L"%s\naltitude: %.2f\n",sInfo,m_vPosition.Y);

          fThrust=1.0f;

          fRoll=getRollControl(fYaw,sInfo);

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

          fRoll=getRollControl(fYaw,sInfo);
          fPitch=getHeliPitch(sInfo,0.0f);

          swprintf(sInfo,4096,L"AutoPilot active\nState:\"HeliLowAlt\"\n");
          swprintf(sInfo,4096,L"%s\naltitude: %.2f\n",sInfo,m_vPosition.Y);
        break;

      case eApHeliCruise:
        if (m_bEnabled) {
          f32 fPitchTarget=-15.0f;
          swprintf(sInfo,4096,L"AutoPilot active\nState:\"HeliCruise\"\n");
          swprintf(sInfo,4096,L"%s\nAngular Velocity: %.2f, %.2f, %.2f\n",sInfo,m_vVATransformed.X,m_vVATransformed.Y,m_vVATransformed.Z);

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
              else
                fThrust=1.0f-((m_vPosition.Y-550.0f)/50.0f)-m_vVelocityLin.Y/15.0f;

          if (m_pRay!=NULL) {
            f32 fLeft=0.0f,fRight=0.0f;

            vDir.normalize();

            core::vector2df vStart =core::vector2df( 10.0f*vDir.X, 10.0f*vDir.Z),
                            vCenter=core::vector2df(250.0f*vDir.X,250.0f*vDir.Z);

            for (u32 j=0; j<2; j++) {
              f32 fDist[]={ 500.0f, 500.0f, 500.0f, 500.0f, 500.0f },
                  fFact=1.0f;

              for (u32 i=0; i<m_iHeliCheckCount; i++) {
                core::array<core::vector3df> aHits;
                core::vector2df v=vCenter,vEnd=v.rotateBy(j==0?m_fHeliDistLeft[i]:m_fHeliDistRight[i]);
                core::vector3df v1=vPos+core::vector3df(vStart.X,0.0f,vStart.Y),
                                v2=     core::vector3df(  vEnd.X,0.0f,  vEnd.Y);

                m_pRay->set(v1,v2,m_fHeliCheckLength);
                m_pRay->checkWithWorld(aHits);

                for (u32 k=0; k<aHits.size(); k++) {
                  core::vector3df vDist=m_vPosition-aHits[k];
                  f32 f=vDist.getLength();
                  if (f<fDist[i]) fDist[i]=f;
                }

                if (j==0) fLeft+=fFact*fDist[i]; else fRight+=fFact*fDist[i];

                fFact+=1.0f;
              }
            }

            swprintf(sInfo,4096,L"%s\nleft: %.2f\nright: %.2f\n",sInfo,fLeft,fRight);
            fYaw=0.5f*getYawControl(false,sInfo);
            if (fLeft<m_fHeliCheckMax || fRight<m_fHeliCheckMax) {
              swprintf(sInfo,4096,L"%s\ncollision warning - ",sInfo);
              if (fLeft>fRight) {
                fYaw=-2.5f*(m_fHeliCheckMax-fRight)/m_fHeliCheckMax;

                if (fYaw> 1.0f) fYaw= 1.0f;
                if (fYaw<-1.0f) fYaw=-1.0f;

                fPitchTarget=-15.0f*(fRight/m_fHeliCheckMax);
                swprintf(sInfo,4096,L"%sleft turn (%.2f%%)!\n",sInfo,100.0f*fYaw);
              }
              else {
                fYaw=2.5f*(m_fHeliCheckMax-fLeft)/m_fHeliCheckMax;

                if (fYaw> 1.0f) fYaw= 1.0f;
                if (fYaw<-1.0f) fYaw=-1.0f;

                fPitchTarget=-15.0f*(fLeft/m_fHeliCheckMax);
                swprintf(sInfo,4096,L"%sright turn (%.2f%%)!\n",sInfo,100.0f*fYaw);
              }
            }

            {
              core::array<core::vector3df> aHits;
              m_pRay->set(m_vPosition-core::vector3df(0.0f,5.0f,0.0f),core::vector3df(0.0f,-1.0f,0.0f),250.0f);
              m_pRay->checkWithWorld(aHits);
              if (aHits.size()>0) {
                f32 fHeight=250.0f;
                for (u32 i=0; i<aHits.size(); i++) {
                  core::vector3df vDist=m_vPosition-aHits[i];
                  f32 f=vDist.getLength();
                  if (f<fHeight) fHeight=f;
                }

                if (fHeight<250.0f) {
                  f32 fMaxAddPower=1.0f-fThrust,fAdd=-(fHeight-250.0f)/50.0f*fMaxAddPower;
                  if (fThrust+fAdd>1.0f) fAdd=1.0f-fThrust;
                  fThrust+=fAdd;
                  swprintf(sInfo,4096,L"%s\nLow alt detected: %.2f\nPower Plus %.2f%%\n",sInfo,fHeight,100.0f*fAdd);
                }
              }
            }

            if (m_vVelocityLin.Y<-10.0f) {
              f32 fMaxAddPower=1.0f-fThrust,fAdd=-(m_vVelocityLin.Y+10.0f)/5.0f*fMaxAddPower;
              if (fThrust+fAdd>1.0f) fAdd=1.0f-fThrust;
              fThrust+=fAdd;
              swprintf(sInfo,4096,L"%s\nHigh sink rate detected\nPower Plus: %.2f%%\n",sInfo,100.0f*fAdd);
            }
          }

          if (fThrust>1.0f) fThrust=1.0f;
          if (fThrust<0.0f) fThrust=0.0f;

          fRoll=getRollControl(fYaw,sInfo);
          fPitch=getHeliPitch(sInfo,fPitchTarget);
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
          fRoll=getRollControl(fYaw,NULL);
        }
        break;
    }
  }

  if (m_pAutoPilotInfo!=NULL) m_pAutoPilotInfo->setText(sInfo);
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

void CAutoPilot::setAutoPilotInfo(gui::IGUIStaticText *pInfo) {
  m_pAutoPilotInfo=pInfo;
}

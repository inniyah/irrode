  #include <irrlicht.h>
  #include "CCar.h"
  #include <CCustomEventReceiver.h>
  #include <CCockpitCar.h>
  #include <math.h>
  #include <irrklang.h>
  #include <CRearView.h>
  #include <CEventVehicleState.h>
  #include <CIrrOdeCarTrack.h>

  #include <irrCC.h>

using namespace irr;

void findNodesOfType(ISceneNode *pParent, irr::scene::ESCENE_NODE_TYPE iType, array<ISceneNode *> &aNodes) {
  list<irr::scene::ISceneNode *> children=pParent->getChildren();
  list<irr::scene::ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) {
    if ((*it)->getType()==iType) {
      aNodes.push_back(*it);
    }
    findNodesOfType(*it,iType,aNodes);
  }
}

CCar::CCar(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitCar *pCockpit, CRearView *pRView) : CIrrOdeCarState(pDevice,L"Car","../../data/irrOdeCarHelp.txt", pCtrl) {
  //get the car body
  m_pCarBody=reinterpret_cast<ode::CIrrOdeBody *>(pNode);
  m_fSound=0.75f;
  m_fOldSlider=0.0f;
  m_bGasStation=false;
  m_bGasLastStep=false;
  m_iNextCp=-1;
  m_iCurStep=0;
  m_iLastLapStep=0;

  if (m_pCarBody) {
    m_iBodyId=m_pCarBody->getID();
    m_pCarBody->setUserData(this);
    CCustomEventReceiver::getSharedInstance()->addCar(m_pCarBody);
    array<ISceneNode *> aNodes;

    m_pAxesFront[0]=reinterpret_cast<ode::CIrrOdeJointHinge2 *>(m_pCarBody->getChildByName("axis_fl",m_pCarBody));
    m_pAxesFront[1]=reinterpret_cast<ode::CIrrOdeJointHinge2 *>(m_pCarBody->getChildByName("axis_fr",m_pCarBody));

    printf("axis_FL=%i\n",(int)m_pAxesFront[0]);
    printf("axis_FR=%i\n",(int)m_pAxesFront[1]);

    m_pAxesRear[0]=reinterpret_cast<ode::CIrrOdeJointHinge *>(m_pCarBody->getChildByName("axis_rl",m_pCarBody));
    m_pAxesRear[1]=reinterpret_cast<ode::CIrrOdeJointHinge *>(m_pCarBody->getChildByName("axis_rr",m_pCarBody));

    printf("axis_RL=%i\n",(int)m_pAxesRear[0]);
    printf("axis_RR=%i\n",(int)m_pAxesRear[1]);

    for (u32 i=0; i<2; i++) {
      c8 s[0xFF];
      sprintf(s,"sc_motor%i",i+1);
      m_pMotor[i]=reinterpret_cast<ode::CIrrOdeMotor *>(m_pCarBody->getChildByName(s,m_pCarBody));
      sprintf(s,"brk_fr_%i",i+1);
      m_pBrkFr[i]=reinterpret_cast<ode::CIrrOdeMotor *>(m_pCarBody->getChildByName(s,m_pCarBody));
      sprintf(s,"brk_re_%i",i+1);
      m_pBrkRe[i]=reinterpret_cast<ode::CIrrOdeMotor *>(m_pCarBody->getChildByName(s,m_pCarBody));
    }

    c8 sWheelNames[][20]={ "wheel_fl", "wheel_fr", "wheel_rl", "wheel_rr" };
    for (u32 i=0; i<4; i++)
      m_pWheels[i]=reinterpret_cast<ode::CIrrOdeGeomSphere *>(m_pCarBody->getChildByName(sWheelNames[i],m_pCarBody));

    core::stringw sParamNames[]={ L"surfaceTireFront", L"surfaceTireStop", L"surfaceTireBack", L"surfaceTireStop" };
    for (u32 i=0; i<4; i++) {
      m_pParams[i]=new ode::CIrrOdeSurfaceParameters();
      ode::CIrrOdeManager::getSharedInstance()->getSurfaceParameter(sParamNames[i])->copy(m_pParams[i]);
    }

    m_pSuspension=reinterpret_cast<ode::CIrrOdeBody *>(m_pCarBody->getChildByName("sc_suspension_rear",m_pCarBody));
    if (m_pSuspension!=NULL)
      m_vSuspNeutral=m_pSuspension->getPosition();
    else
      m_vSuspNeutral=core::vector3df(0.0f,0.0f,0.0f);

    m_pJointSus=reinterpret_cast<ode::CIrrOdeJointSlider *>(m_pCarBody->getChildByName("suspension_joint",m_pCarBody));

    c8 sWheelBodies[][20]={ "sc_wheel_rl", "sc_wheel_rr" };
    for (u32 i=0; i<2; i++)
      m_pRearWheels[i]=reinterpret_cast<ode::CIrrOdeBody *>(m_pCarBody->getChildByName(sWheelBodies[i],m_pCarBody));

    printf("**** motors: %i, %i\n",(int)m_pMotor[0],(int)m_pMotor[1]);
    printf("**** front brakes: %i, %i\n",(int)m_pBrkFr[0],(int)m_pBrkFr[1]);
    printf("**** rear brakes: %i, %i\n",(int)m_pBrkRe[0],(int)m_pBrkRe[1]);
    printf("**** wheels: ");
    for (u32 i=0; i<4; i++) printf("%i%s",(int)m_pWheels[i],i<3?", ":"");
    printf("\n");
    printf("**** wheel bodies: %i, %i\n",(int)m_pRearWheels[0],(int)m_pRearWheels[1]);
    printf("**** params: ");
    for (u32 i=0; i<4; i++) printf("%i%s",(int)m_pParams[i],i<3?", ":"");
    printf("\n");
    printf("**** suspension: %i, %i\n",(int)m_pSuspension,(int)m_pJointSus);
    printf("**** suspension neutral: (%.2f, %.2f, %.2f)\n",m_vSuspNeutral.X,m_vSuspNeutral.Y,m_vSuspNeutral.Z);

    aNodes.clear();
    //get the two servos that are attached to the front wheels
    findNodesOfType(m_pCarBody,(ESCENE_NODE_TYPE)irr::ode::IRR_ODE_SERVO_ID,aNodes);

    if (aNodes.size()>=2) {
      m_pServo[0]=dynamic_cast<ode::CIrrOdeServo *>(aNodes[0]);
      m_pServo[1]=dynamic_cast<ode::CIrrOdeServo *>(aNodes[1]);
    }

    //initialize the members
    m_bHelp=false;
    m_bBrake=false;
    m_bBoost=false;
    m_bAdaptSteer=false;
    m_iThrottle=0;

    m_fCamAngleH=0.0f;
    m_fCamAngleV=0.0f;
    m_fOldVel=0.0f;

    //add a camera
    m_pCam=m_pSmgr->addCameraSceneNode();
    m_pCam->setNearValue(0.1f);

    m_pTab=m_pGuiEnv->addTab(core::rect<s32>(0,0,300,300));

    m_pTab->setVisible(false);
    m_pCtrls=NULL;

    //we are an IrrOde event listener
    ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
    m_pCockpit=pCockpit;
    m_pRView=pRView;

    m_pLap = new CIrrOdeCarTrack(m_pCarBody);

    m_bInternal=false;
    m_bDifferential=true;

    m_bInitialized=true;
  }

  m_vOldSpeed=core::vector3df(0.0f,0.0f,0.0f);
  m_fDiff=0.0f;
  m_fRpm=0.0f;
}

CCar::~CCar() {
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
  for (u32 i=0; i<4; i++) delete m_pParams[i];
}

//This method is called when the state is activated.
void CCar::activate() {
  m_pSmgr->setActiveCamera(m_pCam);
  m_pTab->setVisible(true);
  m_pDevice->setEventReceiver(this);
  m_pDevice->getCursorControl()->setVisible(false);
  m_bSwitchToMenu=false;
  m_bActive=true;

  //get the parameters for the camera ...
  vector3df pos=m_pCarBody->getRotation().rotationToDirection(vector3df(8,4  ,0)),
            up =m_pCarBody->getRotation().rotationToDirection(vector3df(0,0.2,0)),
            tgt=m_pCarBody->getRotation().rotationToDirection(vector3df(0,2  ,0));

  //... and apply them to the active camera
  m_pCam->setPosition(m_pCarBody->getPosition()+pos);
  m_pCam->setUpVector(up);
  m_pCam->setTarget(m_pCarBody->getPosition()+tgt);

  loadHelpFile();
  wchar_t s[0xFFFF];
  swprintf(s,0xFFFE,m_pHelp->getText(),m_pController->getSettingsText(0));
  m_pHelp->setText(s);

  if (m_pCockpit) m_pCockpit->activate(m_pCarBody);
}

void CCar::deactivate() {
  m_pTab->setVisible(false);
  m_bActive=false;
  m_pController->reset();

  for (u32 i=0; i<2; i++) {
    m_pMotor[i]->setVelocity(0.0f);
    m_pMotor[i]->setForce(5.0f);
  }

  for (u32 i=0; i<2; i++) m_pServo[i]->setServoPos(0.0f);

  if (m_pCockpit) m_pCockpit->activate(NULL);
}

//This method is called once for each rendered frame.
u32 CCar::update() {
  //call the superclasse's update method
  u32 iRet=CIrrOdeCarState::update();

  #define _MAX_STEER 25.0f
  #define _MIN_STEER 10.0f

  //the car's velocity
  f32 v=m_pCarBody->getLinearVelocity().getLength();
  //is the adaptive steer option is not active ...
  if (!m_bAdaptSteer)
    m_fActSteer=_MAX_STEER;   //... just use the default steering angle of 45 degrees, otherwise ...
  else
    if (v<10.0f)
      m_fActSteer=_MAX_STEER;    //... just use this value if the velocity is low. If the velocity is higher, even ...
    else
      if (v>45.0f)
        m_fActSteer=_MIN_STEER;  //... higher than 45 we use the minimum value of 10 degrees. If the speed is between 10 ...
      else
        //and 45 we calculate the actual steering angle
        m_fActSteer=_MAX_STEER-(_MAX_STEER-_MIN_STEER)*(v-10.0f)/(_MAX_STEER-_MIN_STEER);

  //get the parameters for the camera ...
  vector3df pos=m_pCarBody->getRotation().rotationToDirection(m_bInternal?vector3df(0,1.35,0):vector3df(8,4,0)),
            up =m_pCarBody->getRotation().rotationToDirection(vector3df(0,0.2,0)),
            tgt=m_pCarBody->getRotation().rotationToDirection(m_bInternal?vector3df(-5,1.35,0):vector3df(0,2,0)),
            rot=m_pCarBody->getRotation();

  if (m_bInternal) {
    core::vector2df lookAt=core::vector2df(0.0f,-5.0f).rotateBy(m_fCamAngleH),
                    lookUp=core::vector2df(5.0f, 0.0f).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(vector3df(0.0f,1.35f,0.0f)),
    up =rot.rotationToDirection(vector3df(0.0f, 1.0f,0.0f));
    tgt=rot.rotationToDirection(vector3df(lookAt.Y,1.1+lookUp.Y,lookAt.X));
  }
  else {
    core::vector2df lookAt=core::vector2df(  0.0f,15.0f).rotateBy(m_fCamAngleH),
                    lookUp=core::vector2df(-15.0f, 0.0f).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(vector3df(lookAt.Y,5.0f+lookUp.Y,lookAt.X)),
    up =rot.rotationToDirection(vector3df(0,1,0));
    tgt=rot.rotationToDirection(vector3df(0,4,0));
  }

  //... and apply them to the active camera
  m_pCam->setPosition(m_pCarBody->getPosition()+pos);
  m_pCam->setUpVector(up);
  m_pCam->setTarget(m_pCarBody->getPosition()+tgt);

  //now we fill the info text with useful information
  wchar_t dummy[0xFF];
  pos=m_pCarBody->getAbsolutePosition();
  swprintf(dummy,0xFE,L"vel: %.2f\npos: (%.0f, %.0f, %.0f)\nsteer: %.2f %s",v,pos.X,pos.Y,pos.Z,m_fActSteer,m_bAdaptSteer?L"(adaptive)":L"");
  if (m_pController->get(m_pCtrls[eCarBoost])!=0.0f) swprintf(dummy,0xFF,L"%s\nBOOST!",dummy);

  //if the iRet value we got from CVehicle::update is more than 0 the state will be deactivated and
  //one of the other states will get active.
  return iRet;
}

//here we have the Irrlicht event receiver
bool CCar::OnEvent(const SEvent &event) {
  bool bRet=m_pController->OnEvent(event);
  bRet|=CIrrOdeCarState::OnEvent(event);
  return bRet;
}

bool CCar::onEvent(ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    m_iCurStep++;
    if (m_iNextCp!=-1 && m_iLastLapStep!=0) m_pCockpit->setCurrentLapTime((((float)m_iCurStep)-(float)m_iLastLapStep)*0.016f);
    if (m_bGasLastStep && !m_bGasStation) {
      const core::vector3df v=m_pCarBody->getPosition();
      CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndBell,2.0f,v);
      ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
    }

    m_bGasLastStep = m_bGasStation;
    m_bGasStation=false;

    bool bDataChanged=false,bBrake=false;

    core::vector3df vForeward=m_pCarBody->getRotation().rotationToDirection(core::vector3df(-1.0f,0.0f,0.0f)),
                    vNormVel=m_pCarBody->getLinearVelocity();

    vNormVel.normalize();

    f32 fVelocity=vForeward.dotProduct(m_pCarBody->getLinearVelocity());

    if (m_bActive) {
      bool bBoost=m_pController->get(m_pCtrls[eCarBoost])!=0.0f;

      if (bBoost!=m_bBoost) {
        m_pCockpit->setBoost(bBoost);
        m_bBoost=bBoost;
        bDataChanged=true;
      }

      m_pCockpit->setRpm(-m_fRpm);

      f32 fForeward=m_pController->get(m_pCtrls[eCarForeward]),
          fSpeed=-0.8f*(m_pAxesFront[0]->getHingeAngle2Rate()+m_pAxesFront[1]->getHingeAngle2Rate())/2;

      if ((fForeward<0.0f && fVelocity>2.0f) || (fForeward>0.0f && fVelocity<-2.0f)) bBrake=true;
      //calculate the differential gear
      f32 f1=m_pAxesRear[0]->getHingeAngleRate(),
          f2=m_pAxesRear[1]->getHingeAngleRate(),
          fDiff=f1-f2,fFact[2]={ 1.0f, 1.0f };

      if (m_bDifferential) {
        if (fDiff>2.5f || fDiff<-2.5f) {
          if (fDiff> 150.0f) fDiff= 150.0f;
          if (fDiff<-150.0f) fDiff=-150.0f;
          fFact[0]=1.0f-fDiff/150.0f;
          fFact[1]=1.0f+fDiff/150.0f;
        }

        if (m_fDiff>fDiff) {
          m_fDiff-=3.5f;
          if (m_fDiff<fDiff) m_fDiff=fDiff;
        }

        if (m_fDiff<fDiff) {
          m_fDiff+=3.5f;
          if (m_fDiff>fDiff) m_fDiff=fDiff;
        }
      }
      else {
        fDiff=0.0f;
        m_fDiff=0.0f;
        fFact[0]=1.0f;
        fFact[1]=1.0f;
      }

      m_pCockpit->setDiff(-m_fDiff);

      if (fForeward!=0.0f) {
        f32 fForce=fForeward<0.0f?-fForeward:fForeward;

        for (u32 i=0; i<2; i++) {
          m_pMotor[i]->setVelocity(-250.0*fForeward);
          m_pMotor[i]->setForce(bBoost?fFact[i]*60*fForce:fFact[i]*40*fForce);
          m_iThrottle=-1;
        }
      }
      else
        for (u32 i=0; i<2; i++) {
          m_pMotor[i]->setVelocity(0.0f);
          m_pMotor[i]->setForce(5.0f);
        }

      m_fSteer=m_pController->get(m_pCtrls[eCarLeft]);

      if (m_fSteer!=0.0f)
        for (u32 i=0; i<2; i++) m_pServo[i]->setServoPos(m_fActSteer*m_fSteer);
      else
        for (u32 i=0; i<2; i++) m_pServo[i]->setServoPos(0.0f);

      if (bBrake || m_pController->get(eCarBrake)!=0.0f) {
        f32 fFact=(m_pController->get(eCarBrake)!=0.0f)?m_pController->get(eCarBrake):fForeward;
        if (fFact<0.0f) fFact=-fFact;

        for (u32 i=0; i<2; i++) {
          m_pBrkFr[i]->setVelocity(0.0f); m_pBrkFr[i]->setForce(fFact*350.0f);
          m_pBrkRe[i]->setVelocity(0.0f); m_pBrkRe[i]->setForce(fFact*150.0f);
        }
        m_bBrake=true;
      }
      else m_bBrake=false;

      if (m_pController->get(m_pCtrls[eCarToggleAdaptiveSteer])!=0.0f) {
        m_bAdaptSteer=!m_bAdaptSteer;
        m_pController->set(m_pCtrls[eCarToggleAdaptiveSteer],0.0f);
      }

      //if the flip car key was pressed we add a torque to the car in order to turn it back on it's wheels
      if (m_pController->get(m_pCtrls[eCarFlip])!=0.0f) {
        vector3df v=m_pCarBody->getAbsoluteTransformation().getRotationDegrees().rotationToDirection(vector3df(0,0.3f,0));
        m_pCarBody->addForceAtPosition(m_pCarBody->getPosition()+v,vector3df(0,120,0));
      }

      if (m_pController->get(m_pCtrls[eCarInternal])!=0.0f) {
        m_bInternal=!m_bInternal;
        m_pController->set(m_pCtrls[eCarInternal],0.0f);
      }

      m_pCockpit->setSpeed(fSpeed);
      m_pTab->setVisible(false);
      m_pCockpit->update(false);
      m_pTab->setVisible(true);

      core::vector3df cRot=m_pCarBody->getAbsoluteTransformation().getRotationDegrees(),
                      cPos=m_pCarBody->getAbsolutePosition()+cRot.rotationToDirection(core::vector3df(1.0f,1.75f,0.0f)),
                      cTgt=cPos+cRot.rotationToDirection(core::vector3df(1.0f,0.0f,0.0f)),
                      cUp=cRot.rotationToDirection(core::vector3df(0.0f,1.0f,0.0f));

      if (m_pRView!=NULL) {
        m_pRView->setCameraParameters(cPos,cTgt,cUp);
        m_pRView->update(true);
      }

      if (m_pController->get(m_pCtrls[eCarCamRight])!=0.0f) {
        m_fCamAngleH+=m_pController->get(m_pCtrls[eCarCamRight]);

        if (m_fCamAngleH> 190.0f) m_fCamAngleH= 190.0f;
        if (m_fCamAngleH<-190.0f) m_fCamAngleH=-190.0f;
      }

      if (m_pController->get(m_pCtrls[eCarCamUp])!=0.0f) {
        m_fCamAngleV+=m_pController->get(m_pCtrls[eCarCamUp]);

        if (m_fCamAngleV> 60.0f) m_fCamAngleV= 60.0f;
        if (m_fCamAngleV<-60.0f) m_fCamAngleV=-60.0f;
      }

      if (m_pController->get(m_pCtrls[eCarCamCenter])) {
        if (m_fCamAngleH!=0.0f) {
          if (m_fCamAngleH>0.0f) {
            m_fCamAngleH-=5.0f;
            if (m_fCamAngleH<0.0f) m_fCamAngleH=0.0f;
          }
          else {
            m_fCamAngleH+=5.0f;
            if (m_fCamAngleH>0.0f) m_fCamAngleH=0.0f;
          }
        }

        if (m_fCamAngleV!=0.0f) {
          if (m_fCamAngleV>0.0f) {
            m_fCamAngleV-=5.0f;
            if (m_fCamAngleV<0.0f) m_fCamAngleV=0.0f;
          }
          else {
            m_fCamAngleV+=5.0f;
            if (m_fCamAngleV>0.0f) m_fCamAngleV=0.0f;
          }
        }
      }

      if (m_pController->get(m_pCtrls[eCarDifferential])) {
        m_pController->set(m_pCtrls[eCarDifferential],0.0f);
        m_bDifferential=!m_bDifferential;
        m_pCockpit->setDifferentialEnabled(m_bDifferential);
      }
    }

    f32 fVel=m_pCarBody->getLinearVelocity().getLength();
    if ((m_fOldVel>2.0f && fVel<=2.0f) || (m_fOldVel<-2.0f && fVel>=-2.0f)) {
      //printf("**** set parameter slip\n");
      for (u32 i=0; i<2; i++) {
        m_pWheels[i  ]->setSurfaceParameter(0,m_pParams[1]);
        m_pWheels[i+2]->setSurfaceParameter(0,m_pParams[3]);
      }
    }

    if ((m_fOldVel<2.0f && fVel>=2.0f) || (m_fOldVel>-2.0f && fVel<=-2.0f)) {
      //printf("**** set parameter noslip\n");
      for (u32 i=0; i<2; i++) {
        m_pWheels[i  ]->setSurfaceParameter(0,m_pParams[0]);
        m_pWheels[i+2]->setSurfaceParameter(0,m_pParams[2]);
      }
    }

    if (fVel>2.0f || fVel<-2.0f) {
      f32 fFact=fVel-2.0f;
      if (fFact<100.0f) {
        fFact=0.75f*(fFact/100.0f);
      }
      else
        fFact=0.75f;

      if (fFact>0.0f)
        for (u32 i=0; i<4; i++) {
          m_pParams[i]->setSlip1(fFact);
        }
    }

    m_fOldVel=fVel;

    f32 fRpm=(m_pAxesRear[0]->getHingeAngleRate()+m_pAxesRear[1]->getHingeAngleRate())/2.0f;

    if (m_fRpm>fRpm) {
      bDataChanged=true;
      m_fRpm-=1.5f;
      if (m_fRpm<fRpm) m_fRpm=fRpm;
    }

    if (m_fRpm<fRpm) {
      bDataChanged=true;
      m_fRpm+=1.5f;
      if (m_fRpm>fRpm) m_fRpm=fRpm;
    }

    //Send an event if the car's state has changed
    f32 fRot=(m_pAxesRear[0]->getHingeAngleRate()+m_pAxesRear[1]->getHingeAngleRate())/2.0f,fSound=0.75f;
    if (fRot<0.0f) {
      bDataChanged=true;
      fRot=-fRot;
    }

    if (fRot>5.0f) {
      if (fRot>155.0f)
        fSound=5.75f;
      else
        fSound=0.75f+(5.0f*(fRot-5.0f)/150.0f);
    }

    if (m_fSound<fSound) {
      m_fSound+=0.025f;
      if (m_fSound>=fSound) {
        bDataChanged=true;
        m_fSound=fSound;
      }
    }

    if (m_fSound>fSound) {
      m_fSound-=0.025f;
      if (m_fSound<=fSound) {
        m_fSound=fSound;
        bDataChanged=true;
      }
    }

    if (m_pCarBody->getCollision()) {
      f32 fImpulse=(m_pCarBody->getLinearVelocity()-m_vOldSpeed).getLength();
      if (fImpulse<0.0f) fImpulse=-fImpulse;

      if (fImpulse>5.0f) {
        fImpulse-=5.0f;
        fImpulse/=50.0f;
        if (fImpulse>1.0f) fImpulse=1.0f;

        CEventFireSound *pSnd=new CEventFireSound(CEventFireSound::eSndCrash,fImpulse,m_pCarBody->getPosition());
        ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(pSnd);
      }
    }

    m_vOldSpeed=m_pCarBody->getLinearVelocity();

    f32 fSlider=m_pJointSus->getSliderPosition();
    if (fSlider!=m_fOldSlider) {
      bDataChanged=true;
      m_fOldSlider=fSlider;
    }

    if (bDataChanged) {
      bDataChanged=false;
      dataChanged();
    }

    core::vector3df r = m_pCarBody->getAbsoluteTransformation().getRotationDegrees(),
                    v = m_pCarBody->getLinearVelocity(),
                    p = m_pCarBody->getAbsolutePosition(),
                    f = r.rotationToDirection(core::vector3df(-1.0f,0.0f,0.0f)),
                    s = r.rotationToDirection(core::vector3df(0.0f,0.0f,1.0f)),
                    u = r.rotationToDirection(core::vector3df(0.0f,1.0f,0.0f));

    f32 fFact = v.getLength()<10.0f?0.0f:v.getLength()>100.0f?0.1f:0.1f*(v.getLength()-10.0f)/90.0f;
    v.normalize();

    f32 f1 = v.dotProduct(f), f2 = 125.0f*v.dotProduct(s), f3 = 125.0f*v.dotProduct(u);

    m_pCarBody->addForceAtPosition(p-2.0f*f,(-fFact*f2*s)+(-fFact*f3*u));
    m_pCarBody->addForceAtPosition(p+2.0f*f,( fFact*f2*s)+( fFact*f3*u));
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventTrigger) {
    irr::ode::CIrrOdeEventTrigger *pTrig=(irr::ode::CIrrOdeEventTrigger *)pEvent;
    if (pTrig->getBodyId()==m_iBodyId) {
      if (pTrig->getTriggerId()==1) {   //gas station
        core::list<s32>::Iterator it;

        if (!m_bGasLastStep) {
          const core::vector3df v=pTrig->getPosition();
          CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndBell,2.0f,v);
          ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
        }
        m_bGasStation=true;
      }
    }
  }
  return false;
}

bool CCar::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep || pEvent->getType()==irr::ode::eIrrOdeEventTrigger;
}

ode::IIrrOdeEvent *CCar::writeEvent() {
  u8 iFlags=0;

  if (m_bBoost) iFlags|=CEventCarState::eCarFlagBoost;
  if (m_bBrake) iFlags|=CEventCarState::eCarFlagBrake;

  CEventCarState *pEvent=new CEventCarState(m_pCarBody->getID(),
                                            m_pJointSus->getSliderPosition(),
                                            m_pAxesRear[0]->getHingeAngle()*180.0f/PI,
                                            m_pAxesRear[1]->getHingeAngle()*180.0f/PI,
                                            m_fRpm,m_fDiff,m_fSound,m_fSteer*180.0f/PI,iFlags);

  return pEvent;
}

ode::eEventWriterType CCar::getEventWriterType() {
  return ode::eIrrOdeEventWriterUnknown;
}

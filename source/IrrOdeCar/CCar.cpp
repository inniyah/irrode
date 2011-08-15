  #include <irrlicht.h>
  #include "CCar.h"
  #include <CCustomEventReceiver.h>
  #include <CCockpitCar.h>
  #include <math.h>
  #include <irrklang.h>

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

CCar::CCar(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitCar *pCockpit, irrklang::ISoundEngine *pSndEngine) : CIrrOdeCarState(pDevice,L"Car","../../data/irrOdeCarHelp.txt", pCtrl,pSndEngine) {
  //get the car body
  m_pCarBody=reinterpret_cast<ode::CIrrOdeBody *>(pNode);
  m_fSound=0.75f;

  if (m_pCarBody) {
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

    //get the two motors that are attached to the rear wheels
    findNodesOfType(m_pCarBody,(ESCENE_NODE_TYPE)irr::ode::IRR_ODE_MOTOR_ID,aNodes);

    if (aNodes.size()>=2) {
      m_pMotor[0]=dynamic_cast<ode::CIrrOdeMotor *>(aNodes[0]);
      m_pMotor[1]=dynamic_cast<ode::CIrrOdeMotor *>(aNodes[1]);
    }

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
    m_bAdaptSteer=true;
    m_iThrottle=0;

    m_fCamAngleH=0.0f;
    m_fCamAngleV=0.0f;

    //add a camera
    m_pCam=m_pSmgr->addCameraSceneNode();
    m_pCam->setNearValue(0.1f);

    m_pTab=m_pGuiEnv->addTab(core::rect<s32>(0,0,300,300));
    //add a static text element that will show useful information
    m_pInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,5,150,55),true,false,m_pTab);
    m_pInfo->setDrawBackground(true);
    m_pInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
    m_pTab->setVisible(false);
    m_pCtrls=NULL;

    //we are an IrrOde event listener
    ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
    m_pCockpit=pCockpit;

    m_bInternal=false;

    m_bInitialized=true;
  }

  m_pSound=m_pSndEngine->play3D("../../data/sound/car.ogg",irrklang::vec3df(0.0f,0.0f,0.0f),true,true);

  if (m_pSound) m_pSound->setMinDistance(25.0f);

  m_vOldSpeed=core::vector3df(0.0f,0.0f,0.0f);
}

CCar::~CCar() {
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
  if (m_pSound) m_pSound->drop();
}

//This method is called when the state is activated.
void CCar::activate() {
  if (m_pSound!=NULL) m_pSound->setIsPaused(false);
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
}

//This method is called once for each rendered frame.
u32 CCar::update() {
  //call the superclasse's update method
  u32 iRet=CIrrOdeCarState::update();

  //the car's velocity
  f32 v=m_pCarBody->getLinearVelocity().getLength();
  //is the adaptive steer option is not active ...
  if (!m_bAdaptSteer)
    m_fActSteer=45.0f;   //... just use the default steering angle of 45 degrees, otherwise ...
  else
    if (v<10.0f)
      m_fActSteer=45.0f;    //... just use this value if the velocity is low. If the velocity is higher, even ...
    else
      if (v>45.0f)
        m_fActSteer=10.0f;  //... higher than 45 we use the minimum value of 10 degrees. If the speed is between 10 ...
      else
        //and 45 we calculate the actual steering angle
        m_fActSteer=45.0f-(35.0f)*(v-10.0f)/35.0f;

  //get the parameters for the camera ...
  vector3df pos=m_pCarBody->getRotation().rotationToDirection(m_bInternal?vector3df(0,1.35,0):vector3df(8,4,0)),
            up =m_pCarBody->getRotation().rotationToDirection(vector3df(0,0.2,0)),
            tgt=m_pCarBody->getRotation().rotationToDirection(m_bInternal?vector3df(-5,1.35,0):vector3df(0,2,0)),
            rot=m_pCarBody->getRotation();

  if (m_bInternal) {
    core::vector2df lookAt=core::vector2df(0.0f,-5.0f).rotateBy(m_fCamAngleH),
                    lookUp=core::vector2df(5.0f, 0.0f).rotateBy(m_fCamAngleV);

    pos=rot.rotationToDirection(vector3df(0,1.35,0)),
    up =rot.rotationToDirection(vector3df(0,1,0));
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
  m_pInfo->setText(dummy);

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
    if (m_bActive) {
      bool bBoost=m_pController->get(m_pCtrls[eCarBoost])!=0.0f;

      if (bBoost!=m_bBoost) m_pCockpit->setBoost(bBoost);
      m_bBoost=bBoost;

      f32 fForeward=m_pController->get(m_pCtrls[eCarForeward]),
          fSpeed=-0.8f*(m_pAxesFront[0]->getHingeAngle2Rate()+m_pAxesFront[1]->getHingeAngle2Rate())/2;

      if (fForeward!=0.0f) {
        f32 fForce=fForeward<0.0f?-fForeward:fForeward;

        for (u32 i=0; i<2; i++) {
          m_pMotor[i]->setVelocity(-250.0*fForeward);
          m_pMotor[i]->setForce(bBoost?55*fForce:30*fForce);
          m_iThrottle=-1;
        }
      }
      else
        for (u32 i=0; i<2; i++) {
          m_pMotor[i]->setVelocity(0.0f);
          m_pMotor[i]->setForce(5.0f);
        }

      f32 fSteer=m_pController->get(m_pCtrls[eCarLeft]);

      if (fSteer!=0.0f)
        for (u32 i=0; i<2; i++) m_pServo[i]->setServoPos(m_fActSteer*fSteer);
      else
        for (u32 i=0; i<2; i++) m_pServo[i]->setServoPos(0.0f);

      if (m_pController->get(m_pCtrls[eCarBrake])!=0.0f)
        for (u32 i=0; i<2; i++) {
            m_pMotor[i]->setVelocity(0.0f);
            m_pMotor[i]->setForce(75);
        }

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
    }

    if (m_pSndEngine!=NULL) {
      core::vector3df irrPos=m_pCarBody->getPosition(),
                      irrVel=m_pCarBody->getLinearVelocity();

      irrklang::vec3df vPos=irrklang::vec3df(irrPos.X,irrPos.Y,irrPos.Z),
                       vVel=irrklang::vec3df(irrVel.X,irrVel.Y,irrVel.Z);

      if (m_pSound!=NULL) {
        f32 fRot=(m_pAxesRear[0]->getHingeAngleRate()+m_pAxesRear[1]->getHingeAngleRate())/2.0f,fSound=0.75f;
        if (fRot<0.0f) fRot=-fRot;

        if (fRot>5.0f) {
          if (fRot>155.0f)
            fSound=5.75f;
          else
            fSound=0.75f+(5.0f*(fRot-5.0f)/150.0f);
        }

        if (m_fSound<fSound) {
          m_fSound+=0.025f;
          if (m_fSound>=fSound) m_fSound=fSound;
        }

        if (m_fSound>fSound) {
          m_fSound-=0.025f;
          if (m_fSound<=fSound) m_fSound=fSound;
        }

        m_pSound->setVelocity(vVel);
        m_pSound->setPosition(vPos);
        m_pSound->setPlaybackSpeed(m_fSound);
      }

      f32 fImpulse=(vVel-m_vOldSpeed).getLength();
      if (fImpulse<0.0f) fImpulse=-fImpulse;

      if (fImpulse>5.0f) {
        fImpulse-=5.0f;
        fImpulse/=50.0f;
        if (fImpulse>1.0f) fImpulse=1.0f;

        irrklang::ISound *pSnd=m_pSndEngine->play3D("../../data/sound/crash.ogg",vPos,false,true);
        pSnd->setVolume(fImpulse);
        pSnd->setIsPaused(false);
      }

      m_vOldSpeed=irrVel;
    }
  }
  return false;
}

bool CCar::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

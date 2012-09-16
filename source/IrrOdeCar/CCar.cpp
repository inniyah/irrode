  #include <irrlicht.h>
  #include "CCar.h"
  #include <CCustomEventReceiver.h>
  #include <CCockpitCar.h>
  #include <math.h>
  #include <irrklang.h>
  #include <CRearView.h>
  #include <CEventVehicleState.h>
  #include <CIrrOdeCarTrack.h>
  #include <CControlEvents.h>

  #include <irrCC.h>

void findNodesOfType(irr::scene::ISceneNode *pParent, irr::scene::ESCENE_NODE_TYPE iType, irr::core::array<irr::scene::ISceneNode *> &aNodes) {
  irr::core::list<irr::scene::ISceneNode *> children=pParent->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) {
    if ((*it)->getType()==iType) {
      aNodes.push_back(*it);
    }
    findNodesOfType(*it,iType,aNodes);
  }
}

CCar::CCar(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CRearView *pRView, irr::ode::IIrrOdeEventQueue *pInputQueue) : CIrrOdeCarState(pDevice,L"Car", pInputQueue) {
  irr::ode::IIrrOdeEventWriter::setWorld(reinterpret_cast<irr::ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode")));
  //get the car body
  m_pCarBody=reinterpret_cast<irr::ode::CIrrOdeBody *>(pNode);
  m_fSound     = 0.75f;
  m_fOldSlider = 0.0f;
  m_fCtrlSteer = 0.0f;
  m_fSteer     = 0.0f;
  m_fSpeed     = 0.0f;

  m_iBoost = 1800;

  m_bGasStation  = false;
  m_bGasLastStep = false;
  m_bBoost       = false;
  m_bFlip        = false;
  m_bAdapt       = true;

  m_vWheelOld[0] = irr::core::vector3df(0.0f,0.0f,0.0f);
  m_vWheelOld[1] = irr::core::vector3df(0.0f,0.0f,0.0f);

  if (m_pCarBody) {
    m_iBodyId=m_pCarBody->getID();
    m_pCarBody->setUserData(this);
    CCustomEventReceiver::getSharedInstance()->addCar(m_pCarBody);
    irr::core::array<irr::scene::ISceneNode *> aNodes;

    m_pAxesFront[0]=reinterpret_cast<irr::ode::CIrrOdeJointHinge2 *>(m_pCarBody->getChildByName("axis_fl",m_pCarBody));
    m_pAxesFront[1]=reinterpret_cast<irr::ode::CIrrOdeJointHinge2 *>(m_pCarBody->getChildByName("axis_fr",m_pCarBody));

    printf("axis_FL=%i\n",(int)m_pAxesFront[0]);
    printf("axis_FR=%i\n",(int)m_pAxesFront[1]);

    m_pAxesRear[0]=reinterpret_cast<irr::ode::CIrrOdeJointHinge *>(m_pCarBody->getChildByName("axis_rl",m_pCarBody));
    m_pAxesRear[1]=reinterpret_cast<irr::ode::CIrrOdeJointHinge *>(m_pCarBody->getChildByName("axis_rr",m_pCarBody));

    printf("axis_RL=%i\n",(int)m_pAxesRear[0]);
    printf("axis_RR=%i\n",(int)m_pAxesRear[1]);

    for (irr::u32 i=0; i<2; i++) {
      irr::c8 s[0xFF];
      sprintf(s,"sc_motor%i",i+1);
      m_pMotor[i]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pCarBody->getChildByName(s,m_pCarBody));
      sprintf(s,"brk_fr_%i",i+1);
      m_pBrkFr[i]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pCarBody->getChildByName(s,m_pCarBody));
      sprintf(s,"brk_re_%i",i+1);
      m_pBrkRe[i]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pCarBody->getChildByName(s,m_pCarBody));
    }

    m_pGearBox = new CGearBox(m_pMotor, m_pAxesRear);

    irr::c8 sWheelNames[][20]={ "geom_wheel_fl", "geom_wheel_fr", "wheel_rl", "wheel_rr" };
    for (irr::u32 i=0; i<4; i++)
      m_pWheels[i]=reinterpret_cast<irr::ode::CIrrOdeGeomSphere *>(m_pCarBody->getChildByName(sWheelNames[i],m_pCarBody));

    irr::core::stringw sParamNames[]={ L"surfaceTireFront", L"surfaceTireStop", L"surfaceTireBack", L"surfaceTireStop" };
    for (irr::u32 i=0; i<4; i++) {
      m_pParams[i]=new irr::ode::CIrrOdeSurfaceParameters();
      m_pWorld->getParameter(sParamNames[i])->copy(m_pParams[i]);
    }

    m_pSuspension=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pCarBody->getChildByName("sc_suspension_rear",m_pCarBody));
    if (m_pSuspension!=NULL)
      m_vSuspNeutral=m_pSuspension->getPosition();
    else
      m_vSuspNeutral=irr::core::vector3df(0.0f,0.0f,0.0f);

    m_pJointSus=reinterpret_cast<irr::ode::CIrrOdeJointSlider *>(m_pCarBody->getChildByName("suspension_joint",m_pCarBody));

    irr::c8 sWheelBodies[][20]={ "sc_wheel_rl", "sc_wheel_rr" };
    for (irr::u32 i=0; i<2; i++)
      m_pRearWheels[i]=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pCarBody->getChildByName(sWheelBodies[i],m_pCarBody));

    irr::c8 sFrontWheelBodies[][20] = { "sc_wheel_fr", "sc_wheel_fl"};
    for (irr::u32 i = 0; i < 2; i++)
      m_pFrontWheels[i]=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pCarBody->getChildByName(sFrontWheelBodies[i],m_pCarBody));

    printf("**** motors: %i, %i\n",(int)m_pMotor[0],(int)m_pMotor[1]);
    printf("**** front brakes: %i, %i\n",(int)m_pBrkFr[0],(int)m_pBrkFr[1]);
    printf("**** rear brakes: %i, %i\n",(int)m_pBrkRe[0],(int)m_pBrkRe[1]);
    printf("**** wheels: ");
    for (irr::u32 i=0; i<4; i++) printf("%i%s",(int)m_pWheels[i],i<3?", ":"");
    printf("\n");
    printf("**** wheel bodies: %i, %i, %i, %i\n",(int)m_pRearWheels[0],(int)m_pRearWheels[1],(int)m_pFrontWheels[0],(int)m_pFrontWheels[1]);
    printf("**** params: ");
    for (irr::u32 i=0; i<4; i++) printf("%i%s",(int)m_pParams[i],i<3?", ":"");
    printf("\n");
    printf("**** suspension: %i, %i\n",(int)m_pSuspension,(int)m_pJointSus);
    printf("**** suspension neutral: (%.2f, %.2f, %.2f)\n",m_vSuspNeutral.X,m_vSuspNeutral.Y,m_vSuspNeutral.Z);

    aNodes.clear();
    //get the two servos that are attached to the front wheels
    findNodesOfType(m_pCarBody,(irr::scene::ESCENE_NODE_TYPE)irr::ode::IRR_ODE_SERVO_ID,aNodes);

    if (aNodes.size()>=2) {
      m_pServo[0]=dynamic_cast<irr::ode::CIrrOdeServo *>(aNodes[0]);
      m_pServo[1]=dynamic_cast<irr::ode::CIrrOdeServo *>(aNodes[1]);
    }

    //initialize the members
    m_bHelp=false;
    m_bBrake=false;
    m_fThrottle=0.0f;

    m_fOldVel=0.0f;

    //we are an IrrOde event listener
    irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
    m_pCockpit=NULL;
    m_pRView=pRView;

    m_pLap = new CIrrOdeCarTrack(m_pCarBody);

    m_bInternal=false;

    m_bInitialized=true;
  }

  m_vOldSpeed=irr::core::vector3df(0.0f,0.0f,0.0f);
}

CCar::~CCar() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
  for (irr::u32 i=0; i<4; i++) delete m_pParams[i];
}

//This method is called when the state is activated.
void CCar::activate() {
  if (m_pCockpit) m_pCockpit->setActive(true);
  if (m_pRView  ) m_pRView  ->setActive(true);
}

void CCar::deactivate() {
  for (irr::u32 i=0; i<2; i++) {
    m_pMotor[i]->setVelocity(0.0f);
    m_pMotor[i]->setForce(5.0f);
  }

  for (irr::u32 i=0; i<2; i++) m_pServo[i]->setServoPos(0.0f);

  if (m_pCockpit) m_pCockpit->setActive(false);
  if (m_pRView  ) m_pRView  ->setActive(false);
}

//This method is called once for each rendered frame.
irr::u32 CCar::update() {
  //call the superclasse's update method
  irr::u32 iRet=CIrrOdeCarState::update();

  return iRet;
}

bool CCar::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == eCtrlMsgCar) {
    CCarControls *p = reinterpret_cast<CCarControls *>(pEvent);
    if (p->getNode() == m_iBodyId && p->getClient() == m_iControllerBy) {
      m_fThrottle  = p->getThrottle     ();
      m_fSteer     = p->getSteer        ();
      m_bBoost     = p->getBoost        ();
      m_bFlip      = p->getFlip         ();

      if (p->getShiftUp()) {
        if (m_pGearBox->shiftUp()) {
          CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndShift,0.05f,m_pCarBody->getPosition());
          irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
        }
      }

      if (p->getShiftDown()) {
        if (m_pGearBox->shiftDown()) {
          CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndShift,1.0f,m_pCarBody->getPosition());
          irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
        }
      }

      if (p->getAdaptiveSteer()) {
        m_bAdapt = !m_bAdapt;
      }

      if (p->getDifferential()) {
        m_pGearBox->toggleDifferential();
        dataChanged();
      }

      if (m_fThrottle >  1.0f) m_fThrottle =  1.0f; if (m_fSteer >  1.0f) m_fSteer =  1.0f;
      if (m_fThrottle < -1.0f) m_fThrottle = -1.0f; if (m_fSteer < -1.0f) m_fSteer = -1.0f;
    }
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    if (m_bGasLastStep && !m_bGasStation) {
      const irr::core::vector3df v=m_pCarBody->getPosition();
      CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndBell,2.0f,v);
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
    }

    m_bGasLastStep = m_bGasStation;
    m_bGasStation=false;

    bool bDataChanged=false;

    irr::core::vector3df vForeward=m_pCarBody->getRotation().rotationToDirection(irr::core::vector3df(-1.0f,0.0f,0.0f)),
                         vNormVel=m_pCarBody->getLinearVelocity();

    vNormVel.normalize();

    m_fSpeed=-0.3f*(m_pAxesFront[0]->getHingeAngle2Rate()+m_pAxesFront[1]->getHingeAngle2Rate())/2;

    m_pGearBox->update(m_fThrottle, m_bBoost && m_iBoost > 0);

    if (m_bBoost) {
      if (m_iBoost > 0)
        m_iBoost -= 3;
    }
    else {
      if (m_iBoost < 1800) m_iBoost++;
    }

    irr::f32 v = m_pCarBody->getLinearVelocity().getLength();

    m_fAngle = 22.5f;

    if (m_bAdapt) {
      if (v>50.0f)
        m_fAngle = 10.0f;
      else
        if (v>5.0f)
          m_fAngle = 22.5f - 15.0f * ((v - 5.0f) / 45.0f);
    }

    if (m_fSteer!=0.0f)
      for (irr::u32 i=0; i<2; i++) m_pServo[i]->setServoPos(m_fAngle * m_fSteer);
    else
      for (irr::u32 i=0; i<2; i++) m_pServo[i]->setServoPos(0.0f);

    irr::f32 fForeward = m_fThrottle;

    if (fForeward < 0.0f) {
      fForeward=-fForeward;

      for (irr::u32 i=0; i<2; i++) {
        m_pBrkFr[i]->setVelocity(0.0f); m_pBrkFr[i]->setForce(fForeward*200.0f);
        m_pBrkRe[i]->setVelocity(0.0f); m_pBrkRe[i]->setForce(fForeward*120.0f);
      }
      m_bBrake=true;
    }
    else m_bBrake=false;

    //if the flip car key was pressed we add a force to the car in order to turn it back on it's wheels
    if (m_bFlip) {
      irr::core::vector3df v=m_pCarBody->getAbsoluteTransformation().getRotationDegrees().rotationToDirection(irr::core::vector3df(0,0.3f,0));
      m_pCarBody->addForceAtPosition(m_pCarBody->getPosition()+v,irr::core::vector3df(0,350,0));
    }

    if (m_pRView!=NULL && m_pRView->isActive()) {
      irr::core::vector3df cRot=m_pCarBody->getAbsoluteTransformation().getRotationDegrees(),
                           cPos=m_pCarBody->getAbsolutePosition()+cRot.rotationToDirection(irr::core::vector3df(1.0f,1.75f,0.0f)),
                           cTgt=cPos+cRot.rotationToDirection(irr::core::vector3df(1.0f,0.0f,0.0f)),
                           cUp=cRot.rotationToDirection(irr::core::vector3df(0.0f,1.0f,0.0f));

      m_pRView->setCameraParameters(cPos,cTgt,cUp);
      m_pRView->update();
    }

    irr::f32 fVel=m_pCarBody->getLinearVelocity().getLength();
    if ((m_fOldVel>2.0f && fVel<=2.0f) || (m_fOldVel<-2.0f && fVel>=-2.0f)) {
      for (irr::u32 i=0; i<2; i++) {
        m_pWheels[i  ]->setSurfaceParameter(0,m_pParams[1]);
        m_pWheels[i+2]->setSurfaceParameter(0,m_pParams[3]);
      }
    }

    if ((m_fOldVel<2.0f && fVel>=2.0f) || (m_fOldVel>-2.0f && fVel<=-2.0f)) {
      for (irr::u32 i=0; i<2; i++) {
        m_pWheels[i  ]->setSurfaceParameter(0,m_pParams[0]);
        m_pWheels[i+2]->setSurfaceParameter(0,m_pParams[2]);
      }
    }
    m_fOldVel=fVel;

    irr::f32 fRpm = m_pGearBox->getRpm();

    irr::f32 fSound = 0.75;

    if (fRpm<-0.05f) {
      fSound=0.75f+(-5.0f*(fRpm+0.05f));
      if (fSound > 5.75f) fSound = 5.75f;
    }
    if (m_fSound<fSound) {
      m_fSound+=0.05f;
      if (m_fSound>=fSound) {
        bDataChanged=true;
        m_fSound=fSound;
      }
    }

    if (m_fSound>fSound) {
      m_fSound-=0.05f;
      if (m_fSound<=fSound) {
        m_fSound=fSound;
        bDataChanged=true;
      }
    }

    irr::f32 fImpulse=(m_pCarBody->getLinearVelocity()-m_vOldSpeed).getLength();
    if (fImpulse<0.0f) fImpulse=-fImpulse;

    if (fImpulse > 15.0f) {
      fImpulse-=15.0f;
      fImpulse/=50.0f;
      if (fImpulse>1.0f) fImpulse=1.0f;

      CEventFireSound *pSnd=new CEventFireSound(CEventFireSound::eSndCrash,fImpulse,m_pCarBody->getPosition());
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(pSnd);
    }
    else {
      if (fImpulse > 1.0f) {
        fImpulse -= 1.0f;
        fImpulse /= 50.0f;
        if (fImpulse > 1.0f) fImpulse = 1.0f;

        CEventFireSound *pSnd=new CEventFireSound(CEventFireSound::eSndCreaky,fImpulse,m_pCarBody->getPosition());
        irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(pSnd);
      }
    }

    m_vOldSpeed=m_pCarBody->getLinearVelocity();

    irr::f32 fSlider=m_pJointSus->getSliderPosition();
    if (fSlider!=m_fOldSlider) {
      bDataChanged=true;
      m_fOldSlider=fSlider;
    }

    if (bDataChanged || m_pGearBox->dataChanged()) {
      dataChanged();
    }

    m_bTouch = false;
    for (irr::u32 i = 0; i < 2 && !m_bTouch; i++) {
      m_bTouch |= m_pFrontWheels[i]->getTouched() != NULL || m_pRearWheels[i]->getTouched() != NULL;
    }

    applyAeroEffect();
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventTrigger) {
    irr::ode::CIrrOdeEventTrigger *pTrig=(irr::ode::CIrrOdeEventTrigger *)pEvent;
    if (pTrig->getBodyId()==m_iBodyId) {
      if (pTrig->getTriggerId()==1) {   //gas station
        irr::core::list<irr::s32>::Iterator it;

        if (!m_bGasLastStep) {
          const irr::core::vector3df v=pTrig->getPosition();
          CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndBell,2.0f,v);
          irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
        }
        m_bGasStation=true;
      }
    }
  }

  return false;
}

void CCar::applyAeroEffect() {
  irr::core::vector3df r = m_pCarBody->getAbsoluteTransformation().getRotationDegrees(),
                  v = m_pCarBody->getLinearVelocity(),
                  p = m_pCarBody->getAbsolutePosition(),
                  f = r.rotationToDirection(irr::core::vector3df(-1.0f,0.0f,0.0f)),
                  s = r.rotationToDirection(irr::core::vector3df(0.0f,0.0f,1.0f)),
                  u = r.rotationToDirection(irr::core::vector3df(0.0f,1.0f,0.0f));

  irr::f32 fFact = v.getLength()<10.0f?0.0f:v.getLength()>100.0f?0.1f:0.1f*(v.getLength()-10.0f)/90.0f;
  v.normalize();

  irr::f32 f1 = 125.0f*v.dotProduct(s), f2 = 125.0f*v.dotProduct(u);

  m_pCarBody->addForceAtPosition(p-2.0f*f,(-fFact*f1*s)+(-fFact*f2*u));
  m_pCarBody->addForceAtPosition(p+2.0f*f,( fFact*f1*s)+( fFact*f2*u));
}

bool CCar::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep || pEvent->getType()==irr::ode::eIrrOdeEventTrigger || pEvent->getType() == eCtrlMsgCar;
}

irr::ode::IIrrOdeEvent *CCar::writeEvent() {
  irr::u8 iFlags=0;

  if (m_bBrake                  ) iFlags|=CEventCarState::eCarFlagBrake;
  if (m_pGearBox->differential()) iFlags|=CEventCarState::eCarFlagDifferential;
  if (m_pGearBox->exhaustSmoke()) iFlags|=CEventCarState::eCarFlagSmoke;
  if (m_bTouch                  ) iFlags|=CEventCarState::eCarFlagTouch;
  if (m_bBoost && m_iBoost > 0  ) iFlags|=CEventCarState::eCarFlagBoost;
  if (m_bAdapt                  ) iFlags|=CEventCarState::eCarFlagAdapt;

  irr::f32 fSuspension = m_pJointSus->getSliderPosition();
  if (fSuspension < -0.25f) fSuspension = -0.25f;

  irr::core::vector3df cPos, v1, v2, r1, r2, vRotBody,
                       a1 = m_pAxesFront[0]->getPosition(),
                       a2 = m_pAxesFront[1]->getPosition(),
                       vDir = m_pCarBody->getRotation().rotationToDirection(irr::core::vector3df(-1.0f, 0.0f, 0.0f)),
                       vWheel[2], v;

  m_pCarBody->getOdePosition(cPos);
  m_pCarBody->getOdeRotation(vRotBody);
  m_pFrontWheels[0]->getOdePosition(v1);
  m_pFrontWheels[0]->getOdePosition(v2);

  v1 -= cPos;
  v2 -= cPos;

  irr::core::CMatrix4<irr::f32> cMat, cInv = cMat.setInverseRotationDegrees(vRotBody);

  cInv.rotateVect(r1, v1);
  cInv.rotateVect(r2, v2);

  irr::f32 p1 = r1.Y < -0.1f ? r1.Y : -0.1f,
           p2 = r2.Y < -0.1f ? r1.Y : -0.1f;

  irr::f32 fRot[2], fPos[2] = { p1, p2 };

  for (irr::u32 i = 0; i < 2; i++) {
    m_pFrontWheels[i]->getOdePosition(vWheel[i]);
    v = vDir.crossProduct(m_vWheelOld[i] - vWheel[i]);

    irr::core::vector3df vRot;
    m_pFrontWheels[i]->getOdeRotation(vRot);

    if (m_fSpeed < 0.0f)
      fRot[i] = vRot.Z - v.getLength() * 1080.0f;
    else
      fRot[i] = vRot.Z + v.getLength() * 1080.0f;

    while (fRot[i] >= 360.0f) fRot[i] -= 360.0f;
    while (fRot[i] <    0.0f) fRot[i] += 360.0f;
    m_vWheelOld[i] = vWheel[i];
  }

  irr::f32 fSteer = m_fSteer * m_fAngle;

  CEventCarState *pEvent=new CEventCarState(m_pCarBody->getID(), fSuspension,
                                            m_pAxesRear[0]->getHingeAngle()*180.0f/irr::core::PI,
                                            m_pAxesRear[1]->getHingeAngle()*180.0f/irr::core::PI,
                                            m_pGearBox->getRpm(),m_pGearBox->getDiff(),m_fSound,fSteer,iFlags,m_fSpeed,m_pGearBox->getGear(), m_iBoost, fPos, fRot);

  return pEvent;
}

irr::ode::eEventWriterType CCar::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

CCar::CGearBox::CGearBox(irr::ode::CIrrOdeMotor *pMotor[2], irr::ode::CIrrOdeJointHinge *pAxesRear[2]) {
  for (irr::u16 i = 0; i< 2; i++) {
    m_pMotor[i] = pMotor[i];
    m_pAxesRear[i] = pAxesRear[i];
  }

  m_iGear = 0;
  m_iClutch = 0;
  m_fRpm = 0.0f;
  m_fDiff = 0.0f;
  m_fThrottle = 0.0f;

  m_bDifferential = true;
  m_bDataChanged = false;

  m_fVelocity[0] = -100.0f; m_fForce[0] =  40.0f;
  m_fVelocity[1] = -150.0f; m_fForce[1] =  35.0f;
  m_fVelocity[2] = -200.0f; m_fForce[2] =  30.0f;
  m_fVelocity[3] = -250.0f; m_fForce[3] =  25.0f;
  m_fVelocity[4] = -300.0f; m_fForce[4] =  20.5f;
}

bool CCar::CGearBox::shiftUp() {
  if (m_iGear < 5) {
    m_iGear++;
    m_iClutch = 12;
    return true;
  }
  return false;
}

bool CCar::CGearBox::shiftDown() {
  if (m_iGear > -1) {
    m_iGear--;
    m_iClutch = 6;
    return true;
  }
  return false;
}

void CCar::CGearBox::update(irr::f32 fThrottle, bool bBoost) {
  //calculate differntial (if active)
  m_fThrottle = fThrottle;

  irr::f32 f1=m_pAxesRear[0]->getHingeAngleRate(),
           f2=m_pAxesRear[1]->getHingeAngleRate(),
           fDiff=f1-f2,fDiffFact[2]={ 1.0f, 1.0f };

  if (m_bDifferential) {
    if (fDiff>2.5f || fDiff<-2.5f) {
      if (fDiff> 150.0f) fDiff= 150.0f;
      if (fDiff<-150.0f) fDiff=-150.0f;
      fDiffFact[0]=1.0f-fDiff/150.0f;
      fDiffFact[1]=1.0f+fDiff/150.0f;
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
    fDiffFact[0]=1.0f;
    fDiffFact[1]=1.0f;
  }

  if (bBoost) fThrottle *= 3.0f;

  //apply the forces of the motor
  for (irr::u32 i = 0; i < 2; i++) {
    if (fThrottle > 0) {
      if (m_iGear == 0 || m_iClutch > 6) {
        m_pMotor[i]->setVelocity(0.0f);
        m_pMotor[i]->setForce(0.0f);
      }
      else
        if (m_iGear >= 1) {
          m_pMotor[i]->setVelocity(m_fVelocity[m_iGear-1]);
          m_pMotor[i]->setForce(fThrottle*fDiffFact[i]*m_fForce[m_iGear-1]);
        }
        else {
          m_pMotor[i]->setVelocity(25.0f);
          m_pMotor[i]->setForce(fThrottle*fDiffFact[i]*20.0f);
        }
    }
    else {
      m_pMotor[i]->setVelocity(0.0f);
      if (m_iGear != 0) {
        m_pMotor[i]->setForce(15.0f);
      }
      else {
        m_pMotor[i]->setForce(1.0f);
      }
    }
  }

  irr::f32 fRpm = 0.0f;
  if (m_iGear != 0 && m_iClutch == 0)
    fRpm=(m_pAxesRear[0]->getHingeAngleRate()+m_pAxesRear[1]->getHingeAngleRate())/(2.0f*getMaxVelocity());
  else
    fRpm = -0.95f*fThrottle;

  if (m_fRpm>fRpm) {
    m_bDataChanged=true;
    m_fRpm-=0.005f;
    if (m_fRpm<fRpm) m_fRpm=fRpm;
  }

  if (m_fRpm<fRpm) {
    m_bDataChanged=true;
    m_fRpm+=0.005f;
    if (m_fRpm>fRpm) m_fRpm=fRpm;
  }

  if (m_iClutch > 0) m_iClutch--;
}

irr::s8 CCar::CGearBox::getGear() {
  return m_iGear;
}

irr::f32 CCar::CGearBox::getMaxVelocity() {
  switch (m_iGear) {
    case -1: return -45.0f;
    case  0: return 0.0f;
    default: return -m_fVelocity[m_iGear-1];
  }
}

bool CCar::CGearBox::dataChanged() {
  bool bRet = m_bDataChanged;
  m_bDataChanged = false;
  return bRet;
}

bool CCar::CGearBox::exhaustSmoke() {
  return m_fRpm < -0.8f || m_fRpm > 0.8f || m_iClutch > 0;
}

bool CCar::CGearBox::differential() {
  return m_bDifferential;
}

void CCar::CGearBox::toggleDifferential() {
  m_bDifferential = !m_bDifferential;
}

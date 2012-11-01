  #include <CVehicle.h>

  #include <thread/IThread.h>

  #include <CCustomEventReceiver.h>
  #include <CEventVehicleState.h>
  #include <CTargetSelector.h>
  #include <CIrrOdeCarTrack.h>
  #include <CControlEvents.h>
  #include <CCockpitPlane.h>
  #include <CCockpitCar.h>
  #include <CProjectile.h>
  #include <CAutoPilot.h>
  #include <CRearView.h>

void CVehicle::removeFromScene(irr::scene::ISceneNode *pNode, irr::ode::CIrrOdeWorld *pWorld) {
  pWorld->removeTreeFromPhysics(pNode);
  irr::s32 iNodeId=pNode->getID();
  pNode->remove();

  irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
}

void CVehicle::fillBodyList(irr::core::list<irr::scene::ISceneNode *> &aVehicles, irr::scene::ISceneNode *pNode, const irr::c8 *sClassName, irr::u32 iMax, irr::ode::CIrrOdeWorld *pWorld) {
  if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
    irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
    if (p->getOdeClassname().equals_ignore_case(sClassName)) {
      printf("%s found (%i)\n",sClassName,aVehicles.size());
      if (aVehicles.size()<iMax)
        aVehicles.push_back(pNode);
      else {
        removeFromScene(pNode, pWorld);
        return;
      }
    }
  }

  irr::core::list<irr::scene::ISceneNode *> children=pNode->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) fillBodyList(aVehicles,*it,sClassName,iMax, pWorld);
}

CVehicle::CVehicle(irr::IrrlichtDevice *pDevice, irr::u32 iNumCars, irr::u32 iNumPlanes, irr::u32 iNumHelis, irr::u32 iNumTanks, irr::ode::CIrrOdeWorld *pWorld, bool bRearView) {
  m_pDevice = pDevice;
  m_pWorld = pWorld;

  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getInputQueue()->addEventListener(this);

  m_pSmgr = pDevice->getSceneManager();

  fillBodyList(m_lCars  ,m_pSmgr->getRootSceneNode(),"car"  ,iNumCars  , m_pWorld);
  fillBodyList(m_lPlanes,m_pSmgr->getRootSceneNode(),"plane",iNumPlanes, m_pWorld);
  fillBodyList(m_lTanks ,m_pSmgr->getRootSceneNode(),"tank" ,iNumTanks , m_pWorld);
  fillBodyList(m_lHelis ,m_pSmgr->getRootSceneNode(),"heli" ,iNumHelis , m_pWorld);

  printf("\nvehicles found:\n\ncars: %i (%i)\nplanes: %i (%i)\nhelicopters: %i (%i)\ntanks: %i (%i)\n\n",m_lCars.size(),iNumCars,m_lPlanes.size(),iNumPlanes,m_lHelis.size(),iNumHelis,m_lTanks.size(),iNumTanks);

  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=m_lPlanes.begin(); it!=m_lPlanes.end(); it++) {
    CPlane *p=new CPlane(m_pDevice, *it);
    m_lVehicles.push_back(p);
  }

  for (it=m_lCars.begin(); it!=m_lCars.end(); it++) {
    CCar *p=new CCar(m_pDevice,*it);
    m_lVehicles.push_back(p);
  }

  for (it=m_lTanks.begin(); it!=m_lTanks.end(); it++) {
    CVehicle::CTank *p=new CVehicle::CTank(m_pDevice,*it);
    m_lVehicles.push_back(p);
  }

  for (it=m_lHelis.begin(); it!=m_lHelis.end(); it++) {
    CHeli *p=new CHeli(m_pDevice,*it);
    m_lVehicles.push_back(p);
  }

  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->addEventListener(this);
}

CVehicle::~CVehicle() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->removeEventListener(this);
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getInputQueue ()->removeEventListener(this);
}

bool CVehicle::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == eCtrlMsgRequestVehicle) {
    CRequestVehicle *pEvt = reinterpret_cast<CRequestVehicle *>(pEvent);
    printf("Client %i requests vehicle %i\n", pEvt->getClient(), pEvt->getNode());

    irr::core::list<CIrrOdeCarState *>::Iterator it;
    for (it = m_lVehicles.begin(); it != m_lVehicles.end(); it++) {
      CIrrOdeCarState *p = *it;
      if (p->getBody() != NULL && p->getBody()->getID() == pEvt->getNode()) {
        printf("Vehicle is \"%s\"\n", p->getBody()->getName());

        if (p->getControlledBy() == -1) {
          p->setControlledBy(pEvt->getClient());
          CVehicleApproved *pOk = new CVehicleApproved();
          pOk->setNode  (pEvt->getNode  ());
          pOk->setClient(pEvt->getClient());

          irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(pOk);
          return false;
        }
        else printf("Sorry ... vehicle is occupied by %i.\n", p->getControlledBy());
      }
    }
  }

  if (pEvent->getType() == eCtrlMsgLeaveVehicle) {
    CLeaveVehicle *pEvt = reinterpret_cast<CLeaveVehicle *>(pEvent);
    if (pEvt->getAnswer() == 0) {
      printf("%i leaves vehicle %i\n", pEvt->getClient(), pEvt->getNode());
      irr::core::list<CIrrOdeCarState *>::Iterator it;
      for (it = m_lVehicles.begin(); it != m_lVehicles.end(); it++) {
        CIrrOdeCarState *p = *it;
        if (p->getBody() != NULL && p->getBody()->getID() == pEvt->getNode()) {
          printf("Vehicle is \"%s\"\n", p->getBody()->getName());
          if (p->getControlledBy() == pEvt->getClient()) {
            printf("OK, vehicle occupied by correct client ... abandon!\n");

            CLeaveVehicle *pLeave = new CLeaveVehicle(1);
            pLeave->setNode(p->getBody()->getID());
            pLeave->setClient(p->getControlledBy());
            irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(pLeave);

            p->setControlledBy(-1);
            return false;
          }
        }
      }
    }
  }

  return true;
}

bool CVehicle::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == irr::ode::eIrrOdeEventStep || pEvent->getType() == eCtrlMsgRequestVehicle || pEvent->getType() == eCtrlMsgLeaveVehicle;
}


//Implementation of CCarControls
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

CVehicle::CCar::CCar(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode) : CIrrOdeCarState(pDevice,L"Car") {
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
    irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->addEventListener(this);

    m_pLap = new CIrrOdeCarTrack(m_pCarBody);

    m_bInternal=false;

    m_bInitialized=true;
  }

  m_vOldSpeed=irr::core::vector3df(0.0f,0.0f,0.0f);
}

CVehicle::CCar::~CCar() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->removeEventListener(this);
  for (irr::u32 i=0; i<4; i++) delete m_pParams[i];
}

//This method is called when the state is activated.
void CVehicle::CCar::activate() {
}

void CVehicle::CCar::deactivate() {
  for (irr::u32 i=0; i<2; i++) {
    m_pMotor[i]->setVelocity(0.0f);
    m_pMotor[i]->setForce(5.0f);
  }

  for (irr::u32 i=0; i<2; i++) m_pServo[i]->setServoPos(0.0f);
}

//This method is called once for each rendered frame.
irr::u32 CVehicle::CCar::update() {
  //call the superclasse's update method
  irr::u32 iRet=CIrrOdeCarState::update();

  return iRet;
}

bool CVehicle::CCar::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
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
          irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
        }
      }

      if (p->getShiftDown()) {
        if (m_pGearBox->shiftDown()) {
          CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndShift,1.0f,m_pCarBody->getPosition());
          irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
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
      irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
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
      if (m_iBoost < 1800) m_iBoost+=2;
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

    irr::f32 fVel=m_pCarBody->getLinearVelocity().getLength();
    if ((m_fOldVel>2.0f && fVel<=2.0f) || (m_fOldVel<-2.0f && fVel>=-2.0f)) {
      for (irr::u32 i=0; i<2; i++) {
        m_pWheels[i  ]->setSurfaceParameter(0,m_pParams[1]);
        m_pWheels[i+2]->setSurfaceParameter(0,m_pParams[3]);
      }
    }

    if ((m_fOldVel<2.0f && fVel>=2.0f) || (m_fOldVel>-2.0f && fVel<=-2.0f) || m_fThrottle >= 0.1f) {
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
      irr::ode::CIrrOdeManager::getSharedInstance()->getIrrThread()->getInputQueue()->postEvent(pSnd);
    }
    else {
      if (fImpulse > 1.0f) {
        fImpulse -= 1.0f;
        fImpulse /= 50.0f;
        if (fImpulse > 1.0f) fImpulse = 1.0f;

        CEventFireSound *pSnd=new CEventFireSound(CEventFireSound::eSndCreaky,fImpulse,m_pCarBody->getPosition());
        irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(pSnd);
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
          irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
        }
        m_bGasStation=true;
      }
    }
  }

  return false;
}

void CVehicle::CCar::applyAeroEffect() {
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

bool CVehicle::CCar::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep || pEvent->getType()==irr::ode::eIrrOdeEventTrigger || pEvent->getType() == eCtrlMsgCar;
}

irr::ode::IIrrOdeEvent *CVehicle::CCar::writeEvent() {
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
  m_pFrontWheels[1]->getOdePosition(v2);

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

irr::ode::eEventWriterType CVehicle::CCar::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

CVehicle::CCar::CGearBox::CGearBox(irr::ode::CIrrOdeMotor *pMotor[2], irr::ode::CIrrOdeJointHinge *pAxesRear[2]) {
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

bool CVehicle::CCar::CGearBox::shiftUp() {
  if (m_iGear < 5) {
    m_iGear++;
    m_iClutch = 12;
    return true;
  }
  return false;
}

bool CVehicle::CCar::CGearBox::shiftDown() {
  if (m_iGear > -1) {
    m_iGear--;
    m_iClutch = 6;
    return true;
  }
  return false;
}

void CVehicle::CCar::CGearBox::update(irr::f32 fThrottle, bool bBoost) {
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

irr::s8 CVehicle::CCar::CGearBox::getGear() {
  return m_iGear;
}

irr::f32 CVehicle::CCar::CGearBox::getMaxVelocity() {
  switch (m_iGear) {
    case -1: return -45.0f;
    case  0: return 0.0f;
    default: return -m_fVelocity[m_iGear-1];
  }
}

bool CVehicle::CCar::CGearBox::dataChanged() {
  bool bRet = m_bDataChanged;
  m_bDataChanged = false;
  return bRet;
}

bool CVehicle::CCar::CGearBox::exhaustSmoke() {
  return m_fRpm < -0.8f || m_fRpm > 0.8f || m_iClutch > 0;
}

bool CVehicle::CCar::CGearBox::differential() {
  return m_bDifferential;
}

void CVehicle::CCar::CGearBox::toggleDifferential() {
  m_bDifferential = !m_bDifferential;
}

//Implementation of CPlane
CVehicle::CPlane::CPlane(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode) : CAeroVehicle(pDevice, pNode) {

  CCustomEventReceiver::getSharedInstance()->addPlane(m_pBody);
  //get the visual rudders
  m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor);
  m_pAutoPilot->setLinkYawRoll(true);

  m_pTargetSelector=new CTargetSelector(m_pBody,m_pDevice,m_pAero->getForeward());

  printf("\nplane state\n\n");
  m_bLeftMissile=true;

  m_iNextCp=-1;
  irr::scene::ISceneNode *pCheckRoot=m_pSmgr->getSceneNodeFromName("planepoints");
  if (pCheckRoot!=NULL) {
    irr::core::list<irr::scene::ISceneNode *> lChildren=pCheckRoot->getChildren();
    irr::core::list<irr::scene::ISceneNode *>::Iterator it;

    for (it=lChildren.begin(); it!=lChildren.end(); it++) m_aCheckPoints.push_back(*it);
    printf("%i checkpoints for plane found!\n",m_aCheckPoints.size());
  }
  else printf("no checkpoints for plane found!\n");

  for (irr::u32 i=0; i<2; i++) {
    irr::c8 s[0xFF];
    sprintf(s,"axis%i",i+1);
    m_pAxes[i]=(irr::ode::CIrrOdeJointHinge *)m_pBody->getChildByName(s,m_pBody);
    printf("%s: %i\n",s,(int)m_pAxes[i]);
    m_fAngleRate[i]=0.0f;
  }
  m_fAngleRate[2]=0.0f;

  m_pSteerAxis=(irr::ode::CIrrOdeJointHinge2 *)m_pBody->getChildByName("axisSteer",m_pBody);
  printf("steer axis: %i\n",(int)m_pSteerAxis);

  m_pLap = new CIrrOdeCarTrack(m_pBody);

  m_bAutoPilot = false;
  dataChanged();
}

CVehicle::CPlane::~CPlane() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->removeEventListener(this);
}

void CVehicle::CPlane::odeStep(irr::u32 iStep) {
  if (m_bFirePrimary) {
    m_iLastShot1=iStep;
    //We add a new bomb...
    irr::core::vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(m_bLeftMissile?4.0f:-4.0f,-1.0f,-2.0f)),
              rot=m_pBody->getRotation(),vel=m_pBody->getLinearVelocity();

    m_bLeftMissile=!m_bLeftMissile;

    CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true,this);
    p->setTarget(m_pTargetSelector->getTarget());
    incShotsFired();
    m_bFirePrimary=false;
  }

  if (m_bFireSecondary) {
    m_iLastShot2=iStep;
    //We add a new bullet...
    irr::core::vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(0.2f,1.5f,-11.0f)),
              rot=m_pBody->getRotation(),
              vel=m_pBody->getLinearVelocity().getLength()*m_pBody->getRotation().rotationToDirection(irr::core::vector3df(0.0f,0.0f,1.0f))+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(0.0f,0.0f,-350.0f));

    new CProjectile(m_pSmgr,pos,rot,vel,"bullet",600,m_pWorld,true,this);
    incShotsFired();
    m_bFireSecondary=false;
  }

  if (m_bDataChanged) {
    dataChanged();
    m_bDataChanged=false;
  }

  for (irr::u32 i=0; i<2; i++) {
    if (m_pAxes[i]) {
      irr::f32 f=m_pAxes[i]->getHingeAngleRate(),fImpulse=m_fAngleRate[i]-f;
      if (fImpulse<0.0f) fImpulse=-fImpulse;
      if (fImpulse>15.0f) {
        irr::f32 fVol=(fImpulse-15.0f)/75.0f;
        if (fVol>1.0f) fVol=1.0f;
        irr::core::vector3df irrPos=m_pAxes[i]->getAbsolutePosition();
        CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndSkid,fVol,irrPos);
        irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
      }
      m_fAngleRate[i]=f;
    }
  }

  if (m_pSteerAxis) {
    irr::f32 f=m_pSteerAxis->getHingeAngle2Rate(),fImpulse=m_fAngleRate[2]-f;;
    if (fImpulse<0.0f) fImpulse=-fImpulse;
    if (fImpulse>15.0f) {
      irr::f32 fVol=(fImpulse-15.0f)/75.0f;
      if (fVol>1.0f) fVol=1.0f;
      irr::core::vector3df irrPos=m_pSteerAxis->getAbsolutePosition();
      CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndSkid,fVol,irrPos);
      irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
    }
    m_fAngleRate[2]=f;
  }

  if (m_bAutoPilot != m_pAutoPilot->isEnabled()) {
    m_bAutoPilot = m_pAutoPilot->isEnabled();
    dataChanged();
  }
}

void CVehicle::CPlane::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

irr::ode::IIrrOdeEvent *CVehicle::CPlane::writeEvent() {
  CEventPlaneState *p=new CEventPlaneState(m_pBody->getID(),m_fYaw,m_fPitch,m_fRoll,m_pMotor->getPower(),m_pBrakes[0]->getForce()>20.0f,m_pAutoPilot->isEnabled(),m_fThrust);
  return p;
}

irr::ode::eEventWriterType CVehicle::CPlane::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}


//Implementation of CHeli
CVehicle::CHeli::CHeli(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode) : CAeroVehicle(pDevice, pNode) {
  m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor,m_pRay);

  m_pTargetSelector=new CTargetSelector(m_pBody,m_pDevice,m_pAero->getForeward());

  m_bLeft=false;
  m_fSound=0.0f;
  m_iNodeId=pNode->getID();

  CCustomEventReceiver::getSharedInstance()->addHeli(pNode);

  dataChanged();
}

CVehicle::CHeli::~CHeli() {
}

void CVehicle::CHeli::odeStep(irr::u32 iStep) {
  irr::core::vector3df rot=m_pBody->getRotation();

  if (m_bFirePrimary) {
    m_bFirePrimary=false;
    m_iLastShot1=iStep;
    //We add a new missile...
    irr::core::vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(m_bLeft?-3.0f:3.0f,-0.6f,-2.0f)),
                         rot=m_pBody->getAbsoluteTransformation().getRotationDegrees(),
                         vel=m_pBody->getLinearVelocity();

    CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true,this);
    p->setTarget(m_pTargetSelector->getTarget());
    m_bLeft=!m_bLeft;
    incShotsFired();
  }

  m_fSound=0.75f+0.5*m_pMotor->getPower();
  if (m_bDataChanged) {
    dataChanged();
    m_bDataChanged=false;
  }
}

void CVehicle::CHeli::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

irr::ode::IIrrOdeEvent *CVehicle::CHeli::writeEvent() {
  return new CEventHeliState(m_iNodeId,m_fSound,m_pAutoPilot->isEnabled(),m_fThrust);
}

irr::ode::eEventWriterType CVehicle::CHeli::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

//Implementation of CAeroVehicle
CVehicle::CAeroVehicle::CAeroVehicle(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode) : CIrrOdeCarState(pDevice,L"Helicopter") {
  irr::ode::IIrrOdeEventWriter::setWorld(reinterpret_cast<irr::ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode")));
  m_pBody=reinterpret_cast<irr::ode::CIrrOdeBody *>(pNode);

  if (m_pBody!=NULL) {
    m_pBody->setUserData(this);
    m_pTerrain=reinterpret_cast<irr::scene::ITerrainSceneNode *>(m_pSmgr->getSceneNodeFromName("terrain"));

    m_iShotsFired=0;
    m_fCamAngleH=0.0f;
    m_fCamAngleV=0.0f;

    m_fPitch  = 0.0f;
    m_fYaw    = 0.0f;
    m_fRoll   = 0.0f;
    m_fThrust = 0.0f;

    m_bFirePrimary   = false;
    m_bFireSecondary = false;
    m_bDataChanged   = false;
    m_bBrakes        = false;
    m_bFlip          = false;

    irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->addEventListener(this);

    m_pMotor =(irr::ode::CIrrOdeImpulseMotor *)m_pBody->getStepMotorFromName("aero_motor" );
    m_pTorque=(irr::ode::CIrrOdeTorqueMotor  *)m_pBody->getStepMotorFromName("aero_torque");
    m_pAero  =(irr::ode::CIrrOdeAeroDrag     *)m_pBody->getStepMotorFromName("aero_aero"  );

    m_pRay=(irr::ode::CIrrOdeGeomRay *)m_pBody->getGeomFromName("aero_ray");

    for (irr::u32 i=0; i<2; i++) {
      irr::c8 s[0xFF];
      sprintf(s,"brake%i",i);
      m_pBrakes[i]=dynamic_cast<irr::ode::CIrrOdeMotor *>(m_pBody->getMotorFromName(s));
    }

    m_pSteer=(irr::ode::CIrrOdeServo *)m_pBody->getMotorFromName("plane_wheel_steer");

    printf("\naero state:\n\n");
    printf("motors: %i, %i, %i, ray: %i\n\n",(int)m_pMotor,(int)m_pTorque,(int)m_pAero,(int)m_pRay);
    printf("brakes: %i, %i\n",(int)m_pBrakes[0],(int)m_pBrakes[1]);
    printf("steer: %i\n",(int)m_pSteer);

    m_fThrust=0;
    m_fPitch=0;
    m_fRoll=0;
    m_fYaw=0;
    m_bInitialized=true;

    m_iNextCp=-1;
    irr::scene::ISceneNode *pCheckRoot=m_pSmgr->getSceneNodeFromName("planepoints");
    if (pCheckRoot!=NULL) {
      irr::core::list<irr::scene::ISceneNode *> lChildren=pCheckRoot->getChildren();
      irr::core::list<irr::scene::ISceneNode *>::Iterator it;

      for (it=lChildren.begin(); it!=lChildren.end(); it++) m_aCheckPoints.push_back(*it);
      printf("%i checkpoints for plane found!\n",m_aCheckPoints.size());
    }
    else printf("no checkpoints for helicopter found!\n");
  }

  for (irr::u32 i=0; i<0xFF; i++) m_aCtrlBuffer[i]=0.0f;
}

CVehicle::CAeroVehicle::~CAeroVehicle() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->removeEventListener(this);
}

void CVehicle::CAeroVehicle::activate() {
}

void CVehicle::CAeroVehicle::deactivate() {
}

bool CVehicle::CAeroVehicle::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == eCtrlMsgPlane) {
    CPlaneControls *p = reinterpret_cast<CPlaneControls *>(pEvent);

    if (p->getNode() == m_pBody->getID() && p->getClient() == m_iControllerBy) {
      irr::f32 f;

      f = p->getPower(); if (f != m_fThrust) { m_fThrust = f; m_bDataChanged = true; }
      f = p->getPitch(); if (f != m_fPitch ) { m_fPitch  = f; m_bDataChanged = true; }
      f = p->getRoll (); if (f != m_fRoll  ) { m_fRoll   = f; m_bDataChanged = true; }
      f = p->getYaw  (); if (f != m_fYaw   ) { m_fYaw    = f; m_bDataChanged = true; }

      if (!m_bFirePrimary  ) m_bFirePrimary   = p->getFirePrimary  ();
      if (!m_bFireSecondary) m_bFireSecondary = p->getFireSecondary();

      if (p->getPowerZero()) m_fThrust = 0.0f;
      if (p->getAutoPilot()) m_pAutoPilot->setEnabled(!m_pAutoPilot->isEnabled());

      m_bFlip    = p->getFlip ();
      m_bBrakes  = p->getBrake();

      if (p->getSelectTarget()) m_pTargetSelector->selectOption();

      m_bDataChanged = true;
    }
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    if (m_pAutoPilot->isEnabled()) m_bDataChanged = true;
    irr::ode::CIrrOdeEventStep *pStep=(irr::ode::CIrrOdeEventStep *)pEvent;
    irr::core::vector3df vPos=m_pBody->getAbsolutePosition();
    irr::f32 fMinHeight=50,
        fMaxHeight=3000,
        fAltFact=vPos.Y<fMinHeight?1.0f:vPos.Y>fMaxHeight?0.0f:1-((vPos.Y-fMinHeight)/(fMaxHeight-fMinHeight)),
        fVel=m_pAero->getForewardVel(),
        fVelFact=fVel<100.0f?1.0f:fVel>180.0f?0.2f:1.0f-((fVel-100.0f)/100.0f);

    if (m_fThrust> 1.0f) { m_bDataChanged=true; m_fThrust =1.0f; }
    if (m_fThrust<-0.0f) { m_bDataChanged=true; m_fThrust/=4.0f; }

    if (m_pBrakes[0]!=NULL) {
      m_pBrakes[0]->setForce(m_bBrakes ? 50.0f : 0.0f);
    }

    if (m_pBrakes[1]!=NULL) {
      m_pBrakes[1]->setForce(m_bBrakes ? 50.0f : 0.0f);
    }

    if (m_bFlip) {
      m_pBody->addForceAtPosition(m_pBody->getPosition()+irr::core::vector3df(0.0f,1.5f,0.0f),irr::core::vector3df(0.0f,15.0f,0.0f));
    }

    if (m_pAutoPilot->isEnabled()) {
      if (m_iNextCp==-1) {
        m_iNextCp=rand()%m_aCheckPoints.size();
        m_pAutoPilot->setTarget(m_aCheckPoints[m_iNextCp]);
      }

      if (m_pAutoPilot->getApDist() < 150.0f) {
        m_iNextCp = rand()%m_aCheckPoints.size();
        m_pAutoPilot->setTarget(m_aCheckPoints[m_iNextCp]);
      }
    }

    m_pAutoPilot->step(m_fYaw,m_fPitch,m_fRoll,m_fThrust);
    m_pTargetSelector->update();

    m_pTorque->setPower(fVelFact);
    m_pAero  ->setPower(fAltFact);

    irr::f32 fPitch,fRoll,fYaw;

    if (m_pAutoPilot->isEnabled()) {
      fPitch=m_fPitch;
      fRoll =m_fRoll ;
      fYaw  =m_fYaw  ;
    }
    else {
      fPitch=m_fPitch>0.0f?m_fPitch*m_fPitch:-m_fPitch*m_fPitch;
      fRoll =m_fRoll >0.0f?m_fRoll *m_fRoll :-m_fRoll *m_fRoll ;
      fYaw  =m_fYaw  >0.0f?m_fYaw  *m_fYaw  :-m_fYaw  *m_fYaw  ;
    }

    if (m_fPitch==m_fPitch) { m_fPitch = fPitch; m_pTorque->setPitch(fPitch); }
    if (m_fRoll ==m_fRoll ) { m_fRoll  = fRoll ; m_pTorque->setRoll (fRoll ); }
    if (m_fYaw  ==m_fYaw  ) {
      m_pTorque->setYaw(fYaw);
      m_fYaw = fYaw;

      if (m_pSteer!=NULL) m_pSteer->setServoPos(-20.0f*m_fYaw);
    }

    m_pMotor->setPower(m_fThrust*fAltFact);

    odeStep(pStep->getStepNo());
  }

  return false;
}

bool CVehicle::CAeroVehicle::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep || pEvent->getType() == eCtrlMsgPlane;
}

void CVehicle::CAeroVehicle::incHitsScored() {
  CIrrOdeCarState::incHitsScored();
  postShotEvent();
}

void CVehicle::CAeroVehicle::incHitsTaken() {
  CIrrOdeCarState::incHitsTaken();
  postShotEvent();
}

void CVehicle::CAeroVehicle::incShotsFired() {
  m_iShotsFired++;
  postShotEvent();
}

void CVehicle::CAeroVehicle::postShotEvent() {
  CEventShots *p = new CEventShots(m_pBody->getID(), m_iShotsFired, m_iHitsTaken, m_iHitsScored);
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
}

//Implementation of CTank
irr::ode::CIrrOdeBody *getChildBodyFromName(irr::ode::CIrrOdeBody *pBody, const irr::c8 *sName) {
  printf("%s .. %s\n",pBody->getName(),sName);

  irr::core::list<irr::ode::CIrrOdeBody *> children=pBody->getChildBodies();
  irr::core::list<irr::ode::CIrrOdeBody *>::Iterator it;
  printf("children: %i\n",children.size());

  for (it=children.begin(); it!=children.end(); it++) {
    irr::ode::CIrrOdeBody *pBody=NULL;
    pBody=getChildBodyFromName(*it,sName);
    if (pBody!=NULL) return pBody;
  }

  return NULL;
}

CVehicle::CTank::CTank(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode) : CIrrOdeCarState(pDevice,L"Tank") {
  irr::ode::IIrrOdeEventWriter::setWorld(reinterpret_cast<irr::ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode")));
  m_pTankBody=reinterpret_cast<irr::ode::CIrrOdeBody *>(pNode);
  m_iLastShot=0;
  m_fSound=0.5f;

  if (m_pTankBody!=NULL) {
    m_pTankBody->setUserData(this);
    CCustomEventReceiver::getSharedInstance()->addTank(m_pTankBody);

    m_pTurret=m_pTankBody->getChildBodyFromName("turret");

    m_pMotor[0]=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("motorRR");
    m_pMotor[1]=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("motorFR");
    m_pMotor[2]=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("motorRL");
    m_pMotor[3]=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("motorFL");

    m_pTurretMotor=(irr::ode::CIrrOdeMotor *)m_pTankBody->getMotorFromName("turretMotor");
    m_pCannonServo=(irr::ode::CIrrOdeServo *)m_pTankBody->getMotorFromName("cannonServo");

    printf("\t\tturret motor: %i\n\t\tcannon servo: %i\n",(int)m_pTurretMotor,(int)m_pCannonServo);

    irr::ode::CIrrOdeJointHinge *pAxis=NULL;

    pAxis=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("tankAxisRR"); if (pAxis!=NULL) m_lAxes.push_back(pAxis);
    pAxis=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("tankAxisFR"); if (pAxis!=NULL) m_lAxes.push_back(pAxis);
    pAxis=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("tankAxisRL"); if (pAxis!=NULL) m_lAxes.push_back(pAxis);
    pAxis=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("tankAxisFL"); if (pAxis!=NULL) m_lAxes.push_back(pAxis);

    m_pCannonHinge=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("cannonAxis");
    m_pTurretHinge=(irr::ode::CIrrOdeJointHinge *)m_pTankBody->getJointFromName("turretAxis");

    m_pCannon=m_pTankBody->getChildBodyFromName("cannon");

    printf("\ntank state:\n\n");
    printf("axes: ");
    irr::core::list<irr::ode::CIrrOdeJointHinge *>::Iterator it;
    for (it=m_lAxes.begin(); it!=m_lAxes.end(); it++) printf("%i  ",(int)(*it));
    printf("\n");
    printf("motors: ");
    for (irr::u32 i=0; i<4; i++) printf("%i  ",(int)m_pMotor[i]);
    printf("\n");

    printf("turret: %i -- axis=%i, motor=%i\n",(int)m_pTurret,(int)m_pTurretHinge,(int)m_pTurretMotor);
    printf("cannon: %i -- axis=%i, motor=%i\n",(int)m_pCannon,(int)m_pCannonHinge,(int)m_pCannonServo);

    m_bFlip         =false;
    m_bFire         =false;
    m_bFastCollision=true;

    m_fCannonAngle=0.0f;
    m_fTurretAngle=0.0f;
    m_fCannonUp   =0.0f;
    m_fCannonLeft =0.0f;

    m_pTextures[0]=m_pDevice->getVideoDriver()->getTexture("../../data/target.jpg");
    m_pTextures[1]=m_pDevice->getVideoDriver()->getTexture("../../data/target1.jpg");

    irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->addEventListener(this);
    m_bInitialized=true;
  }
}

CVehicle::CTank::~CTank() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->removeEventListener(this);
}

void CVehicle::CTank::activate() {
  irr::core::vector3df rot=!m_bFollowTurret?m_pTankBody->getRotation():m_pTurret->getRotation();

  irr::core::vector3df pos=rot.rotationToDirection(irr::core::vector3df(7.5,2.5,0)),
                           up =m_pTankBody->getRotation().rotationToDirection(irr::core::vector3df(0,0.1,0)),
                           tgt=m_pTankBody->getRotation().rotationToDirection(irr::core::vector3df(0,1  ,0));

}

void CVehicle::CTank::deactivate() {
  for (int i=0; i<4; i++) {
    m_pMotor[i]->setVelocity(0);
    m_pMotor[i]->setForce(13);
  }
}

irr::u32 CVehicle::CTank::update() {
  irr::u32 iRet=CIrrOdeCarState::update();
  static irr::u32 iLastTime=0,iStep=0;

  if (!iLastTime)
    iLastTime=m_pDevice->getTimer()->getTime();
  else {
    irr::u32 iTime=m_pDevice->getTimer()->getTime();
    iStep+=iTime-iLastTime;
    iLastTime=iTime;
  }

  irr::core::vector3df rot=!m_bFollowTurret?m_pTankBody->getRotation():m_pTurret->getAbsoluteTransformation().getRotationDegrees();

  irr::core::vector3df pos=rot.rotationToDirection(irr::core::vector3df(7.5,2.5,0)),
                       up =m_pTankBody->getRotation().rotationToDirection(irr::core::vector3df(0,0.1,0)),
                       tgt=m_pTankBody->getRotation().rotationToDirection(irr::core::vector3df(0,1  ,0));

  return iRet;
}

bool CVehicle::CTank::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == eCtrlMsgTank) {
    CTankControls *p = reinterpret_cast<CTankControls *>(pEvent);
    if (p->getNode() == m_pTankBody->getID()) {
      m_fCannonLeft = p->getCannonLeft();
      m_fCannonUp   = p->getCannonUp  ();
      m_fThrottle   = p->getThrottle  ();
      m_fSteer      = p->getSteer     ();
      m_bFlip       = p->getFlip      ();

      if (p->getFire         ()) m_bFire          = true;
      if (p->getFastCollision()) m_bFastCollision = !m_bFastCollision;
    }
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    irr::ode::CIrrOdeEventStep *pStep=(irr::ode::CIrrOdeEventStep *)pEvent;
    if (m_bFire && pStep->getStepNo()-m_iLastShot>45) {
      m_bFire = false;
      m_iLastShot=pStep->getStepNo();
      //We add a new bullet...
      irr::core::vector3df rot=m_pCannon->getAbsoluteTransformation().getRotationDegrees(),
                           pos=m_pCannon->getAbsolutePosition()+rot.rotationToDirection(irr::core::vector3df(-3.0f,0,0)),
                           vel=m_pTankBody->getLinearVelocity()+rot.rotationToDirection(irr::core::vector3df(-350.0f,0.0f,0.0f));

      new CProjectile(m_pSmgr,pos,rot,vel,"shell",600,m_pWorld,m_bFastCollision,this);
    }

    irr::f32 fVel[4]={ 0.0f, 0.0f, 0.0f, 0.0f };

    if (m_fThrottle!=0.0f) {
      for (irr::u32 i=0; i<4; i++) fVel[i]=-75.0f * m_fThrottle;
    }
    else {
      for (irr::u32 i=0; i<4; i++) fVel[i]=0.0f;
    }

    if (m_fSteer!=0.0f) {
      fVel[0]-=m_fSteer*45.0f;
      fVel[1]-=m_fSteer*45.0f;
      fVel[2]+=m_fSteer*45.0f;
      fVel[3]+=m_fSteer*45.0f;
    }

    irr::f32 cPos = m_pCannonServo->getServoPos();

    cPos+=0.5f*m_fCannonUp;

    if (cPos< -10) cPos= -10;
    if (cPos>  80) cPos=  80;

    m_pCannonServo->setServoPos(cPos);

    m_pTurretMotor->setVelocity(0.5f * m_fCannonLeft);

    for (irr::u32 i=0; i<4; i++) {
      m_pMotor[i]->setVelocity(fVel[i]);
      m_pMotor[i]->setForce(20);
    }

    if (m_bFlip) {
      m_pTankBody->addForceAtPosition(m_pTankBody->getPosition()+irr::core::vector3df(0.0f,0.2f,0.0f),irr::core::vector3df(0.0f,250.0f,0.0f));
    }

    irr::core::list<irr::ode::CIrrOdeJointHinge *>::Iterator it;
    bool bMoved=false;
    irr::u32 i=0;
    for (it=m_lAxes.begin(); it!=m_lAxes.end() && i<4; it++) {
      irr::ode::CIrrOdeJointHinge *p=*it;
      irr::s8 iAngle=(irr::s8)(p->getHingeAngle()*60/M_PI);
      if (m_aAxesAngles[i]!=iAngle) {
        bMoved=true;
        m_aAxesAngles[i]=iAngle;
      }
      i++;
    }

    if (m_fCannonAngle!=m_pCannonHinge->getHingeAngle()) {
      bMoved=true;
      m_fCannonAngle=m_pCannonHinge->getHingeAngle();
    }

    if (m_fTurretAngle!=m_pTurretHinge->getHingeAngle()) {
      bMoved=true;
      m_fTurretAngle=m_pTurretHinge->getHingeAngle();
    }

    if (bMoved) dataChanged();

    irr::f32 fRot=0.0f,fSound=0.5f;

    irr::core::list<irr::ode::CIrrOdeJointHinge *>::Iterator hit;

    for (hit=m_lAxes.begin(); hit!=m_lAxes.end(); hit++) {
      irr::ode::CIrrOdeJointHinge *p=*hit;
      if (p->getHingeAngleRate()<0) fRot-=p->getHingeAngleRate(); else fRot+=p->getHingeAngleRate();
    }

    fRot/=4.0f;

    if (fRot>1.0f) {
      if (fRot<35.0f) {
        fSound=0.5f+(fRot-1.0f)/34.0f;
      }
      else {
        fSound=1.5f;
      }

      if (m_fSound<fSound) {
        m_fSound+=0.01f;
        if (m_fSound>fSound) m_fSound=fSound;
      }

      if (m_fSound>fSound) {
        m_fSound-=0.01f;
        if (m_fSound<fSound) m_fSound=fSound;
      }
    }
  }
  return false;
}

bool CVehicle::CTank::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep || pEvent->getType() == eCtrlMsgTank;
}

irr::ode::IIrrOdeEvent *CVehicle::CTank::writeEvent() {
  CEventTankState *p=new CEventTankState(m_pTankBody->getID(),m_aAxesAngles,m_fCannonAngle,m_fTurretAngle,m_fSound);
  return p;
}

irr::ode::eEventWriterType CVehicle::CTank::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

irr::ode::CIrrOdeBody *CVehicle::CTank::getBody() {
  return m_pTankBody;
}

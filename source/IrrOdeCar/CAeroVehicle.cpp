  #include "CAeroVehicle.h"
  #include <CProjectile.h>
  #include <CAutoPilot.h>
  #include <irrCC.h>
  #include <CCockpitPlane.h>
  #include <CTargetSelector.h>
  #include <CRearView.h>
  #include <irrklang.h>
  #include <CEventVehicleState.h>
  #include <CControlEvents.h>

CAeroVehicle::CAeroVehicle(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CRearView *pRView, irr::ode::IIrrOdeEventQueue *pInputQueue) : CIrrOdeCarState(pDevice,L"Helicopter", pInputQueue) {
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

    irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);

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
    m_pCockpit=NULL;
    m_pRView=pRView;
  }

  for (irr::u32 i=0; i<0xFF; i++) m_aCtrlBuffer[i]=0.0f;
}

CAeroVehicle::~CAeroVehicle() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CAeroVehicle::activate() {
  if (m_pCockpit) m_pCockpit->setActive(true);
  if (m_pRView  ) m_pRView  ->setActive(true);
}

void CAeroVehicle::deactivate() {
  if (m_pCockpit) m_pCockpit->setActive(false);
  if (m_pRView  ) m_pRView  ->setActive(false);
}

bool CAeroVehicle::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
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

    if (m_fPitch==m_fPitch) m_pTorque->setPitch(fPitch);
    if (m_fRoll ==m_fRoll ) m_pTorque->setRoll (fRoll );
    if (m_fYaw  ==m_fYaw  ) {
      m_pTorque->setYaw(fYaw);

      if (m_pSteer!=NULL) m_pSteer->setServoPos(-20.0f*m_fYaw);
    }

    m_pMotor->setPower(m_fThrust*fAltFact);

    odeStep(pStep->getStepNo());
  }

  return false;
}

bool CAeroVehicle::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep || pEvent->getType() == eCtrlMsgPlane;
}

void CAeroVehicle::incHitsScored() {
  CIrrOdeCarState::incHitsScored();
  postShotEvent();
}

void CAeroVehicle::incHitsTaken() {
  CIrrOdeCarState::incHitsTaken();
  postShotEvent();
}

void CAeroVehicle::incShotsFired() {
  m_iShotsFired++;
  postShotEvent();
}

void CAeroVehicle::postShotEvent() {
  CEventShots *p = new CEventShots(m_pBody->getID(), m_iShotsFired, m_iHitsTaken, m_iHitsScored);
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
}

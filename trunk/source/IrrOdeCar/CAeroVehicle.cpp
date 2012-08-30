  #include "CAeroVehicle.h"
  #include <CProjectile.h>
  #include <CAutoPilot.h>
  #include <irrCC.h>
  #include <CCockpitPlane.h>
  #include <CTargetSelector.h>
  #include <CRearView.h>
  #include <irrklang.h>
  #include <CEventVehicleState.h>

CAeroVehicle::CAeroVehicle(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CIrrCC *pCtrl, CRearView *pRView) : CIrrOdeCarState(pDevice,L"Helicopter","../../data/irrOdeHeliHelp.txt",pCtrl) {
  irr::ode::IIrrOdeEventWriter::setWorld(reinterpret_cast<irr::ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode")));
  m_pBody=reinterpret_cast<irr::ode::CIrrOdeBody *>(pNode);

  if (m_pBody!=NULL) {
    m_pBody->setUserData(this);
    m_pTerrain=reinterpret_cast<irr::scene::ITerrainSceneNode *>(m_pSmgr->getSceneNodeFromName("terrain"));

    m_iShotsFired=0;
    m_fCamAngleH=0.0f;
    m_fCamAngleV=0.0f;
    m_bFirePrimary=false;
    m_bFireSecondary=false;
    m_bDataChanged=false;
    m_bBrakes = false;

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
  m_bSwitchToMenu=false;
  m_bActive=true;

  loadHelpFile();
  wchar_t s[1024];
  swprintf(s,1023,m_pHelp->getText(),m_pController->getSettingsText(2));
  m_pHelp->setText(s);
  m_pController->restoreState((irr::f32 *)m_aCtrlBuffer);

  if (m_pCockpit) m_pCockpit->setActive(true);
  if (m_pRView  ) m_pRView  ->setActive(true);
}

void CAeroVehicle::deactivate() {
  m_pController->dumpState((irr::f32 *)m_aCtrlBuffer);
  m_bActive=false;

  if (m_pCockpit) m_pCockpit->setActive(false);
  if (m_pRView  ) m_pRView  ->setActive(false);
}

bool CAeroVehicle::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    irr::ode::CIrrOdeEventStep *pStep=(irr::ode::CIrrOdeEventStep *)pEvent;
    irr::core::vector3df vPos=m_pBody->getAbsolutePosition();
    irr::f32 fMinHeight=50,
        fMaxHeight=3000,
        fAltFact=vPos.Y<fMinHeight?1.0f:vPos.Y>fMaxHeight?0.0f:1-((vPos.Y-fMinHeight)/(fMaxHeight-fMinHeight)),
        fVel=m_pAero->getForewardVel(),
        fVelFact=fVel<100.0f?1.0f:fVel>180.0f?0.2f:1.0f-((fVel-100.0f)/100.0f);

    if (m_bActive) {
      irr::f32 fThrust=m_pController->get(m_pCtrls[eAeroPowerUp]);
      if (fThrust<m_fThrust-0.001 || fThrust>m_fThrust+0.001) {
        m_bDataChanged=true;
        m_fThrust=fThrust;
      }

      if (m_pController->get(m_pCtrls[eAeroPowerZero])) {
        m_fThrust=0.0f;
        m_pController->set(m_pCtrls[eAeroPowerUp],0.0f);
        m_bDataChanged=true;
      }

      irr::f32 f;

      f=m_pController->get(m_pCtrls[eAeroPitchUp ]); if (f!=m_fPitch) { m_bDataChanged=true; m_fPitch=f; }
      f=m_pController->get(m_pCtrls[eAeroRollLeft]); if (f!=m_fRoll ) { m_bDataChanged=true; m_fRoll =f; }
      f=m_pController->get(m_pCtrls[eAeroYawRight]); if (f!=m_fYaw  ) { m_bDataChanged=true; m_fYaw  =f; }

      if (m_fThrust> 1.0f) { m_bDataChanged=true; m_fThrust =1.0f; }
      if (m_fThrust<-0.0f) { m_bDataChanged=true; m_fThrust/=4.0f; }

      if (m_pController->get(m_pCtrls[eAeroFirePrimary])!=0.0f && pStep->getStepNo()-m_iLastShot1>60) {
        m_bFirePrimary=true;
      }

      if (m_pController->get(m_pCtrls[eAeroFireSecondary])!=0.0f && pStep->getStepNo()-m_iLastShot2>30) {
        m_bFireSecondary=true;
      }

      bool b = m_pController->get(m_pCtrls[eAeroBrake])>0.2f;
      m_bDataChanged |= m_bBrakes != b;
      m_bBrakes = b;

      if (m_pBrakes[0]!=NULL) {
        m_pBrakes[0]->setForce(50.0f*m_pController->get(m_pCtrls[eAeroBrake]));
      }

      if (m_pBrakes[1]!=NULL) {
        m_pBrakes[1]->setForce(50.0f*m_pController->get(m_pCtrls[eAeroBrake]));
      }

      if (m_pController->get(m_pCtrls[eAeroFlip])) {
        m_pBody->addForceAtPosition(m_pBody->getPosition()+irr::core::vector3df(0.0f,1.5f,0.0f),irr::core::vector3df(0.0f,15.0f,0.0f));
      }

      if (m_pController->get(m_pCtrls[eAeroAutoPilot])) {
        m_pController->set(m_pCtrls[eAeroAutoPilot],0.0f);
        m_pAutoPilot->setEnabled(!m_pAutoPilot->isEnabled());
        m_bDataChanged = true;
      }

      if (m_pController->get(m_pCtrls[eAeroSelectTarget])) {
        m_pController->set(m_pCtrls[eAeroSelectTarget],0.0f);
        m_pTargetSelector->selectOption();
      }
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
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
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

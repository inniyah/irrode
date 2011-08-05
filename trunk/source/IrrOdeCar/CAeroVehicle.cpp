  #include "CAeroVehicle.h"
  #include <CProjectile.h>
  #include <CAutoPilot.h>
  #include <irrCC.h>
  #include <CCockpitPlane.h>
  #include <CTargetSelector.h>

CAeroVehicle::CAeroVehicle(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit) : CIrrOdeCarState(pDevice,L"Helicopter","../../data/irrOdeHeliHelp.txt",pCtrl) {
  m_pWorld=reinterpret_cast<ode::CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode"));
  m_pBody=reinterpret_cast<ode::CIrrOdeBody *>(pNode);
  if (m_pBody!=NULL) {
    m_pTerrain=reinterpret_cast<ITerrainSceneNode *>(m_pSmgr->getSceneNodeFromName("terrain"));

    m_pCam=m_pSmgr->addCameraSceneNode();
    m_pCam->setNearValue(0.1f);
    m_bWeaponCam=false;
    m_bInternal=false;
    m_iShotsFired=0;
    m_fCamAngleH=0.0f;
    m_fCamAngleV=0.0f;
    m_bFirePrimary=false;
    m_bFireSecondary=false;
    m_bRudderChanged=false;

    ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);

    m_pTab=m_pGuiEnv->addTab(core::rect<s32>(0,0,500,500));
    m_pTab->setVisible(false);
    m_pInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,5,150,85),true,true,m_pTab);
    m_pInfo->setDrawBackground(true);
    m_pInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));

    m_pApInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,95,150,355),true,true,m_pTab);
    m_pApInfo->setDrawBackground(true);
    m_pApInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
    m_pApInfo->setVisible(false);

    m_pMotor =(irr::ode::CIrrOdeImpulseMotor *)m_pBody->getStepMotorFromName("aero_motor" );
    m_pTorque=(irr::ode::CIrrOdeTorqueMotor  *)m_pBody->getStepMotorFromName("aero_torque");
    m_pAero  =(irr::ode::CIrrOdeAeroDrag     *)m_pBody->getStepMotorFromName("aero_aero"  );

    m_pRay=(irr::ode::CIrrOdeGeomRay *)m_pBody->getGeomFromName("aero_ray");

    for (u32 i=0; i<2; i++) {
      c8 s[0xFF];
      sprintf(s,"brake%i",i);
      m_pBrakes[i]=dynamic_cast<irr::ode::CIrrOdeMotor *>(m_pBody->getMotorFromName(s));
    }

    m_pSteer=(ode::CIrrOdeServo *)m_pBody->getMotorFromName("plane_wheel_steer");

    m_bThreeWheeler=!strcmp(m_pBody->getName(),"plane2") || !strcmp(m_pBody->getName(),"plane4");

    printf("\naero state:\n\n");
    printf("motors: %i, %i, %i, ray: %i\n\n",(int)m_pMotor,(int)m_pTorque,(int)m_pAero,(int)m_pRay);
    printf("brakes: %i, %i\n",(int)m_pBrakes[0],(int)m_pBrakes[1]);
    printf("steer: %i\n",(int)m_pSteer);
    printf("3-wheeler: %s\n",m_bThreeWheeler?"yes":"no");

    m_fThrust=0;
    m_fPitch=0;
    m_fRoll=0;
    m_fYaw=0;
    m_bInitialized=true;

    m_iNextCp=-1;
    ISceneNode *pCheckRoot=m_pSmgr->getSceneNodeFromName("planepoints");
    if (pCheckRoot!=NULL) {
      irr::core::list<irr::scene::ISceneNode *> lChildren=pCheckRoot->getChildren();
      irr::core::list<irr::scene::ISceneNode *>::Iterator it;

      for (it=lChildren.begin(); it!=lChildren.end(); it++) m_aCheckPoints.push_back(*it);
      printf("%i checkpoints for plane found!\n",m_aCheckPoints.size());
    }
    else printf("no checkpoints for helicopter found!\n");
    m_fApDist=0.0f;
    m_pCockpit=pCockpit;
  }

  for (u32 i=0; i<0xFF; i++) m_aCtrlBuffer[i]=0.0f;
}

CAeroVehicle::~CAeroVehicle() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CAeroVehicle::activate() {
  m_pSmgr->setActiveCamera(m_pCam);
  m_pDevice->setEventReceiver(this);
  m_pDevice->getCursorControl()->setVisible(false);
  m_pTab->setVisible(true);
  m_pApInfo->setVisible(m_pAutoPilot->isEnabled());
  m_bSwitchToMenu=false;
  m_bActive=true;

  vector3df pos=m_pBody->getRotation().rotationToDirection(vector3df(0,5,15)),
            up =m_pBody->getRotation().rotationToDirection(vector3df(0,0.1,0)),
            tgt=m_pBody->getRotation().rotationToDirection(vector3df(0,1  ,0));

  m_pCam->setPosition(m_pBody->getPosition()+pos);
  m_pCam->setUpVector(up);
  m_pCam->setTarget(m_pBody->getPosition()+tgt);

  loadHelpFile();
  wchar_t s[1024];
  swprintf(s,1023,m_pHelp->getText(),m_pController->getSettingsText(2));
  m_pHelp->setText(s);
  m_pController->restoreState((f32 *)m_aCtrlBuffer);
}

void CAeroVehicle::deactivate() {
  m_pController->dumpState((f32 *)m_aCtrlBuffer);
  m_pTab->setVisible(false);
  m_pApInfo->setVisible(false);
  m_bActive=false;
}

bool CAeroVehicle::OnEvent(const SEvent &event) {
  bool bRet=m_pController->OnEvent(event);
  bRet|=CIrrOdeCarState::OnEvent(event);
  return bRet;
}

bool CAeroVehicle::onEvent(ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    irr::ode::CIrrOdeEventStep *pStep=(irr::ode::CIrrOdeEventStep *)pEvent;
    vector3df vPos=m_pBody->getAbsolutePosition();
    f32 fMinHeight=50,
        fMaxHeight=2000,
        fAltFact=vPos.Y<fMinHeight?1.0f:vPos.Y>fMaxHeight?0.0f:1-((vPos.Y-fMinHeight)/(fMaxHeight-fMinHeight)),
        fVel=m_pAero->getForewardVel(),
        fVelFact=fVel<100.0f?1.0f:fVel>180.0f?0.2f:1.0f-((fVel-100.0f)/100.0f);

    if (m_bActive) {
      m_fThrust=m_pController->get(m_pCtrls[eAeroPowerUp]);

      if (m_pController->get(m_pCtrls[eAeroPowerZero])) {
        m_fThrust=0.0f;
        m_pController->set(m_pCtrls[eAeroPowerUp],0.0f);
      }

      f32 f;

      f=m_pController->get(m_pCtrls[eAeroPitchUp ]); if (f!=m_fPitch) { m_bRudderChanged=true; m_fPitch=f; }
      f=m_pController->get(m_pCtrls[eAeroRollLeft]); if (f!=m_fRoll ) { m_bRudderChanged=true; m_fRoll =f; }
      f=m_pController->get(m_pCtrls[eAeroYawRight]); if (f!=m_fYaw  ) { m_bRudderChanged=true; m_fYaw  =f; }

      if (m_fThrust> 1.0f) m_fThrust =1.0f;
      if (m_fThrust<-0.0f) m_fThrust/=4.0f;

      if (m_pController->get(m_pCtrls[eAeroFirePrimary])!=0.0f && pStep->getStepNo()-m_iLastShot1>60) {
        m_bFirePrimary=true;
      }

      if (m_pController->get(m_pCtrls[eAeroFireSecondary])!=0.0f && pStep->getStepNo()-m_iLastShot2>30) {
        m_bFireSecondary=true;
      }

      if (m_pBrakes[0]!=NULL) {
        m_pBrakes[0]->setForce(100.0f*m_pController->get(m_pCtrls[eAeroBrake]));
      }

      if (m_pBrakes[1]!=NULL) {
        m_pBrakes[1]->setForce(100.0f*m_pController->get(m_pCtrls[eAeroBrake]));
      }

      if (m_pController->get(m_pCtrls[eAeroToggleCam])) {
        m_pController->set(m_pCtrls[eAeroToggleCam],0.0f);
        m_bWeaponCam=!m_bWeaponCam;
      }

      if (m_pController->get(m_pCtrls[eAeroInternalView])) {
        m_pController->set(m_pCtrls[eAeroInternalView],0.0f);
        m_bInternal=!m_bInternal;
      }

      if (m_pController->get(m_pCtrls[eAeroFlip])) {
        m_pBody->addForceAtPosition(m_pBody->getPosition()+vector3df(0.0f,1.5f,0.0f),vector3df(0.0f,15.0f,0.0f));
      }

      if (m_pController->get(m_pCtrls[eAeroAutoPilot])) {
        m_pController->set(m_pCtrls[eAeroAutoPilot],0.0f);
        m_pAutoPilot->setEnabled(!m_pAutoPilot->isEnabled());
        m_pApInfo->setVisible(m_pAutoPilot->isEnabled());
      }

      if (m_pController->get(m_pCtrls[eAeroSelectTarget])) {
        m_pController->set(m_pCtrls[eAeroSelectTarget],0.0f);
        m_pTargetSelector->selectOption();
      }

      if (m_pController->get(m_pCtrls[eAeroCamLeft])!=0.0f) {
        m_fCamAngleH+=m_pController->get(m_pCtrls[eAeroCamLeft]);

        if (m_fCamAngleH> 190.0f) m_fCamAngleH= 190.0f;
        if (m_fCamAngleH<-190.0f) m_fCamAngleH=-190.0f;
      }

      if (m_pController->get(m_pCtrls[eAeroCamUp])!=0.0f) {
        m_fCamAngleV+=m_pController->get(m_pCtrls[eAeroCamUp]);

        if (m_fCamAngleV> 60.0f) m_fCamAngleV= 60.0f;
        if (m_fCamAngleV<-60.0f) m_fCamAngleV=-60.0f;
      }

      if (m_pController->get(m_pCtrls[eAeroCamCenter])) {
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

    if (m_pAutoPilot->isEnabled()) {
      if (m_iNextCp==-1) {
        m_iNextCp=rand()%m_aCheckPoints.size();
        m_pAutoPilot->setTarget(m_aCheckPoints[m_iNextCp]);
      }
      m_fApDist=m_pAutoPilot->getApDist();
      if (m_fApDist<100.0f) {
        s32 iNext=m_iNextCp;
        while (iNext==m_iNextCp) {
          iNext=rand()%m_aCheckPoints.size();
          printf("next checkpoint (%s): %i\n",m_pBody->getName(),iNext);
        }
        m_iNextCp=iNext;
        m_pAutoPilot->setTarget(m_aCheckPoints[m_iNextCp]);
      }
    }

    m_pAutoPilot->step(m_fYaw,m_fPitch,m_fRoll,m_fThrust);
    m_pTargetSelector->update();

    m_pTorque->setPower(fAltFact*fVelFact);
    m_pAero  ->setPower(fAltFact);

    f32 fPitch,fRoll,fYaw;

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

      if (m_pSteer!=NULL) {
        if (m_bThreeWheeler)
          m_pSteer->setServoPos(-20.0f*m_fYaw);
        else
          m_pSteer->setServoPos(10.0f*m_fYaw);
      }
    }
    m_pMotor->setPower(m_fThrust*fAltFact);

    odeStep(pStep->getStepNo());
  }

  return false;
}

bool CAeroVehicle::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

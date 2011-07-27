  #include "CHeli.h"
  #include <CProjectile.h>
  #include <CAutoPilot.h>
  #include <irrCC.h>
  #include <CCockpitPlane.h>
  #include <CTargetSelector.h>

CHeli::CHeli(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit) : CIrrOdeCarState(pDevice,L"Helicopter","../../data/irrOdeHeliHelp.txt",pCtrl) {
  m_pWorld=reinterpret_cast<CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode"));
  m_pHeliBody=reinterpret_cast<CIrrOdeBody *>(pNode);
  if (m_pHeliBody!=NULL) {
    m_pTerrain=reinterpret_cast<ITerrainSceneNode *>(m_pSmgr->getSceneNodeFromName("terrain"));

    m_pCam=m_pSmgr->addCameraSceneNode();
    m_pCam->setNearValue(0.1f);
    m_bLeft=true;
    m_bMissileCam=false;
    m_bBackView=false;
    m_bInternal=false;
    m_iShotsFired=0;

    CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);

    m_pTab=m_pGuiEnv->addTab(core::rect<s32>(0,0,500,500));
    m_pTab->setVisible(false);
    m_pInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,5,150,85),true,true,m_pTab);
    m_pInfo->setDrawBackground(true);
    m_pInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));

    m_pApInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,95,150,355),true,true,m_pTab);
    m_pApInfo->setDrawBackground(true);
    m_pApInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
    m_pApInfo->setVisible(false);

    m_pMotor =(irr::ode::CIrrOdeImpulseMotor *)m_pHeliBody->getStepMotorFromName("heli_motor" );
    m_pTorque=(irr::ode::CIrrOdeTorqueMotor  *)m_pHeliBody->getStepMotorFromName("heli_torque");
    m_pAero  =(irr::ode::CIrrOdeAeroDrag     *)m_pHeliBody->getStepMotorFromName("heli_aero"  );

    m_pRay=(irr::ode::CIrrOdeGeomRay *)m_pHeliBody->getGeomFromName("heli_ray");

    printf("\nheli state:\n\n");
    printf("motors: %i, %i, %i, ray: %i\n\n",(int)m_pMotor,(int)m_pTorque,(int)m_pAero,(int)m_pRay);

    m_fThrust=0;
    m_fPitch=0;
    m_fRoll=0;
    m_fYaw=0;
    m_bInitialized=true;

    m_pAutoPilot=new CAutoPilot(m_pHeliBody,m_pAero,m_pTorque,m_pMotor,m_pRay);
    m_pAutoPilot->setState(CAutoPilot::eApHeliLowAlt);
    m_pAutoPilot->setAutoPilotInfo(m_pApInfo);

    m_pTargetSelector=new CTargetSelector(m_pHeliBody,m_pDevice,m_pAero->getForeward());

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

    c8 s[0xFF];
    sprintf(s,"heli_hi_%s",m_pHeliBody->getName());
    printf("name: %s\n",s);
    m_pCockpit=pCockpit;
  }
}

CHeli::~CHeli() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CHeli::activate() {
  m_pSmgr->setActiveCamera(m_pCam);
  m_pDevice->setEventReceiver(this);
  m_pDevice->getCursorControl()->setVisible(false);
  m_pTab->setVisible(true);
  m_pApInfo->setVisible(m_pAutoPilot->isEnabled());
  m_bSwitchToMenu=false;
  m_bActive=true;

  vector3df pos=m_pHeliBody->getRotation().rotationToDirection(vector3df(0,5,15)),
            up =m_pHeliBody->getRotation().rotationToDirection(vector3df(0,0.1,0)),
            tgt=m_pHeliBody->getRotation().rotationToDirection(vector3df(0,1  ,0));

  m_pCam->setPosition(m_pHeliBody->getPosition()+pos);
  m_pCam->setUpVector(up);
  m_pCam->setTarget(m_pHeliBody->getPosition()+tgt);

  loadHelpFile();
  wchar_t s[1024];
  swprintf(s,1023,m_pHelp->getText(),m_pController->getSettingsText(2));
  m_pHelp->setText(s);
}

void CHeli::deactivate() {
  m_pTab->setVisible(false);
  m_pApInfo->setVisible(false);
  m_bActive=false;
}

u32 CHeli::update() {
  u32 iRet=CIrrOdeCarState::update();

  vector3df rot=m_pHeliBody->getRotation();

  //get the parameters for the camera
  vector3df pos,tgt,
            up =m_pHeliBody->getRotation().rotationToDirection(vector3df(0,0.1,0));

  if (m_bInternal) {
    pos=rot.rotationToDirection(m_bBackView?core::vector3df(1.25,-0.15,-1.3):core::vector3df(0,-0.15,-1.3));
    tgt=rot.rotationToDirection(m_bBackView?core::vector3df(1.25,-0.15,5):core::vector3df(0,-0.15,-5));
  }
  else {
    pos=rot.rotationToDirection(m_bBackView?vector3df(0,5,-15):vector3df(0,5,15)),
    tgt=m_pHeliBody->getRotation().rotationToDirection(vector3df(0,5  ,0));
  }

  CProjectileManager *ppm=CProjectileManager::getSharedInstance();

  //if we follow a bomb we focus the last dropped bomb ...
  if (m_bMissileCam && ppm->getLast()!=NULL) {
    pos=ppm->getLast()->getBody()->getRotation().rotationToDirection(vector3df(0,5,10));
    m_pCam->setPosition(ppm->getLast()->getBody()->getPosition()+pos);
    m_pCam->setUpVector(vector3df(0,1,0));
    m_pCam->setTarget(ppm->getLast()->getBody()->getPosition());
  }
  else {  //... otherwise we focus the plane
    m_pCam->setPosition(m_pHeliBody->getPosition()+pos);
    m_pCam->setUpVector(up);
    m_pCam->setTarget(m_pHeliBody->getPosition()+tgt);
  }

  //now show some interesting information
  wchar_t dummy[0xFF];
  pos=m_pHeliBody->getAbsolutePosition();
  rot=m_pHeliBody->getRotation();
  vector3df v=m_pHeliBody->getLinearVelocity();
  swprintf(dummy,0xFE,L"pos: (%.0f, %.0f, %.0f)\nvelocity: (%.0f, %.0f, %.0f) %.2f",pos.X,pos.Y,pos.Z,v.X,v.Y,v.Z,m_pAero->getForewardVel());
  swprintf(dummy,0xFE,L"%s\nrotation: (%.0f, %.0f, %.0f)",dummy,rot.X,rot.Y,rot.Z);
  swprintf(dummy,0xFE,L"%s\nrudder: (%.0f%%, %.0f%%, %.0f%%)\n",dummy,m_fPitch*100.0f,m_fRoll*100.0f,m_fYaw*100.0f);

  swprintf(dummy,0xFE,L"%sThrust=%.0f%%",dummy,m_fThrust*100.0f);

  f32 fHeight=pos.Y-m_pTerrain->getHeight(pos.X,pos.Z);
  swprintf(dummy,0xFE,L"%s\nAltitude: %.0f",dummy,fHeight);

  if (m_bMissileCam) swprintf(dummy,0xFE,L"%s\nFollow Missile Cam",dummy);
  if (m_pAutoPilot->isEnabled()) swprintf(dummy,0xFE,L"%s\nAutopilot active (%i, %.0f)",dummy,m_iNextCp,m_fApDist);

  m_pInfo->setText(dummy);

  return iRet;
}

bool CHeli::OnEvent(const SEvent &event) {
  bool bRet=m_pController->OnEvent(event);
  bRet|=CIrrOdeCarState::OnEvent(event);
  return bRet;
}

bool CHeli::onEvent(IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    irr::ode::CIrrOdeEventStep *pStep=(irr::ode::CIrrOdeEventStep *)pEvent;
    vector3df rot=m_pHeliBody->getRotation();
    f32 alt=m_pHeliBody->getAbsolutePosition().Y,f=alt<=0?1:(2000-alt)/2000;

    if (m_bActive) {
      m_fThrust=m_pController->get(m_pCtrls[eHeliPowerUp]);

      m_fPitch=m_pController->get(m_pCtrls[eHeliPitchUp ]);
      m_fRoll =m_pController->get(m_pCtrls[eHeliRollLeft]);
      m_fYaw  =m_pController->get(m_pCtrls[eHeliYawRight]);

      if (m_fThrust> 1.0f) m_fThrust =1.0f;
      if (m_fThrust<-0.0f) m_fThrust/=4.0f;

      if (m_pController->get(m_pCtrls[eHeliFire])!=0.0f && pStep->getStepNo()-m_iLastShot>45) {
        m_iLastShot=pStep->getStepNo();
        //We add a new missile...
        vector3df pos=m_pHeliBody->getAbsolutePosition()+m_pHeliBody->getRotation().rotationToDirection(vector3df(m_bLeft?-3.0f:3.0f,-0.6f,-2.0f)),
                  rot=m_pHeliBody->getAbsoluteTransformation().getRotationDegrees(),
                  vel=m_pHeliBody->getLinearVelocity();

        CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true,this);
        p->setTarget(m_pTargetSelector->getTarget());
        m_bLeft=!m_bLeft;
        m_iShotsFired++;
      }

      if (m_pController->get(m_pCtrls[eHeliToggleCam])) {
        m_pController->set(m_pCtrls[eHeliToggleCam],0.0f);
        m_bMissileCam=!m_bMissileCam;
      }

      m_bBackView=m_pController->get(m_pCtrls[eHeliBackView])!=0.0f;

      if (m_pController->get(m_pCtrls[eHeliInternal])) {
        m_pController->set(m_pCtrls[eHeliInternal],0.0f);
        m_bInternal=!m_bInternal;
      }

      if (m_pController->get(m_pCtrls[eHeliFlip])) {
        m_pHeliBody->addForceAtPosition(m_pHeliBody->getPosition()+vector3df(0.0f,1.5f,0.0f),vector3df(0.0f,15.0f,0.0f));
      }

      if (m_pController->get(m_pCtrls[eHeliAutoPilot])) {
        m_pController->set(m_pCtrls[eHeliAutoPilot],0.0f);
        m_pAutoPilot->setEnabled(!m_pAutoPilot->isEnabled());
        m_pApInfo->setVisible(m_pAutoPilot->isEnabled());
      }

      if (m_pController->get(m_pCtrls[eHeliTarget])) {
        m_pController->set(m_pCtrls[eHeliTarget],0.0f);
        m_pTargetSelector->selectOption();
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
          printf("next checkpoint (%s): %i\n",m_pHeliBody->getName(),iNext);
        }
        m_iNextCp=iNext;
        m_pAutoPilot->setTarget(m_aCheckPoints[m_iNextCp]);
      }
    }

    m_pAutoPilot->step(m_fYaw,m_fPitch,m_fRoll,m_fThrust);
    m_pTargetSelector->update();

    if (m_pAutoPilot->isEnabled()) {
      if (m_fPitch==m_fPitch) m_pTorque->setPitch(m_fPitch);
      if (m_fRoll ==m_fRoll ) m_pTorque->setRoll (m_fRoll );
      if (m_fYaw  ==m_fYaw  ) m_pTorque->setYaw  (m_fYaw  );
    }
    else {
      m_pTorque->setPitch(m_fPitch);
      m_pTorque->setRoll(m_fRoll);
      m_pTorque->setYaw(m_fYaw);
    }
    m_pMotor->setPower(m_fThrust*f);

    if (m_pCockpit!=NULL && m_bActive) {
      core::vector3df vPos=m_pHeliBody->getPosition();

      m_pCockpit->setAltitude(vPos.Y);
      m_pCockpit->setSpeed(m_pAero->getForewardVel());
      m_pCockpit->setPower(100.0f*m_fThrust);
      m_pCockpit->setVelVert(m_pHeliBody->getLinearVelocity().Y);

      core::vector3df v=m_pHeliBody->getRotation().rotationToDirection(m_pAero->getForeward());
      core::vector2df vDir=core::vector2df(v.X,v.Z);

      if (v.getLength()>0.01f) m_pCockpit->setHeading(vDir.getAngle());

      m_pCockpit->setWarnStateHeli(0,m_pAutoPilot->isEnabled()?m_pAutoPilot->getState()==CAutoPilot::eApHeliLowAlt?2:1:0);
      m_pCockpit->setWarnStateHeli(1,vPos.Y<200.0f?3:vPos.Y<400.0f?2:1);

      v=m_pHeliBody->getAbsoluteTransformation().getRotationDegrees();
      m_pCockpit->setHorizon(v,v.rotationToDirection(core::vector3df(0.0f,1.0f,0.0f)));

      ode::CIrrOdeBody *pTarget=m_pTargetSelector->getTarget();

      if (pTarget!=NULL) {
        core::stringw s=core::stringw(pTarget->getName());
        m_pCockpit->setTargetName(s.c_str());
        m_pCockpit->setTargetDist((vPos-pTarget->getPosition()).getLength());
      }
      else {
        m_pCockpit->setTargetName(L"<no target>");
        m_pCockpit->setTargetDist(0.0f);
      }

      m_pCockpit->setShotsFired(m_iShotsFired);
      m_pCockpit->setHits(m_iHits);

      m_pTab->setVisible(false);
      m_pCockpit->update(false);
      m_pTab->setVisible(true);
    }
  }

  return false;
}

bool CHeli::handlesEvent(IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

void CHeli::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

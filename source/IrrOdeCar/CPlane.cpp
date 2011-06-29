  #include "CPlane.h"
  #include "CCustomEventReceiver.h"
  #include <math.h>
  #include <CProjectile.h>
  #include <irrCC.h>
  #include <CAutoPilot.h>
  #include <CTargetSelector.h>

CPlane::CPlane(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl) : CIrrOdeCarState(pDevice,L"Helicopter","../../data/irrOdePlaneHelp.txt",pCtrl) {
  //get the world node
  m_pWorld=reinterpret_cast<CIrrOdeWorld *>(m_pSmgr->getSceneNodeFromName("worldNode"));

  m_iLastShot=0;
  m_iLastMissile=0;
  m_fApDist=0.0f;

  m_pColMgr=pDevice->getSceneManager()->getSceneCollisionManager();
  m_cScreen=pDevice->getVideoDriver()->getScreenSize();

  //get the plane
  m_pPlaneBody=reinterpret_cast<CIrrOdeBody *>(pNode);
  if (m_pPlaneBody!=NULL) {
    m_bThreeWheeler=!strcmp(m_pPlaneBody->getName(),"plane2") || !strcmp(m_pPlaneBody->getName(),"plane4");
    printf("==> %s (%s)\n",m_pPlaneBody->getName(),m_bThreeWheeler?"YES":"NO");

    CCustomEventReceiver::getSharedInstance()->addPlane(m_pPlaneBody);
    //get the terrain
    m_pTerrain=reinterpret_cast<ITerrainSceneNode *>(m_pSmgr->getSceneNodeFromName("terrain"));
    //get the front wheel that is needed for steering the plane on ground
    m_pSteer=(CIrrOdeServo *)m_pPlaneBody->getMotorFromName("plane_wheel_steer");

    //get the visual rudders
    m_pRoll [0]=m_pSmgr->getSceneNodeFromName("roll1");
    m_pRoll [1]=m_pSmgr->getSceneNodeFromName("roll2");
    m_pPitch[0]=m_pSmgr->getSceneNodeFromName("pitch1");
    m_pPitch[1]=m_pSmgr->getSceneNodeFromName("pitch2");
    m_pYaw  [0]=m_pSmgr->getSceneNodeFromName("yaw");
    m_pYaw  [1]=m_pSmgr->getSceneNodeFromName("yaw2");

    //get the motor, the rudders and the aerodynamic caluclator of the plane
    m_pMotor =(irr::ode::CIrrOdeImpulseMotor *)m_pPlaneBody->getStepMotorFromName("plane_motor" );
    m_pTorque=(irr::ode::CIrrOdeTorqueMotor  *)m_pPlaneBody->getStepMotorFromName("plane_torque");
    m_pAero  =(irr::ode::CIrrOdeAeroDrag     *)m_pPlaneBody->getStepMotorFromName("plane_aero"  );

    //intially we don't follow the bombs and look to the front
    m_bFollowBombCam=false;
    m_bBackView=false;

    m_bAutoPilot=false;
    m_pAutoPilot=new CAutoPilot(m_pPlaneBody,m_pAero,m_pTorque,m_pMotor);
    m_pAutoPilot->setLinkYawRoll(true);

    m_pTargetSelector=new CTargetSelector(m_pPlaneBody,m_pDevice,m_pAero->getForeward());

    //we need the rear wheel bodies for the braking on ground
    m_pRearWheels[0]=m_pPlaneBody->getChildBodyFromName("plane_wheel_rl");
    m_pRearWheels[1]=m_pPlaneBody->getChildBodyFromName("plane_wheel_rr");

    m_pFrontWheel=m_pPlaneBody->getChildBodyFromName("frontwheel");

    printf("\nplane state\n\n");
    printf("rudders: (%i, %i), (%i, %i), (%i, %i)\n",(int)m_pRoll [0],(int)m_pRoll [1],
                                                     (int)m_pPitch[0],(int)m_pPitch[1],
                                                     (int)m_pYaw  [0],(int)m_pYaw  [1]);
    printf("motors: %i, %i, %i\n",(int)m_pMotor,(int)m_pTorque,(int)m_pAero);
    printf("rear wheels: %i, %i\n",(int)m_pRearWheels[0],(int)m_pRearWheels[1]);
    printf("front wheel: %i\n",(int)m_pFrontWheel);

    //let's add a camera
    m_pCam=m_pSmgr->addCameraSceneNode();

    //we are an IrrOde event listener
    CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);

    //initiate the engine
    m_iThrustDir=0;
    m_fThrust=0.0f;

    m_fYaw=0.0f;
    m_fPitch=0.0f;
    m_fRoll=0.0f;

    //finally we need a static text to display some information
    m_pInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,5,150,85),true);
    m_pInfo->setDrawBackground(true);
    m_pInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
    m_pInfo->setVisible(false);

    m_pApInfo=m_pGuiEnv->addStaticText(L"Hello World!",rect<s32>(5,95,150,355),true);
    m_pApInfo->setDrawBackground(true);
    m_pApInfo->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
    m_pApInfo->setVisible(false);
    m_pAutoPilot->setAutoPilotInfo(m_pApInfo);

    m_bLeftMissile=true;
    m_bInitialized=true;
  }

  m_iNextCp=-1;
  ISceneNode *pCheckRoot=m_pSmgr->getSceneNodeFromName("planepoints");
  if (pCheckRoot!=NULL) {
    irr::core::list<irr::scene::ISceneNode *> lChildren=pCheckRoot->getChildren();
    irr::core::list<irr::scene::ISceneNode *>::Iterator it;

    for (it=lChildren.begin(); it!=lChildren.end(); it++) m_aCheckPoints.push_back(*it);
    printf("%i checkpoints for plane found!\n",m_aCheckPoints.size());
  }
  else printf("no checkpoints for plane found!\n");

  m_lOdeNodes=irr::ode::CIrrOdeManager::getSharedInstance()->getIrrOdeNodes();
  m_itTarget=m_lOdeNodes.begin();
}

CPlane::~CPlane() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

//on activation we do the usual stuff (activate camera, set Irrlicht event receiver, hide mouse cursor...)
void CPlane::activate() {
  m_pSmgr->setActiveCamera(m_pCam);
  m_pDevice->setEventReceiver(this);
  m_pDevice->getCursorControl()->setVisible(false);
  m_pInfo->setVisible(true);
  m_pApInfo->setVisible(m_pAutoPilot->isEnabled());
  m_bSwitchToMenu=false;
  m_bActive=true;

  //get the parameters for the camera
  vector3df pos=m_pPlaneBody->getRotation().rotationToDirection(m_bBackView?vector3df(0,5,-15):vector3df(0,5,15)),
            up =m_pPlaneBody->getRotation().rotationToDirection(vector3df(0,0.1,0)),
            tgt=m_pPlaneBody->getRotation().rotationToDirection(vector3df(0,0  ,0));

  m_pCam->setPosition(m_pPlaneBody->getPosition()+pos);
  m_pCam->setUpVector(up);
  m_pCam->setTarget(m_pPlaneBody->getPosition()+tgt);

  loadHelpFile();
  wchar_t s[1024];
  swprintf(s,1023,m_pHelp->getText(),m_pController->getSettingsText(1));
  m_pHelp->setText(s);
}

void CPlane::deactivate() {
  m_pInfo->setVisible(false);
  m_pApInfo->setVisible(false);
  m_iThrustDir=0;
  m_bActive=false;
}

u32 CPlane::update() {
  //call the superclass's update method
  u32 iRet=CIrrOdeCarState::update();

  vector3df rot=m_pPlaneBody->getRotation();

  //get the parameters for the camera
  vector3df pos=rot.rotationToDirection(m_bBackView?vector3df(0,5,-15):vector3df(0,5,15)),
            up =m_pPlaneBody->getRotation().rotationToDirection(vector3df(0,0.1,0)),
            tgt=m_pPlaneBody->getRotation().rotationToDirection(vector3df(0,5  ,0));

  CProjectileManager *ppm=CProjectileManager::getSharedInstance();

  //if we follow a bomb we focus the last dropped bomb ...
  if (m_bFollowBombCam && ppm->getLast()!=NULL) {
    pos=ppm->getLast()->getBody()->getRotation().rotationToDirection(vector3df(0,5,10));
    m_pCam->setPosition(ppm->getLast()->getBody()->getPosition()+pos);
    m_pCam->setUpVector(vector3df(0,1,0));
    m_pCam->setTarget(ppm->getLast()->getBody()->getPosition());
  }
  else {  //... otherwise we focus the plane
    m_pCam->setPosition(m_pPlaneBody->getPosition()+pos);
    m_pCam->setUpVector(up);
    m_pCam->setTarget(m_pPlaneBody->getPosition()+tgt);
  }

  //now show some interesting information
  wchar_t dummy[0xFF];
  pos=m_pPlaneBody->getAbsolutePosition();
  rot=m_pPlaneBody->getRotation();
  vector3df v=m_pPlaneBody->getLinearVelocity();
  swprintf(dummy,0xFE,L"pos: (%.0f, %.0f, %.0f)\nvelocity: (%.0f, %.0f, %.0f) %.2f",pos.X,pos.Y,pos.Z,v.X,v.Y,v.Z,m_pAero->getForewardVel());
  swprintf(dummy,0xFE,L"%s\nrotation: (%.0f, %.0f, %.0f)",dummy,rot.X,rot.Y,rot.Z);
  swprintf(dummy,0xFE,L"%s\nrudder: (%.0f%%, %.0f%%, %.0f%%)\n",dummy,m_fPitch*100.0f,m_fRoll*100.0f,m_fYaw*100.0f);

  swprintf(dummy,0xFE,L"%sThrust=%.0f%%",dummy,m_fThrust*100.0f);

  f32 fHeight=pos.Y-m_pTerrain->getHeight(pos.X,pos.Z);
  swprintf(dummy,0xFE,L"%s\nAltitude: %.0f",dummy,fHeight);

  if (m_bFollowBombCam) swprintf(dummy,0xFE,L"%s\nFollow Missile Cam",dummy);
  if (m_bAutoPilot) swprintf(dummy,0xFE,L"%s\nAutopilot active (%i, %.0f)",dummy,m_iNextCp,m_fApDist);

  m_pInfo->setText(dummy);

  return iRet;
}

bool CPlane::OnEvent(const SEvent &event) {
  bool bRet=m_pController->OnEvent(event);
  bRet|=CIrrOdeCarState::OnEvent(event);
  return bRet;
}

bool CPlane::onEvent(IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    f32 fPitch=m_fPitch,fYaw=m_fYaw,fRoll=m_fRoll;
    //Step 1: get the plane's position
    vector3df vPos=m_pPlaneBody->getAbsolutePosition();

    irr::ode::CIrrOdeEventStep *pStep=(irr::ode::CIrrOdeEventStep *)pEvent;
    if (m_bActive) {
      //in order to regulate the increase and decrease of thrust we
      //just calculate the new value when the irrOdeStep event was
      //received. This way this calculation gets called exactly 60
      //times per second.
      m_fThrust=m_pController->get(m_pCtrls[ePlanePowerUp]);

      if (m_fThrust> 1.0f) m_fThrust=1.0f;
      if (m_fThrust<-0.1f) m_fThrust=-0.1f;

      if (m_pController->get(m_pCtrls[ePlanePowerZero])!=0.0f) {
        m_fThrust=0.0f;
        m_pController->set(m_pCtrls[ePlanePowerUp],0.0f);
      }

      //next up we calculate the loss of aerodynamic power in high altitudes. In fact the
      //loss starts at 300 and at 1500 there is no more aerodynamic power.
      f32 fMinHeight=300,fMaxHeight=2000;

      //Step 2: calculate the factor: full power (1.0f) if the height is below 300, no power (0.0f) if
      //        it's above 1500, linear interpolation in between.
      f32 fAltFact=vPos.Y<fMinHeight?1.0f:vPos.Y>fMaxHeight?0.0f:1-((vPos.Y-fMinHeight)/(fMaxHeight-fMinHeight));

      //Step 3: apply the factor to all relevant motors ...
      m_pMotor->setPower(fAltFact*m_fThrust); //... the engine ...
      m_pAero->setPower (fAltFact);            //... the aerodynamics ...
      m_pTorque->setPower(fAltFact);          //... and the rudders.

      fPitch=m_pController->get(m_pCtrls[ePlanePitchUp  ]);
      fRoll =m_pController->get(m_pCtrls[ePlaneRollLeft ]);
      fYaw  =m_pController->get(m_pCtrls[ePlaneYawRight ]);

      m_pTorque->setPitch(fPitch);
      m_pTorque->setRoll (fRoll );
      m_pTorque->setYaw  (fYaw  );

      if (m_bThreeWheeler)
        m_pSteer->setServoPos(-20.0f*fYaw);
      else
        m_pSteer->setServoPos(10.0f*fYaw);

      if (m_pController->get(m_pCtrls[ePlaneFire])!=0.0f && pStep->getStepNo()-m_iLastMissile>=120) {
        m_iLastMissile=pStep->getStepNo();
        //We add a new bomb...
        vector3df pos=m_pPlaneBody->getAbsolutePosition()+m_pPlaneBody->getRotation().rotationToDirection(vector3df(m_bLeftMissile?4.0f:-4.0f,-1.0f,-2.0f)),
                  rot=m_pPlaneBody->getRotation(),vel=m_pPlaneBody->getLinearVelocity();

        m_bLeftMissile=!m_bLeftMissile;

        CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true);
        p->setTarget(m_pTargetSelector->getTarget());
      }

      if (m_pController->get(m_pCtrls[ePlaneGun])!=0.0f && pStep->getStepNo()-m_iLastShot>45) {
        m_iLastShot=pStep->getStepNo();
        //We add a new bullet...
        vector3df pos=m_pPlaneBody->getAbsolutePosition()+m_pPlaneBody->getRotation().rotationToDirection(vector3df(0.2f,1.5f,-11.0f)),
                  rot=m_pPlaneBody->getRotation(),
                  vel=m_pPlaneBody->getLinearVelocity().getLength()*m_pPlaneBody->getRotation().rotationToDirection(vector3df(0.0f,0.0f,1.0f))+m_pPlaneBody->getRotation().rotationToDirection(vector3df(0.0f,0.0f,-350.0f));

        new CProjectile(m_pSmgr,pos,rot,vel,"bullet",600,m_pWorld,true);

      }

      m_pRearWheels[0]->setAngularDamping(0.8f*m_pController->get(m_pCtrls[ePlaneBrake])+0.01f);
      m_pRearWheels[1]->setAngularDamping(0.8f*m_pController->get(m_pCtrls[ePlaneBrake])+0.01f);

      m_bBackView=m_pController->get(m_pCtrls[ePlaneBackview])!=0.0f;

      if (m_pController->get(m_pCtrls[ePlaneToggleCam])!=0.0f) {
        m_pController->set(m_pCtrls[ePlaneToggleCam],0.0f);
        m_bFollowBombCam=!m_bFollowBombCam;
      }

      if (m_pController->get(m_pCtrls[ePlaneFlip])!=0.0f)
        m_pPlaneBody->addForceAtPosition(m_pPlaneBody->getPosition()+vector3df(0.0f,0.5f,0.0f),vector3df(0.0f,12.0f,0.0f));

      if (m_pController->get(m_pCtrls[ePlaneAutoPilot])) {
        m_pController->set(m_pCtrls[ePlaneAutoPilot],0.0f);
        m_bAutoPilot=!m_bAutoPilot;
        m_pAutoPilot->setEnabled(m_bAutoPilot);
        m_pApInfo->setVisible(m_pAutoPilot->isEnabled());
      }

      if (m_pController->get(m_pCtrls[ePlaneTarget])) {
        m_pController->set(m_pCtrls[ePlaneTarget],0.0f);
        m_pTargetSelector->selectOption();
      }
    }

    m_pAutoPilot->step(fYaw,fPitch,fRoll,m_fThrust);
    m_pTargetSelector->update();

    if (m_bAutoPilot) {
      if (m_iNextCp==-1) {
        m_iNextCp=rand()%m_aCheckPoints.size();
        m_pAutoPilot->setTarget(m_aCheckPoints[m_iNextCp]);
      }

      m_pMotor->setPower(m_fThrust);

      if (fPitch==fPitch) m_pTorque->setPitch(fPitch);
      if (fRoll ==fRoll ) m_pTorque->setRoll (fRoll );
      if (fYaw  ==fYaw  ) m_pTorque->setYaw  (fYaw  );

      m_fApDist=m_pAutoPilot->getApDist();
      if (m_fApDist<100.0f) {
        s32 iNext=m_iNextCp;
        while (iNext==m_iNextCp) {
          iNext=rand()%m_aCheckPoints.size();
          printf("next checkpoint (%s): %i\n",m_pPlaneBody->getName(),iNext);
        }
        m_iNextCp=iNext;
        m_pAutoPilot->setTarget(m_aCheckPoints[m_iNextCp]);
      }
    }

    if (fYaw!=m_fYaw || fPitch!=m_fPitch || fRoll!=m_fRoll) {
      CEventRudderPositions *p=new CEventRudderPositions(m_pPlaneBody->getID(),fYaw,fPitch,fRoll,m_bThreeWheeler);
      ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
      m_fYaw=fYaw;
      m_fRoll=fRoll;
      m_fPitch=fPitch;
    }
  }

  return false;
}

bool CPlane::handlesEvent(IIrrOdeEvent *pEvent) {
  //we are just interested in the step event.
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

void CPlane::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

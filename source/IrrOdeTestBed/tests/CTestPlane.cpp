  #include <tests/CTestPlane.h>

CTestPlane::CTestPlane(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Plane Test";
  m_sDescr=L"Just a simple test with a sphere bouncing of a box (formerly this was the \"HelloOdeWorld\" application).";
}

void CTestPlane::activate() {
  m_pSmgr->loadScene(DATADIR "/scenes/Testbed_plane.xml");

  m_pObject =reinterpret_cast<irr::ode::CIrrOdeBody         *>(m_pSmgr->getSceneNodeFromName("plane"      ));
  m_pMotor  =reinterpret_cast<irr::ode::CIrrOdeImpulseMotor *>(m_pSmgr->getSceneNodeFromName("aero_motor" ));
  m_pControl=reinterpret_cast<irr::ode::CIrrOdeTorqueMotor  *>(m_pSmgr->getSceneNodeFromName("aero_torque"));
  m_pAero   =reinterpret_cast<irr::ode::CIrrOdeAeroDrag     *>(m_pSmgr->getSceneNodeFromName("aero_aero"  ));

  m_pCam=m_pSmgr->addCameraSceneNode();

  m_pInfo=m_pGui->addStaticText(L"Hello World",irr::core::rect<irr::s32>(10,10,210,110),true,true,NULL,-1,true);

  printf("\nPlane's ODE objects:\n\tplane body: %i\n\tmotor: %i\n",(int)m_pObject,(int)m_pMotor);
  printf("\taero: %i\n\tcontrols: %i\n\tcamera: %i\n\n",(int)m_pAero,(int)m_pControl,(int)m_pCam);

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();
  m_pOdeMngr->getQueue()->addEventListener(this);

  m_fPitch =0.0f;
  m_fYaw   =0.0f;
  m_fRoll  =0.0f;
  m_fThrust=0.0f;

  printf("Ready.\n");

  m_pRunner->setEventReceiver(this);
}

void CTestPlane::deactivate() {
  m_pSmgr->clear();
  m_pGui->clear();
  m_pOdeMngr->closeODE();
  m_pOdeMngr->getQueue()->removeEventListener(this);
  m_pRunner->setEventReceiver(NULL);
}

irr::s32 CTestPlane::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  if (m_pObject!=NULL && m_pCam!=NULL) {
    irr::core::vector3df vPos=m_pObject->getPosition(),
                         vRot=m_pObject->getRotation(),
                         vCam=vPos+vRot.rotationToDirection(irr::core::vector3df(0.0f,3.0f,12.5f)),
                         vUp =vRot.rotationToDirection(m_pAero->getUpward());

    m_pCam->setPosition(vCam);
    m_pCam->setUpVector(vUp);
    m_pCam->setTarget(vPos+3.0f*vUp);
  }

  return iRet;
}

bool CTestPlane::OnEvent(const irr::SEvent &event) {
  bool bRet=false;
  if (event.EventType==irr::EET_KEY_INPUT_EVENT) {
    if (event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case irr::KEY_KEY_W: m_fDelta= 1.0f; break;
        case irr::KEY_KEY_S: m_fDelta=-1.0f; break;
        case irr::KEY_KEY_D: m_fYaw  = 1.0f; break;
        case irr::KEY_KEY_A: m_fYaw  =-1.0f; break;
        case irr::KEY_DOWN : m_fPitch= 1.0f; break;
        case irr::KEY_UP   : m_fPitch=-1.0f; break;
        case irr::KEY_LEFT : m_fRoll = 1.0f; break;
        case irr::KEY_RIGHT: m_fRoll =-1.0f; break;
        default: break;
      }
    }
    else {
      switch (event.KeyInput.Key) {
        case irr::KEY_KEY_W: m_fDelta=0.0f; break;
        case irr::KEY_KEY_S: m_fDelta=0.0f; break;
        case irr::KEY_KEY_A: m_fYaw  =0.0f; break;
        case irr::KEY_KEY_D: m_fYaw  =0.0f; break;
        case irr::KEY_DOWN : m_fPitch=0.0f; break;
        case irr::KEY_UP   : m_fPitch=0.0f; break;
        case irr::KEY_LEFT : m_fRoll =0.0f; break;
        case irr::KEY_RIGHT: m_fRoll =0.0f; break;
        default: break;
      }
    }
  }

  return bRet;
}

bool CTestPlane::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    m_fThrust+=m_fDelta*0.0125f;
    if (m_fThrust> 1.0f) m_fThrust= 1.0f;
    if (m_fThrust<-0.3f) m_fThrust=-0.3f;
    m_pMotor->setPower(m_fThrust);

    m_pControl->setPitch(m_fPitch);
    m_pControl->setRoll (m_fRoll );
    m_pControl->setYaw  (m_fYaw  );

    irr::core::vector3df vPos=m_pObject->getPosition(),
                         vVel=m_pObject->getLinearVelocity();

    irr::f32 fForewardVel=m_pAero->getForewardVel();

    wchar_t s[0xFF];
    swprintf(s,0xFF,L"position: %.2f, %.2f, %.2f\nvelocity: %.2f, %.2f, %.2f\nforeward velocity: %.2f\nthrust: %.2f\nrudder: p=%.2f, r=%.2f, y=%.2f",
             vPos.X,vPos.Y,vPos.Z,vVel.X,vVel.Y,vVel.Z,fForewardVel,m_fThrust,m_fPitch,m_fRoll,m_fYaw);
    m_pInfo->setText(s);
  }

  return false;
}

bool CTestPlane::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}


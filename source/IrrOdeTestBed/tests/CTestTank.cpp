  #include <tests/CTestTank.h>

CTestTank::CTestTank(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Tank Test";
  m_sDescr=L"Test with user controlled tank (unarmed)";
}

void CTestTank::activate() {
  m_pSmgr->loadScene("../../data/scenes/Testbed_tank.xml");

  m_pCam=m_pSmgr->addCameraSceneNode();

  m_pObject=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("tank"));

  m_pEngine[0]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("motorRR"));
  m_pEngine[1]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("motorRL"));
  m_pEngine[2]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("motorFR"));
  m_pEngine[3]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("motorFL"));

  m_pInfo=m_pGui->addStaticText(L"Hello World",irr::core::rect<irr::s32>(10,10,210,160),true,true,NULL,-1,true);

  printf("\nOde Test Tank's objects:\n\tbody: %i\n\tmotors: %i, %i, %i, %i\n\n",
         (int)m_pObject,(int)m_pEngine[0],(int)m_pEngine[1],(int)m_pEngine[2],(int)m_pEngine[3]);

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();
  m_pOdeMngr->getQueue()->addEventListener(this);

  printf("Ready.\n");

  m_pRunner->setEventReceiver(this);

  m_fPower=0.0f;
  m_fSteer=0.0f;
}

void CTestTank::deactivate() {
  m_pGui->clear();
  m_pSmgr->clear();
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
  m_pOdeMngr->getQueue()->removeEventListener(this);
  m_pRunner->setEventReceiver(NULL);
}

irr::s32 CTestTank::update() {
  irr::s32 iRet=0;

  //step the simulation
  m_pOdeMngr->step();

  if (m_pObject!=NULL && m_pCam!=NULL) {
    irr::core::vector3df vPos=m_pObject->getPosition(),
                         vRot=m_pObject->getRotation(),
                         vCam=vPos+vRot.rotationToDirection(irr::core::vector3df(12.5f,6.0f,0.0f)),
                         vUp =m_pObject->getRotation().rotationToDirection(irr::core::vector3df(0.0f,1.0f,0.0f));

    m_pCam->setPosition(vCam);
    m_pCam->setUpVector(vUp);
    m_pCam->setTarget(vPos+3.0f*vUp);
  }

  return iRet;
}

bool CTestTank::OnEvent(const irr::SEvent &event) {
  bool bRet=false;
  if (event.EventType==irr::EET_KEY_INPUT_EVENT) {
    if (event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case irr::KEY_UP  : m_fPower=-1.0f; break;
        case irr::KEY_DOWN: m_fPower= 1.0f; break;

        case irr::KEY_LEFT : m_fSteer= 1.0f; break;
        case irr::KEY_RIGHT: m_fSteer=-1.0f; break;
        default: break;
      }
    }
    else {
      switch (event.KeyInput.Key) {
        case irr::KEY_UP:
        case irr::KEY_DOWN:
          m_fPower=0.0f;
          break;

        case irr::KEY_LEFT:
        case irr::KEY_RIGHT:
          m_fSteer=0.0f;
          break;

        default: break;
      }
    }

    m_pEngine[0]->setVelocity(20.0f*m_fPower-15.0f*m_fSteer);
    m_pEngine[1]->setVelocity(20.0f*m_fPower+15.0f*m_fSteer);
    m_pEngine[2]->setVelocity(20.0f*m_fPower-15.0f*m_fSteer);
    m_pEngine[3]->setVelocity(20.0f*m_fPower+15.0f*m_fSteer);
  }

  return bRet;
}

bool CTestTank::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    irr::core::vector3df vPos=m_pObject->getPosition(),
                         vRot=m_pObject->getRotation(),
                         vVel=m_pObject->getLinearVelocity();
    irr::f32 fVel=(vRot.rotationToDirection(irr::core::vector3df(-1.0f,0.0f,0.0f))).dotProduct(vVel);

    wchar_t s[0xFF];
    swprintf(s,0xFF,L"Position: %.2f, %.2f, %.2f\nVelocity: %.2f, %.2f, %.2f\nForeward Velocity: %.2f\nPower: %.2f, Steer: %.2f",
             vPos.X,vPos.Y,vPos.Z,vVel.X,vVel.Y,vVel.Z,fVel,m_fPower,m_fSteer);
    m_pInfo->setText(s);
  }

  return false;
}

bool CTestTank::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}



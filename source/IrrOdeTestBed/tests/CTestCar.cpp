  #include <tests/CTestCar.h>

CTestCar::CTestCar(irr::IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Car Test";
  m_sDescr=L"Test with user controlled car";
}

void CTestCar::activate() {
  m_pSmgr->loadScene(DATADIR "/scenes/Testbed_car.xml");

  m_pCam=m_pSmgr->addCameraSceneNode();

  m_pObject=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("car"));

  m_pEngine[0]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("sc_motor1"));
  m_pEngine[1]=reinterpret_cast<irr::ode::CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("sc_motor2"));

  m_pSteer[0]=reinterpret_cast<irr::ode::CIrrOdeServo *>(m_pSmgr->getSceneNodeFromName("sc_servo1"));
  m_pSteer[1]=reinterpret_cast<irr::ode::CIrrOdeServo *>(m_pSmgr->getSceneNodeFromName("sc_servo2"));

  m_pInfo=m_pGui->addStaticText(L"Hello World",irr::core::rect<irr::s32>(10,10,210,160),true,true,NULL,-1,true);

  printf("\nOde Test Car's objects:\n\tbody: %i\n\tmotors: %i, %i\n\tservos: %i, %i\n\n",
         (int)m_pObject,(int)m_pEngine[0],(int)m_pEngine[1],(int)m_pSteer[0],(int)m_pSteer[1]);

  //init the ODE
  m_pOdeMngr->initODE();
  m_pOdeMngr->initPhysics();
  m_pOdeMngr->getQueue()->addEventListener(this);

  printf("Ready.\n");

  m_pRunner->setEventReceiver(this);
}

void CTestCar::deactivate() {
  m_pGui->clear();
  m_pSmgr->clear();
  m_pOdeMngr->closeODE();
  m_pOdeMngr->getQueue()->removeEventListener(this);
  m_pRunner->setEventReceiver(NULL);
}

irr::s32 CTestCar::update() {
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

bool CTestCar::OnEvent(const irr::SEvent &event) {
  bool bRet=false;
  if (event.EventType==irr::EET_KEY_INPUT_EVENT) {
    if (event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case irr::KEY_UP  : m_pEngine[0]->setVelocity(-25.0f); m_pEngine[1]->setVelocity(-25.0f); break;
        case irr::KEY_DOWN: m_pEngine[0]->setVelocity( 25.0f); m_pEngine[1]->setVelocity( 25.0f); break;

        case irr::KEY_LEFT : m_pSteer[0]->setServoPos( 25.0f); m_pSteer[1]->setServoPos( 25.0f); break;
        case irr::KEY_RIGHT: m_pSteer[0]->setServoPos(-25.0f); m_pSteer[1]->setServoPos(-25.0f); break;
        default: break;
      }
    }
    else {
      switch (event.KeyInput.Key) {
        case irr::KEY_UP:
        case irr::KEY_DOWN:
          m_pEngine[0]->setVelocity(0.0f);
          m_pEngine[1]->setVelocity(0.0f);
          break;

        case irr::KEY_LEFT:
        case irr::KEY_RIGHT:
          m_pSteer[0]->setServoPos(0.0f);
          m_pSteer[1]->setServoPos(0.0f);
          break;

        default: break;
      }
    }
  }

  return bRet;
}

bool CTestCar::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    irr::core::vector3df vPos=m_pObject->getPosition(),
                         vRot=m_pObject->getRotation(),
                         vVel=m_pObject->getLinearVelocity();
    irr::f32 fVel=(vRot.rotationToDirection(irr::core::vector3df(-1.0f,0.0f,0.0f))).dotProduct(vVel);

    wchar_t s[0xFF];
    swprintf(s,0xFF,L"Position: %.2f, %.2f, %.2f\nVelocity: %.2f, %.2f, %.2f\nForeward Velocity: %.2f",
             vPos.X,vPos.Y,vPos.Z,vVel.X,vVel.Y,vVel.Z,fVel);
    m_pInfo->setText(s);
  }

  return false;
}

bool CTestCar::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventStep;
}


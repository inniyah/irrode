  #include <tests/CTestCar.h>

CTestCar::CTestCar(IrrlichtDevice *pDevice, IRunner *pRunner) : IState(pDevice, pRunner) {
  m_sName=L"Car Test";
  m_sDescr=L"Test with user controlled car";
}

void CTestCar::activate() {
  m_pSmgr->loadScene("../../data/scenes/Testbed_car.xml");

  m_pCam=m_pSmgr->addCameraSceneNode();
  
  m_pObject=reinterpret_cast<ode::CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName("car"));
  
  m_pEngine[0]=reinterpret_cast<ode::CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("sc_motor1"));
  m_pEngine[1]=reinterpret_cast<ode::CIrrOdeMotor *>(m_pSmgr->getSceneNodeFromName("sc_motor2"));
  
  m_pSteer[0]=reinterpret_cast<ode::CIrrOdeServo *>(m_pSmgr->getSceneNodeFromName("sc_servo1"));
  m_pSteer[1]=reinterpret_cast<ode::CIrrOdeServo *>(m_pSmgr->getSceneNodeFromName("sc_servo2"));
  
  m_pInfo=m_pGui->addStaticText(L"Hello World",core::rect<s32>(10,10,210,160),true,true,NULL,-1,true);
  
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
  m_pOdeMngr->clearODE();
  m_pOdeMngr->closeODE();
  m_pOdeMngr->getQueue()->removeEventListener(this);
  m_pRunner->setEventReceiver(NULL);
}

s32 CTestCar::update() {
  s32 iRet=0;
  
  //step the simulation
  m_pOdeMngr->step();
  
  if (m_pObject!=NULL && m_pCam!=NULL) {
    core::vector3df vPos=m_pObject->getPosition(),
                    vRot=m_pObject->getRotation(),
                    vCam=vPos+vRot.rotationToDirection(core::vector3df(12.5f,6.0f,0.0f)),
                    vUp =m_pObject->getRotation().rotationToDirection(core::vector3df(0.0f,1.0f,0.0f));
    
    m_pCam->setPosition(vCam);
    m_pCam->setUpVector(vUp);
    m_pCam->setTarget(vPos+3.0f*vUp);
  }
  
  return iRet;
}

bool CTestCar::OnEvent(const SEvent &event) {
  bool bRet=false;
  if (event.EventType==EET_KEY_INPUT_EVENT) {
    if (event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case KEY_UP  : m_pEngine[0]->setVelocity(-25.0f); m_pEngine[1]->setVelocity(-25.0f); break;
        case KEY_DOWN: m_pEngine[0]->setVelocity( 25.0f); m_pEngine[1]->setVelocity( 25.0f); break;
        
        case KEY_LEFT : m_pSteer[0]->setServoPos( 25.0f); m_pSteer[1]->setServoPos( 25.0f); break;
        case KEY_RIGHT: m_pSteer[0]->setServoPos(-25.0f); m_pSteer[1]->setServoPos(-25.0f); break;
        default: break;
      }
    }
    else {
      switch (event.KeyInput.Key) {
        case KEY_UP: 
        case KEY_DOWN:
          m_pEngine[0]->setVelocity(0.0f); 
          m_pEngine[1]->setVelocity(0.0f); 
          break;
          
        case KEY_LEFT:
        case KEY_RIGHT:
          m_pSteer[0]->setServoPos(0.0f);
          m_pSteer[1]->setServoPos(0.0f);
          break;
          
        default: break;
      }
    }
  }
  
  return bRet; 
}

bool CTestCar::onEvent(ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==ode::eIrrOdeEventStep) {
    core::vector3df vPos=m_pObject->getPosition(),
                    vRot=m_pObject->getRotation(),
                    vVel=m_pObject->getLinearVelocity();
    f32 fVel=(vRot.rotationToDirection(core::vector3df(-1.0f,0.0f,0.0f))).dotProduct(vVel);
    
    wchar_t s[0xFF];
    swprintf(s,0xFF,L"Position: %.2f, %.2f, %.2f\nVelocity: %.2f, %.2f, %.2f\nForeward Velocity: %.2f",
             vPos.X,vPos.Y,vPos.Z,vVel.X,vVel.Y,vVel.Z,fVel);
    m_pInfo->setText(s);
  }
  
  return false;
}

bool CTestCar::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==ode::eIrrOdeEventStep;
}


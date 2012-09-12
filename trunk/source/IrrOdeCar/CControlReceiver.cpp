  #include <irrlicht.h>
  #include <CControlReceiver.h>
  #include <CControlEvents.h>
  #include <irrCC.h>
  #include <CConfigFileManager.h>

  #include <CCar.h>
  #include <CTank.h>
  #include <CMenu.h>
  #include <CVehicle.h>
  #include <CController.h>
  #include <CAeroVehicle.h>
  #include <CCameraController.h>

void CControlReceiver::initControls() {
  //first up: the car's controls
  m_iCtrls[0][eCarForeward           ]=m_pController->addItem(0,irr::core::stringw(L"Foreward"             ),irr::KEY_UP    ,CIrrCC::eCtrlAxis);
  m_iCtrls[0][eCarBackward           ]=m_pController->addItem(0,irr::core::stringw(L"Backward"             ),irr::KEY_DOWN  ,CIrrCC::eCtrlAxis);
  m_iCtrls[0][eCarLeft               ]=m_pController->addItem(0,irr::core::stringw(L"Left"                 ),irr::KEY_LEFT  ,CIrrCC::eCtrlAxis);
  m_iCtrls[0][eCarRight              ]=m_pController->addItem(0,irr::core::stringw(L"Right"                ),irr::KEY_RIGHT ,CIrrCC::eCtrlAxis);
  m_iCtrls[0][eCarFlip               ]=m_pController->addItem(0,irr::core::stringw(L"Flip"                 ),irr::KEY_RETURN,CIrrCC::eCtrlToggleButton);
  m_iCtrls[0][eCarDifferential       ]=m_pController->addItem(0,irr::core::stringw(L"Toggle Differential"  ),irr::KEY_KEY_D ,CIrrCC::eCtrlButton);
  m_iCtrls[0][eCarShiftUp            ]=m_pController->addItem(0,irr::core::stringw(L"Shift Up"             ),irr::KEY_KEY_W ,CIrrCC::eCtrlToggleButton);
  m_iCtrls[0][eCarShiftDown          ]=m_pController->addItem(0,irr::core::stringw(L"Shift Down"           ),irr::KEY_KEY_S ,CIrrCC::eCtrlToggleButton);
  m_iCtrls[0][eCarBoost              ]=m_pController->addItem(0,irr::core::stringw(L"Boost"                ),irr::KEY_KEY_B ,CIrrCC::eCtrlButton);
  m_iCtrls[0][eCarAdapSteer          ]=m_pController->addItem(0,irr::core::stringw(L"Adaptive Steer"       ),irr::KEY_KEY_A ,CIrrCC::eCtrlToggleButton);

  //we need two axes for the car: acceleration and steering
  m_pController->createAxis(m_iCtrls[0][eCarForeward],m_iCtrls[0][eCarBackward]);
  m_pController->createAxis(m_iCtrls[0][eCarLeft    ],m_iCtrls[0][eCarRight   ]);

  //next up: the tank
  m_iCtrls[1][eTankForeward     ]=m_pController->addItem(1,irr::core::stringw(L"Foreward"         ),irr::KEY_UP    ,CIrrCC::eCtrlAxis);
  m_iCtrls[1][eTankBackward     ]=m_pController->addItem(1,irr::core::stringw(L"Backward"         ),irr::KEY_DOWN  ,CIrrCC::eCtrlAxis);
  m_iCtrls[1][eTankLeft         ]=m_pController->addItem(1,irr::core::stringw(L"Left"             ),irr::KEY_LEFT  ,CIrrCC::eCtrlAxis);
  m_iCtrls[1][eTankRight        ]=m_pController->addItem(1,irr::core::stringw(L"Right"            ),irr::KEY_RIGHT ,CIrrCC::eCtrlAxis);
  m_iCtrls[1][eTankCannonLeft   ]=m_pController->addItem(1,irr::core::stringw(L"Cannon Left"      ),irr::KEY_KEY_A ,CIrrCC::eCtrlAxis);
  m_iCtrls[1][eTankCannonRight  ]=m_pController->addItem(1,irr::core::stringw(L"Cannon Right"     ),irr::KEY_KEY_D ,CIrrCC::eCtrlAxis);
  m_iCtrls[1][eTankCannonUp     ]=m_pController->addItem(1,irr::core::stringw(L"Cannon Up"        ),irr::KEY_KEY_W ,CIrrCC::eCtrlAxis);
  m_iCtrls[1][eTankCannonDown   ]=m_pController->addItem(1,irr::core::stringw(L"Cannon Down"      ),irr::KEY_KEY_S ,CIrrCC::eCtrlAxis);
  m_iCtrls[1][eTankFire         ]=m_pController->addItem(1,irr::core::stringw(L"Fire"             ),irr::KEY_SPACE ,CIrrCC::eCtrlToggleButton);
  m_iCtrls[1][eTankFlip         ]=m_pController->addItem(1,irr::core::stringw(L"Flip"             ),irr::KEY_RETURN,CIrrCC::eCtrlToggleButton);
  m_iCtrls[1][eTankFastCollision]=m_pController->addItem(1,irr::core::stringw(L"Fast Collision"   ),irr::KEY_KEY_F ,CIrrCC::eCtrlToggleButton);

  //this time we're gonna create four axes: acceleration, steering, turret movement and cannon angle
  m_pController->createAxis(m_iCtrls[1][eTankForeward  ],m_iCtrls[1][eTankBackward   ]);
  m_pController->createAxis(m_iCtrls[1][eTankLeft      ],m_iCtrls[1][eTankRight      ]);
  m_pController->createAxis(m_iCtrls[1][eTankCannonUp  ],m_iCtrls[1][eTankCannonDown ]);
  m_pController->createAxis(m_iCtrls[1][eTankCannonLeft],m_iCtrls[1][eTankCannonRight]);

  m_iCtrls[2][eAeroPitchUp      ]=m_pController->addItem(2,irr::core::stringw("Pitch Up"      ),irr::KEY_DOWN  ,CIrrCC::eCtrlAxis);
  m_iCtrls[2][eAeroPitchDown    ]=m_pController->addItem(2,irr::core::stringw("Pitch Down"    ),irr::KEY_UP    ,CIrrCC::eCtrlAxis);
  m_iCtrls[2][eAeroRollLeft     ]=m_pController->addItem(2,irr::core::stringw("Roll Left"     ),irr::KEY_LEFT  ,CIrrCC::eCtrlAxis);
  m_iCtrls[2][eAeroRollRight    ]=m_pController->addItem(2,irr::core::stringw("Roll Right"    ),irr::KEY_RIGHT ,CIrrCC::eCtrlAxis);
  m_iCtrls[2][eAeroYawLeft      ]=m_pController->addItem(2,irr::core::stringw("Yaw Left"      ),irr::KEY_KEY_A ,CIrrCC::eCtrlAxis);
  m_iCtrls[2][eAeroYawRight     ]=m_pController->addItem(2,irr::core::stringw("Yaw Right"     ),irr::KEY_KEY_D ,CIrrCC::eCtrlAxis);
  m_iCtrls[2][eAeroPowerUp      ]=m_pController->addItem(2,irr::core::stringw("Power Up"      ),irr::KEY_KEY_W ,CIrrCC::eCtrlFader);
  m_iCtrls[2][eAeroPowerDown    ]=m_pController->addItem(2,irr::core::stringw("Power Down"    ),irr::KEY_KEY_S ,CIrrCC::eCtrlFader);
  m_iCtrls[2][eAeroPowerZero    ]=m_pController->addItem(2,irr::core::stringw("Power Zero"    ),irr::KEY_KEY_Z ,CIrrCC::eCtrlToggleButton);
  m_iCtrls[2][eAeroBrake        ]=m_pController->addItem(2,irr::core::stringw("Wheel Brake"   ),irr::KEY_KEY_B ,CIrrCC::eCtrlAxis);
  m_iCtrls[2][eAeroFirePrimary  ]=m_pController->addItem(2,irr::core::stringw("Fire Primary"  ),irr::KEY_SPACE ,CIrrCC::eCtrlToggleButton);
  m_iCtrls[2][eAeroFireSecondary]=m_pController->addItem(2,irr::core::stringw("Fire Secondary"),irr::KEY_RETURN,CIrrCC::eCtrlToggleButton);
  m_iCtrls[2][eAeroSelectTarget ]=m_pController->addItem(2,irr::core::stringw("Select Target" ),irr::KEY_KEY_T ,CIrrCC::eCtrlToggleButton);
  m_iCtrls[2][eAeroAutoPilot    ]=m_pController->addItem(2,irr::core::stringw("Autopilot"     ),irr::KEY_KEY_P ,CIrrCC::eCtrlToggleButton);
  m_iCtrls[2][eAeroFlip         ]=m_pController->addItem(2,irr::core::stringw("Flip"          ),irr::KEY_KEY_L ,CIrrCC::eCtrlToggleButton);

  m_pController->createAxis(m_iCtrls[2][eAeroYawLeft ],m_iCtrls[2][eAeroYawRight ]);
  m_pController->createAxis(m_iCtrls[2][eAeroRollLeft],m_iCtrls[2][eAeroRollRight]);
  m_pController->createAxis(m_iCtrls[2][eAeroPitchUp ],m_iCtrls[2][eAeroPitchDown]);

  m_pController->createFader(m_iCtrls[2][eAeroPowerUp],m_iCtrls[2][eAeroPowerDown],10,0.01f);

  m_iCtrls[3][eCameraLeft      ] = m_pController->addItem(3, irr::core::stringw("Camera Left"               ), irr::KEY_KEY_Y, CIrrCC::eCtrlAxis);
  m_iCtrls[3][eCameraRight     ] = m_pController->addItem(3, irr::core::stringw("Camera Right"              ), irr::KEY_KEY_C, CIrrCC::eCtrlAxis);
  m_iCtrls[3][eCameraUp        ] = m_pController->addItem(3, irr::core::stringw("Camera Up"                 ), irr::KEY_KEY_F, CIrrCC::eCtrlAxis);
  m_iCtrls[3][eCameraDown      ] = m_pController->addItem(3, irr::core::stringw("Camera Down"               ), irr::KEY_KEY_V, CIrrCC::eCtrlAxis);
  m_iCtrls[3][eCameraCenter    ] = m_pController->addItem(3, irr::core::stringw("Center Camera"             ), irr::KEY_KEY_X, CIrrCC::eCtrlButton);
  m_iCtrls[3][eCameraInternal  ] = m_pController->addItem(3, irr::core::stringw("Toggle Internal"           ), irr::KEY_KEY_I, CIrrCC::eCtrlToggleButton);
  m_iCtrls[3][eCameraButtonMove] = m_pController->addItem(3, irr::core::stringw("Push Button for Mouse Move"), irr::KEY_KEY_M, CIrrCC::eCtrlToggleButton);

  m_pController->createAxis(m_iCtrls[3][eCameraLeft], m_iCtrls[3][eCameraRight]);
  m_pController->createAxis(m_iCtrls[3][eCameraUp  ], m_iCtrls[3][eCameraDown ]);
}

void CControlReceiver::initWorld(irr::scene::ISceneNode *pNode) {
  if (pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)irr::ode::IRR_ODE_WORLD_ID) {
    m_pWorld = reinterpret_cast<irr::ode::CIrrOdeWorld *>(pNode);
    return;
  }

  irr::core::list<irr::scene::ISceneNode *> children = pNode->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it = children.begin(); it != children.end(); it++) initWorld(*it);
}

void CControlReceiver::updateVehicle() {
  switch (m_pActive->getType()) {
    case eStateCar  : printf("control car\n"  ); m_eVehicle = eControlCar  ; break;
    case eStatePlane: printf("control plane\n"); m_eVehicle = eControlPlane; break;
    case eStateHeli : printf("control heli\n" ); m_eVehicle = eControlHeli ; break;
    case eStateTank : printf("control tank\n" ); m_eVehicle = eControlTank ; break;
    default         : printf("control none\n" ); m_eVehicle = eControlNone ; break;
  }
}

CControlReceiver::CControlReceiver(irr::IrrlichtDevice *pDevice, irr::ode::IIrrOdeEventQueue *pQueue, irrklang::ISoundEngine *pSndEngine) {
  m_pDevice    = pDevice;
  m_pQueue     = pQueue;
  m_iNode      = 0;
  m_iCount     = 0;
  m_eVehicle   = eControlNone;
  m_pSndEngine = pSndEngine;

  m_pController = new CIrrCC(m_pDevice);
  m_pController->setSetsCanConflict(false);
  m_pController->setAllowFKeys(false);
  m_pController->setAllowMouse(false);

  CConfigFileManager::getSharedInstance()->addReader(m_pController);
  CConfigFileManager::getSharedInstance()->addWriter(m_pController);

  m_bSwitchToMenu = false;

  initControls();

  CConfigFileManager::getSharedInstance()->loadConfig(m_pDevice,"../../data/irrOdeCarControls.xml");
  m_pMenu = new CMenu(m_pDevice); m_aStates.push_back(m_pMenu);
  m_pActive = m_pMenu;

  m_pCtrlDialog = new CController(m_pDevice,m_pController); m_aStates.push_back(m_pCtrlDialog);

  m_pCamCtrl = new CCameraController(m_pDevice, m_pSndEngine, m_pController, irr::ode::CIrrOdeManager::getSharedInstance());
  m_pCamCtrl->setCtrl(m_iCtrls[3]);

  irr::core::dimension2d<irr::u32> cSize=m_pDevice->getVideoDriver()->getScreenSize();
  irr::core::rect<irr::s32> cRect=irr::core::rect<irr::s32>(cSize.Width/2-100,25,cSize.Width/2+100,40);

  m_pRecorder  = NULL;
  m_pRecording = m_pDevice->getGUIEnvironment()->addStaticText(L"Recording"       , irr::core::rect<irr::s32>(cSize.Width/2 - 100, 10, cSize.Width/2 + 100, 30), true); m_pRecording->setVisible(false);
  m_pSaveFile  = m_pDevice->getGUIEnvironment()->addStaticText(L"Saving Replay...", irr::core::rect<irr::s32>(cSize.Width/2 - 100, 35, cSize.Width/2 + 100, 55), true); m_pSaveFile ->setVisible(false);

  m_pSaveFile=m_pDevice->getGUIEnvironment()->addStaticText(L"Replay file saved.",cRect,true,true,0,-1,true);
  m_pSaveFile->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);
  m_pSaveFile->setVisible(false);

  initWorld(m_pDevice->getSceneManager()->getRootSceneNode());

  m_iActive = 0;
}

CControlReceiver::~CControlReceiver() {
  CConfigFileManager::getSharedInstance()->writeConfig(m_pDevice,"../../data/irrOdeCarControls.xml");
}

void CControlReceiver::setControlledVehicle(irr::s32 iType, irr::s32 iNode) {
  switch (iType) {
    case -1: m_eVehicle = eControlNone ; break;
    case  0: m_eVehicle = eControlCar  ; break;
    case  1: m_eVehicle = eControlPlane; break;
    case  2: m_eVehicle = eControlHeli ; break;
    case  3: m_eVehicle = eControlTank ; break;
    default: m_eVehicle = eControlNone ; printf("ERROR: unknown vehicle code (%i)\n", iType);
  }

  m_iNode = iNode;
}

void CControlReceiver::createMenu(irr::u32 iCars, irr::u32 iPlanes, irr::u32 iHelis, irr::u32 iTanks, bool bRearCam) {
  CVehicle *pVehicles = new CVehicle(m_pDevice, iCars, iPlanes, iHelis, iTanks, m_pWorld, bRearCam);

  irr::core::list<CIrrOdeCarState *> lVehicles = pVehicles->getVehicles();
  irr::core::list<CIrrOdeCarState *>::Iterator it;

  for (it = lVehicles.begin(); it!=lVehicles.end(); it++) {
    m_pMenu->addButtonForState(*it);
    m_aStates.push_back(*it);
  }
}

void CControlReceiver::start() {
  m_pActive->activate();
  updateVehicle();
}

void CControlReceiver::update() {
  m_pCamCtrl->update();

  //call the update method of the currently active state
  irr::u32 iSwitch=m_pActive->update();

  if (m_bSwitchToMenu) {
    iSwitch = 1;
    m_bSwitchToMenu = false;
  }

  //change the active state if wished, i.e. a value other than zero was returned
  if (iSwitch) {
    iSwitch--;
    m_pActive->deactivate();
    m_pActive=m_aStates[iSwitch];
    m_pActive->activate();
    updateVehicle();
    m_pCamCtrl->setTarget(m_pActive->getBody());
    iSwitch=0;
  }

  switch (m_eVehicle) {
    case eControlCar:
      break;

    case eControlPlane:
      break;

    case eControlHeli:
      break;

    case eControlTank:
      break;

    case eControlNone:
      break;
  }
  if (m_pSaveFile->isVisible() && m_pDevice->getTimer()->getTime()>m_iCount) m_pSaveFile->setVisible(false);
}

CIrrCC *CControlReceiver::getController() {
  return m_pController;
}

bool CControlReceiver::OnEvent(const irr::SEvent &event) {
  m_pController->OnEvent(event);
  m_pCamCtrl   ->OnEvent(event);

  if (m_pActive == m_pMenu) m_pMenu->OnEvent(event);

  if (event.EventType==irr::EET_KEY_INPUT_EVENT) {
    if (event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case irr::KEY_F2:
          if (m_pRecorder==NULL) {
            printf("starting recording...\n");
            m_pRecorder=new irr::ode::CIrrOdeRecorder(m_pDevice,"IrrOdeCar");
            m_pRecorder->startRecording();
            m_pRecording->setVisible(true);
          }
          else {
            printf("stopping recording...\n");
            m_pRecorder->stopRecording();
            m_pRecorder->saveRecording("../../data/replay/car.rec");
            delete m_pRecorder;
            m_pRecorder=NULL;
            m_pRecording->setVisible(false);
            m_pSaveFile->setVisible(true);
            m_iCount=m_pDevice->getTimer()->getTime()+3000;
          }
          break;

        //if TAB is pressed the program shall return to the vehicle selection menu
        case irr::KEY_TAB:
          m_bSwitchToMenu=true;
          return true;
          break;

        default:
          break;
      }
    }
  }
  return false;
}


void CControlReceiver::removeFromScene(const irr::c8 *sName, irr::scene::ISceneManager *smgr) {
  irr::scene::ISceneNode *pNode=smgr->getSceneNodeFromName(sName);
  if (pNode!=NULL) {
    m_pWorld->removeTreeFromPhysics(pNode);
    irr::s32 iNodeId=pNode->getID();
    pNode->remove();

    irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
    irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
  }
}

void CControlReceiver::removeFromScene(irr::scene::ISceneNode *pNode) {
  m_pWorld->removeTreeFromPhysics(pNode);
  irr::s32 iNodeId=pNode->getID();
  pNode->remove();

  irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
}

  #include <irrlicht.h>
  #include <CControlReceiver.h>
  #include <CControlEvents.h>
  #include <irrCC.h>
  #include <CConfigFileManager.h>

  #include <CMenu.h>
  #include <CVehicle.h>
  #include <CController.h>
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
  m_iCtrls[3][eCameraNearFocus ] = m_pController->addItem(3, irr::core::stringw("Near Focus"                ), irr::KEY_KEY_N, CIrrCC::eCtrlButton);

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

CControlReceiver::CControlReceiver(irr::IrrlichtDevice *pDevice, irr::ode::IIrrOdeEventQueue *pQueue, irrklang::ISoundEngine *pSndEngine, irr::f32 fVrAr) {
  m_pDevice     = pDevice;
  m_pInputQueue = pQueue;
  m_iNode       = 0;
  m_iCount      = 0;
  m_iClient     = 0;
  m_iLastEvent  = 0;
  m_eVehicle    = eControlNone;
  m_pSndEngine  = pSndEngine;

  m_pController = new CIrrCC(m_pDevice);
  m_pController->setSetsCanConflict(false);
  m_pController->setAllowFKeys(false);
  m_pController->setAllowMouse(false);

  m_bShowFps = false;

  CConfigFileManager::getSharedInstance()->addReader(m_pController);
  CConfigFileManager::getSharedInstance()->addWriter(m_pController);

  initControls();

  CConfigFileManager::getSharedInstance()->loadConfig(m_pDevice,"../../data/irrOdeCarControls.xml");
  m_pMenu = new CMenu(m_pDevice, m_pInputQueue); m_aStates.push_back(m_pMenu);
  m_pActive = m_pMenu;

  m_pCtrlDialog = new CController(m_pDevice,m_pController); m_aStates.push_back(m_pCtrlDialog);

  m_pCamCtrl = new CCameraController(m_pDevice, m_pSndEngine, m_pController, irr::ode::CIrrOdeManager::getSharedInstance(), fVrAr);
  m_pCamCtrl->setCtrl(m_iCtrls[3]);

  irr::core::dimension2d<irr::u32> cSize=m_pDevice->getVideoDriver()->getScreenSize();
  irr::core::rect<irr::s32> cRect=irr::core::rect<irr::s32>(cSize.Width/2-100,25,cSize.Width/2+100,40);

  m_pRecorder  = NULL;
  m_pRecording = m_pDevice->getGUIEnvironment()->addStaticText(L"Recording"       , irr::core::rect<irr::s32>(cSize.Width/2 - 100, 10, cSize.Width/2 + 100, 30), true); m_pRecording->setVisible(false);
  m_pSaveFile  = m_pDevice->getGUIEnvironment()->addStaticText(L"Saving Replay...", irr::core::rect<irr::s32>(cSize.Width/2 - 100, 35, cSize.Width/2 + 100, 55), true); m_pSaveFile ->setVisible(false);

  m_pSaveFile=m_pDevice->getGUIEnvironment()->addStaticText(L"Replay file saved.",cRect,true,true,0,-1,true);
  m_pSaveFile->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);
  m_pSaveFile->setVisible(false);

  m_pTimer = m_pDevice->getTimer();

  initWorld(m_pDevice->getSceneManager()->getRootSceneNode());

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CControlReceiver::~CControlReceiver() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
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
  CVehicle *pVehicles = new CVehicle(m_pDevice, iCars, iPlanes, iHelis, iTanks, m_pWorld, bRearCam, m_pInputQueue);

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

void CControlReceiver::switchToState(irr::s32 iNewState) {
  if (iNewState > 1000) {
    for (irr::u32 i = 0; i < m_aStates.size(); i++) {
      CIrrOdeCarState *p = m_aStates[i];
      if (p->getBody() != NULL && p->getBody()->getID() == iNewState) {
        iNewState = i;
        printf("switch to state %i\n", iNewState);
        m_iLastEvent = m_pTimer->getTime();
        break;
      }
    }
  }

  if (iNewState > 1000) {
    printf("no vehicle found with ID %i\n", iNewState);
    return;
  }

  m_pActive->deactivate();
  m_pActive=m_aStates[iNewState];
  m_pActive->activate();
  updateVehicle();
  m_pCamCtrl->setTarget(m_pActive->getBody());
  m_iNode = m_pActive->getBody() != NULL ? m_pActive->getBody()->getID() : -1;
}

void CControlReceiver::update() {
  m_pCamCtrl->setLeft();
  m_pCamCtrl->updateRearView();

  //call the update method of the currently active state
  irr::u32 iSelect = m_pActive->update();

  if (iSelect != 0) {
    printf("Select: %i\n", iSelect);
    switchToState(iSelect - 1);
  }

  m_pActive->drawSpecifics();

  if (m_pTimer->getTime() - m_iLastEvent >= 16) {
    switch (m_eVehicle) {
      case eControlCar: {
          irr::f32 fThrottle = m_pController->get(m_iCtrls[0][eCarForeward]),
                   fSteer    = m_pController->get(m_iCtrls[0][eCarLeft    ]);

          bool bDifferential = m_pController->get(m_iCtrls[0][eCarDifferential]) != 0.0f,
               bShiftUp      = m_pController->get(m_iCtrls[0][eCarShiftUp     ]) != 0.0f,
               bShiftDown    = m_pController->get(m_iCtrls[0][eCarShiftDown   ]) != 0.0f,
               bFlip         = m_pController->get(m_iCtrls[0][eCarFlip        ]) != 0.0f,
               bBoost        = m_pController->get(m_iCtrls[0][eCarBoost       ]) != 0.0f,
               bAdapt        = m_pController->get(m_iCtrls[0][eCarAdapSteer   ]) != 0.0f;

          m_pController->set(m_iCtrls[0][eCarDifferential], 0.0f);
          m_pController->set(m_iCtrls[0][eCarShiftUp     ], 0.0f);
          m_pController->set(m_iCtrls[0][eCarShiftDown   ], 0.0f);
          m_pController->set(m_iCtrls[0][eCarAdapSteer   ], 0.0f);

          CCarControls *pCar = new CCarControls(fThrottle, fSteer);

          pCar->setDifferential (bDifferential);
          pCar->setShifDown     (bShiftDown   );
          pCar->setShiftUp      (bShiftUp     );
          pCar->setBoost        (bBoost       );
          pCar->setFlip         (bFlip        );
          pCar->setAdaptiveSteer(bAdapt       );

          pCar->setNode(m_iNode);
          pCar->setClient(0);

          m_pInputQueue->postEvent(pCar);
          m_iLastEvent = m_pTimer->getTime();
        }
        break;

      case eControlPlane:
      case eControlHeli: {
          irr::f32 fPower = m_pController->get(m_iCtrls[2][eAeroPowerUp  ]),
                   fPitch = m_pController->get(m_iCtrls[2][eAeroPitchUp  ]),
                   fRoll  = m_pController->get(m_iCtrls[2][eAeroRollLeft ]),
                   fYaw   = m_pController->get(m_iCtrls[2][eAeroYawRight ]);

          bool bPowerZero = m_pController->get(m_iCtrls[2][eAeroPowerZero    ]),
               bSelTarget = m_pController->get(m_iCtrls[2][eAeroSelectTarget ]),
               bFirePrim  = m_pController->get(m_iCtrls[2][eAeroFirePrimary  ]),
               bFireSec   = m_pController->get(m_iCtrls[2][eAeroFireSecondary]),
               bFlip      = m_pController->get(m_iCtrls[2][eAeroFlip         ]),
               bBrake     = m_pController->get(m_iCtrls[2][eAeroBrake        ]),
               bAutoPilot = m_pController->get(m_iCtrls[2][eAeroAutoPilot    ]);

          m_pController->set(m_iCtrls[2][eAeroSelectTarget ], 0.0f);
          m_pController->set(m_iCtrls[2][eAeroFirePrimary  ], 0.0f);
          m_pController->set(m_iCtrls[2][eAeroFireSecondary], 0.0f);
          m_pController->set(m_iCtrls[2][eAeroAutoPilot    ], 0.0f);

          CPlaneControls *pPlane = new CPlaneControls(fYaw, fPitch, fRoll, fPower);

          pPlane->setPowerZero    (bPowerZero);
          pPlane->setSelectTarget (bSelTarget);
          pPlane->setFirePrimary  (bFirePrim );
          pPlane->setFireSecondary(bFireSec  );
          pPlane->setFlip         (bFlip     );
          pPlane->setBrake        (bBrake    );
          pPlane->setAutoPilot    (bAutoPilot);

          if (bPowerZero) m_pController->set(m_iCtrls[2][eAeroPowerUp], 0.0f);

          pPlane->setNode(m_iNode);
          pPlane->setClient(0);

          m_pInputQueue->postEvent(pPlane);
          m_iLastEvent = m_pTimer->getTime();
        }
        break;

      case eControlTank: {
          irr::f32 fThrottle   = m_pController->get(m_iCtrls[1][eTankForeward  ]),
                   fSteer      = m_pController->get(m_iCtrls[1][eTankLeft      ]),
                   fCannonLeft = m_pController->get(m_iCtrls[1][eTankCannonLeft]),
                   fCannonUp   = m_pController->get(m_iCtrls[1][eTankCannonUp  ]);

          bool bFire          = m_pController->get(m_iCtrls[1][eTankFire         ]),
               bFlip          = m_pController->get(m_iCtrls[1][eTankFlip         ]),
               bFastCollision = m_pController->get(m_iCtrls[1][eTankFastCollision]);

          CTankControls *pTank = new CTankControls(fThrottle, fSteer, fCannonLeft, fCannonUp);

          pTank->setFire         (bFire         );
          pTank->setFlip         (bFlip         );
          pTank->setFastCollision(bFastCollision);

          if (bFastCollision) m_pController->set(m_iCtrls[1][eTankFastCollision], 0.0f);
          if (bFire         ) m_pController->set(m_iCtrls[1][eTankFire         ], 0.0f);

          pTank->setNode(m_iNode);
          pTank->setClient(0);

          m_pInputQueue->postEvent(pTank);
          m_iLastEvent = m_pTimer->getTime();
        }
        break;

      case eControlNone:
        break;
    }
  }
  if (m_pSaveFile->isVisible() && m_pDevice->getTimer()->getTime()>m_iCount) m_pSaveFile->setVisible(false);
}

CIrrCC *CControlReceiver::getController() {
  return m_pController;
}

bool CControlReceiver::OnEvent(const irr::SEvent &event) {
  m_pController->OnEvent(event);
  m_pCamCtrl   ->OnEvent(event);

  if (m_pActive == m_pMenu      ) m_pMenu      ->OnEvent(event);
  if (m_pActive == m_pCtrlDialog) m_pCtrlDialog->OnEvent(event);;

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

        case irr::KEY_F1:
          m_bShowFps = !m_bShowFps;
          break;

        case irr::KEY_F3:
          m_pCamCtrl->set3d(!m_pCamCtrl->is3dEnabled());
          printf("3d mode %s\n", m_pCamCtrl->is3dEnabled() ? "activated" : "disabled");
          break;

        case irr::KEY_F4:
          m_pCamCtrl->setVr(!m_pCamCtrl->isVrEnabled());
          break;

        //if TAB is pressed the program shall return to the vehicle selection menu
        case irr::KEY_TAB: {
            if (m_iNode != -1) {
              printf("requesting vehicle leave from server (%i).\n", m_iNode);
              CLeaveVehicle *p = new CLeaveVehicle((irr::u8)0);
              p->setClient(0);
              p->setNode(m_iNode);
              irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
            }
            else switchToState(0);
          }
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

void CControlReceiver::drawSpecifics() {
  if (m_pActive != NULL) m_pActive->drawSpecifics();
}

bool CControlReceiver::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == eCtrlMsgVehicleApproved) {
    CVehicleApproved *p = reinterpret_cast<CVehicleApproved *>(pEvent);
    if (p->getClient() == 0) {
      printf("Got Vehicle %i\n", p->getNode());
      switchToState(p->getNode());
    }
  }

  if (pEvent->getType() == eCtrlMsgLeaveVehicle) {
    CLeaveVehicle *p = reinterpret_cast<CLeaveVehicle *>(pEvent);
    if (p->getClient() == m_iClient) {
      if (p->getAnswer() == 1) {
        printf("OK ... leaving vehicle.\n");
        switchToState(0);
      }
    }
  }
  return false;
}

bool CControlReceiver::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == eCtrlMsgVehicleApproved || pEvent->getType() == eCtrlMsgLeaveVehicle;
}

void CControlReceiver::connect() {
  CLoginMessage *p = new CLoginMessage();
  m_pInputQueue->postEvent(p);
}

bool CControlReceiver::is3dEnabled() {
  return m_pCamCtrl->is3dEnabled();
}

bool CControlReceiver::isVrEnabled() {
  return m_pCamCtrl->isVrEnabled();
}

void CControlReceiver::updateCamera() {
  m_pCamCtrl->update();
}

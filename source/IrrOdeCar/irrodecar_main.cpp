  #include <irrlicht.h>
  #include <IrrODE.h>
  #include <CSettings.h>

  #include "CCar.h"
  #include "CTank.h"
  #include "CMenu.h"
  #include "CHeli.h"
  #include "CPlane.h"
  #include "CController.h"
  #include "CIrrOdeCarState.h"
  #include "CConfigFileManager.h"
  #include "irrCC.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

int main(int argc, char** argv) {
  //First thing to do: show the graphics options dialog to let the user choose the graphics options
  CSettings *pSettings=new CSettings("../../data/irrOdeMarble_setup.xml","../../data/irrOdeCarSettings.xml",L"irrOdeCar - Graphics Setup",SColor(0x00,0x21,0xAD,0x10));

  u32 iRet=pSettings->run();
  if (iRet==2) {
    delete pSettings;
    return 0;
  }

  //now create the Irrlicht device from the chosen options
  IrrlichtDevice *device=pSettings->createDeviceFromSettings();
  device->setWindowCaption(L"IrrODE car demo");
  delete pSettings;

  CIrrCC *pController=new CIrrCC(device);
  pController->setSetsCanConflict(false);
  pController->setAllowFKeys(false);
  pController->setAllowMouse(true);
  CConfigFileManager::getSharedInstance()->addReader(pController);
  CConfigFileManager::getSharedInstance()->addWriter(pController);

  u32 iCtrls[4][16];  //an array for all the controls we are going to define

  //first up: the car's controls
  iCtrls[0][eCarForeward           ]=pController->addItem(0,stringw(L"Foreward"             ),KEY_UP    ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarBackward           ]=pController->addItem(0,stringw(L"Backward"             ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarLeft               ]=pController->addItem(0,stringw(L"Left"                 ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarRight              ]=pController->addItem(0,stringw(L"Right"                ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarBrake              ]=pController->addItem(0,stringw(L"Brake"                ),KEY_SPACE ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarBoost              ]=pController->addItem(0,stringw(L"Boost"                ),KEY_KEY_B ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarToggleAdaptiveSteer]=pController->addItem(0,stringw(L"Toggle adaptive Steer"),KEY_KEY_X ,CIrrCC::eCtrlToggleButton);
  iCtrls[0][eCarFlip               ]=pController->addItem(0,stringw(L"Flip"                 ),KEY_RETURN,CIrrCC::eCtrlToggleButton);

  //we need two axes for the car: acceleration and steering
  pController->createAxis(iCtrls[0][eCarForeward],iCtrls[0][eCarBackward]);
  pController->createAxis(iCtrls[0][eCarLeft    ],iCtrls[0][eCarRight   ]);

  //next up: the tank
  iCtrls[1][eTankForeward     ]=pController->addItem(1,stringw(L"Foreward"         ),KEY_UP    ,CIrrCC::eCtrlAxis);
  iCtrls[1][eTankBackward     ]=pController->addItem(1,stringw(L"Backward"         ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
  iCtrls[1][eTankLeft         ]=pController->addItem(1,stringw(L"Left"             ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
  iCtrls[1][eTankRight        ]=pController->addItem(1,stringw(L"Right"            ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
  iCtrls[1][eTankCannonLeft   ]=pController->addItem(1,stringw(L"Cannon Left"      ),KEY_KEY_A ,CIrrCC::eCtrlAxis);
  iCtrls[1][eTankCannonRight  ]=pController->addItem(1,stringw(L"Cannon Right"     ),KEY_KEY_D ,CIrrCC::eCtrlAxis);
  iCtrls[1][eTankCannonUp     ]=pController->addItem(1,stringw(L"Cannon Up"        ),KEY_KEY_W ,CIrrCC::eCtrlAxis);
  iCtrls[1][eTankCannonDown   ]=pController->addItem(1,stringw(L"Cannon Down"      ),KEY_KEY_S ,CIrrCC::eCtrlAxis);
  iCtrls[1][eTankFire         ]=pController->addItem(1,stringw(L"Fire"             ),KEY_SPACE ,CIrrCC::eCtrlToggleButton);
  iCtrls[1][eTankCamTurret    ]=pController->addItem(1,stringw(L"Toggle Turret Cam"),KEY_KEY_T ,CIrrCC::eCtrlToggleButton);
  iCtrls[1][eTankCamBullet    ]=pController->addItem(1,stringw(L"Toggle Bullet Cam"),KEY_KEY_B ,CIrrCC::eCtrlToggleButton);
  iCtrls[1][eTankFlip         ]=pController->addItem(1,stringw(L"Flip"             ),KEY_RETURN,CIrrCC::eCtrlToggleButton);
  iCtrls[1][eTankFastCollision]=pController->addItem(1,stringw(L"Fast Collision"   ),KEY_KEY_F ,CIrrCC::eCtrlToggleButton);

  //this time we're gonna create four axes: acceleration, steering, turret movement and cannon angle
  pController->createAxis(iCtrls[1][eTankForeward  ],iCtrls[1][eTankBackward   ]);
  pController->createAxis(iCtrls[1][eTankLeft      ],iCtrls[1][eTankRight      ]);
  pController->createAxis(iCtrls[1][eTankCannonUp  ],iCtrls[1][eTankCannonDown ]);
  pController->createAxis(iCtrls[1][eTankCannonLeft],iCtrls[1][eTankCannonRight]);

  //3rd in row: the helicopter
  iCtrls[2][eHeliYawLeft  ]=pController->addItem(2,stringw(L"Yaw Left"    ),KEY_KEY_A,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliYawRight ]=pController->addItem(2,stringw(L"Yaw Right"   ),KEY_KEY_D,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliRollLeft ]=pController->addItem(2,stringw(L"Roll Left"   ),KEY_LEFT ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliRollRight]=pController->addItem(2,stringw(L"Roll Right"  ),KEY_RIGHT,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliPitchUp  ]=pController->addItem(2,stringw(L"Pitch Up"    ),KEY_DOWN ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliPitchDown]=pController->addItem(2,stringw(L"Pitch Down"  ),KEY_UP   ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliPowerUp  ]=pController->addItem(2,stringw(L"Power Plus"  ),KEY_KEY_W,CIrrCC::eCtrlFader);
  iCtrls[2][eHeliPowerDown]=pController->addItem(2,stringw(L"Power Minus" ),KEY_KEY_S,CIrrCC::eCtrlFader);
  iCtrls[2][eHeliFire     ]=pController->addItem(2,stringw(L"Fire Missile"),KEY_SPACE,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eHeliToggleCam]=pController->addItem(2,stringw(L"Toggle Cam"  ),KEY_KEY_C,CIrrCC::eCtrlToggleButton);

  //the helicopter gets three axes: pitch, roll and yaw...
  pController->createAxis(iCtrls[2][eHeliYawLeft ],iCtrls[2][eHeliYawRight ]);
  pController->createAxis(iCtrls[2][eHeliRollLeft],iCtrls[2][eHeliRollRight]);
  pController->createAxis(iCtrls[2][eHeliPitchUp ],iCtrls[2][eHeliPitchDown]);

  //...and one fader for the helicopter's thrust control
  pController->createFader(iCtrls[2][eHeliPowerUp],iCtrls[2][eHeliPowerDown],10,0.01f);

  //last but not least: the airplane. This one gets the most controls
  iCtrls[3][ePlaneYawLeft  ]=pController->addItem(3,stringw(L"Yaw Left"   ),KEY_KEY_A,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlaneYawRight ]=pController->addItem(3,stringw(L"Yaw Right"  ),KEY_KEY_D,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlaneRollLeft ]=pController->addItem(3,stringw(L"Roll Left"  ),KEY_LEFT ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlaneRollRight]=pController->addItem(3,stringw(L"Roll Right" ),KEY_RIGHT,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlanePitchUp  ]=pController->addItem(3,stringw(L"Pitch Up"   ),KEY_DOWN ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlanePitchDown]=pController->addItem(3,stringw(L"Pitch Down" ),KEY_UP   ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlanePowerUp  ]=pController->addItem(3,stringw(L"Power Plus" ),KEY_KEY_W,CIrrCC::eCtrlFader);
  iCtrls[3][ePlanePowerDown]=pController->addItem(3,stringw(L"Power Minus"),KEY_KEY_S,CIrrCC::eCtrlFader);
  iCtrls[3][ePlanePowerZero]=pController->addItem(3,stringw(L"Power Zero" ),KEY_KEY_Z,CIrrCC::eCtrlButton);
  iCtrls[3][ePlaneBackview ]=pController->addItem(3,stringw(L"Backview"   ),KEY_KEY_R,CIrrCC::eCtrlButton);
  iCtrls[3][ePlaneFire     ]=pController->addItem(3,stringw(L"Drop Bomb"  ),KEY_SPACE,CIrrCC::eCtrlToggleButton);
  iCtrls[3][ePlaneToggleCam]=pController->addItem(3,stringw(L"Toggle Cam" ),KEY_KEY_C,CIrrCC::eCtrlToggleButton );
  iCtrls[3][ePlaneBrake    ]=pController->addItem(3,stringw(L"Wheel Brake"),KEY_KEY_B,CIrrCC::eCtrlButton);

  //just like the helicopter the plane gets three axes (pitch, roll, yaw)...
  pController->createAxis(iCtrls[3][ePlaneYawRight],iCtrls[3][ePlaneYawLeft  ]);
  pController->createAxis(iCtrls[3][ePlanePitchUp ],iCtrls[3][ePlanePitchDown]);
  pController->createAxis(iCtrls[3][ePlaneRollLeft],iCtrls[3][ePlaneRollRight]);

  //...and one fader for the engine control
  pController->createFader(iCtrls[3][ePlanePowerUp],iCtrls[3][ePlanePowerDown],10,0.01f);

  IVideoDriver *driver = device->getVideoDriver();
  ISceneManager *smgr = device->getSceneManager();
  IGUIEnvironment *guienv = device->getGUIEnvironment();

  //let's add a "please stand by" image filling the complete window / screen ...
  dimension2du cScreenSize=device->getVideoDriver()->getScreenSize();

  IGUIImage *pImg=guienv->addImage(rect<s32>(0,0,cScreenSize.Width,cScreenSize.Height));
  pImg->setScaleImage(true);
  pImg->setImage(driver->getTexture("../../data/standby.png"));

  dimension2d<u32> cSize=driver->getScreenSize();

  driver->beginScene(true, true, SColor(0,200,200,200));
  guienv->drawAll();
  driver->endScene();

  //... and remove it again
  pImg->remove();

  //register the IrrOde scene node factory
  CIrrOdeSceneNodeFactory cFactory(smgr);
  smgr->registerSceneNodeFactory(&cFactory);

  //init the ODE
  CIrrOdeManager::getSharedInstance()->initODE();

  //set IrrODE's timer
  ITimer *pTimer=device->getTimer();
  CIrrOdeManager::getSharedInstance()->setTimer(pTimer);

  //load the scene
  smgr->loadScene("../../data/IrrOdeCar.xml");

  //Initialize the camera
  vector3df camPos=vector3df(1750,99,2500);
  ICameraSceneNode *cam=smgr->addCameraSceneNode();
  cam->setPosition(camPos);

  //modify the textures of the car segment and the tank segment to
  IAnimatedMeshSceneNode *pNode=(IAnimatedMeshSceneNode *)smgr->getSceneNodeFromName("car_segment");
  if (pNode) pNode->getMaterial(0).getTextureMatrix(0).setTextureScale(50.0f,50.0f);
  pNode=(IAnimatedMeshSceneNode *)smgr->getSceneNodeFromName("tank_segment");
  if (pNode) pNode->getMaterial(0).getTextureMatrix(0).setTextureScale(50.0f,50.0f);

  int lastFPS=-1;
  while (pTimer->isStopped()) pTimer->start();

  IGUIStaticText *pFps=guienv->addStaticText(L"Hello World!",rect<s32>(position2di(cSize.Width-155,5),dimension2di(150,22)),true);
  pFps->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
  pFps->setDrawBackground(true);

  //create the necessary state objects
  CIrrOdeCarState *pActive=NULL;
  array<CIrrOdeCarState *> aStates;

  CMenu       *theMenu   =new CMenu      (device             ,pController); aStates.push_back(theMenu );
  CCar        *theCar    =new CCar       (device,"theCar"    ,pController); aStates.push_back(theCar  );
  CCar        *theCar2   =new CCar       (device,"simpleCar" ,pController); aStates.push_back(theCar2 );
  CCar        *theCar3   =new CCar       (device,"complexCar",pController); aStates.push_back(theCar3 );
  CTank       *theTank   =new CTank      (device             ,pController); aStates.push_back(theTank );
  CHeli       *theHeli   =new CHeli      (device             ,pController); aStates.push_back(theHeli );
  CPlane      *thePlane  =new CPlane     (device             ,pController); aStates.push_back(thePlane);
  CController *theCtrl   =new CController(device             ,pController); aStates.push_back(theCtrl );

  theCar  ->setCtrl((const u32 *)iCtrls[0]);
  theCar2 ->setCtrl((const u32 *)iCtrls[0]);
  theCar3 ->setCtrl((const u32 *)iCtrls[0]);
  theTank ->setCtrl((const u32 *)iCtrls[1]);
  theHeli ->setCtrl((const u32 *)iCtrls[2]);
  thePlane->setCtrl((const u32 *)iCtrls[3]);

  //phyiscs initialization
  CIrrOdeManager::getSharedInstance()->initPhysics();

  //set the menu state to active
  pActive=theMenu;
  pActive->activate();

  CConfigFileManager::getSharedInstance()->loadConfig(device,"../../data/irrOdeCarControls.xml");

  //let's run the loop
  while(device->run()) {
    //step the simulation
    CIrrOdeManager::getSharedInstance()->step();

    //call the update method of the currently active state
    u32 iSwitch=pActive->update();

    //change the active state if wished, i.e. a value other than zero was returned
    if (iSwitch) {
      iSwitch--;
      pActive->deactivate();
      pActive=aStates[iSwitch];
      pActive->activate();
      iSwitch=0;
    }

    //now for the normal Irrlicht stuff ... begin, draw and end scene and update window caption
    driver->beginScene(true, true, SColor(0,200,200,200));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
    int fps = driver->getFPS();

    if (lastFPS != fps) {
      core::stringw str = L"Irrlicht Engine - IrrODE Car Demo [";
      str += driver->getName();
      str += "] FPS:";
      str += fps;

      device->setWindowCaption(str.c_str());
      lastFPS = fps;

      wchar_t s[0xFF];
      swprintf(s,0xFE,L"FPS: %i\nPrimives Drawn: %i",lastFPS,driver->getPrimitiveCountDrawn());
      pFps->setText(s);
    }
  }

  CConfigFileManager::getSharedInstance()->writeConfig(device,"../../data/irrOdeCarControls.xml");

  //drop the world so it is destroyed
  device->drop();

  //and now some more cleanup...
  delete theMenu;
  delete theCar;
  delete theTank;
  delete theHeli;
  delete thePlane;

  delete pController;

  return 0;
}


  #include <irrlicht.h>
  #include <IrrODE.h>
  #include <CSettings.h>
  #include <CAdvancedParticleSystemNode.h>

  #include "CCar.h"
  #include "CTank.h"
  #include "CMenu.h"
  #include "CHeli.h"
  #include "CPlane.h"
  #include "CController.h"
  #include "CIrrOdeCarState.h"
  #include "CConfigFileManager.h"
  #include "CProgressBar.h"
  #include "irrCC.h"
  #include "CProjectile.h"
  #include "CCustomEventReceiver.h"
  #include <CRandomForestNode.h>
  #include <CMeshCombiner.h>
  #include <CCockpitPlane.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CProgress : public irr::ode::IIrrOdeEventListener {
  protected:
    IrrlichtDevice *m_pDevice;
    IVideoDriver *m_pDriver;
    IGUIEnvironment *m_pGuienv;

    IGUIImage *m_pImg;
    IGUIStaticText *m_pText;
    CProgressBar *m_pBar;

  public:
    CProgress(IrrlichtDevice *pDevice) {
      m_pDevice=pDevice;
      m_pDriver=pDevice->getVideoDriver();
      m_pGuienv=pDevice->getGUIEnvironment();

      //let's add a "please stand by" image filling the complete window / screen
      dimension2du cScreenSize=m_pDevice->getVideoDriver()->getScreenSize();

      m_pImg=m_pGuienv->addImage(rect<s32>(0,0,cScreenSize.Width,cScreenSize.Height));
      m_pImg->setScaleImage(true);
      m_pImg->setImage(m_pDriver->getTexture("../../data/standby.png"));

      m_pBar=new CProgressBar(m_pGuienv,rect<s32>(position2di(cScreenSize.Width/2-150,cScreenSize.Height-60),dimension2di(300,30)),-1,NULL);
      m_pText=m_pGuienv->addStaticText(L"",rect<s32>(position2di(cScreenSize.Width/2-150,cScreenSize.Height-60),dimension2di(300,30)),true);
      m_pText->setBackgroundColor(SColor(192,192,192,192));
      m_pText->setDrawBackground(true);
      m_pText->setTextAlignment(gui::EGUIA_CENTER,gui::EGUIA_CENTER);

      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
    }

    virtual ~CProgress() {
      m_pImg->remove();
      m_pText->remove();
      m_pBar->remove();
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
    }

    virtual bool onEvent(IIrrOdeEvent *pEvent) {
      bool bRet=false;

      if (pEvent->getType()==irr::ode::eIrrOdeEventProgress) {
        irr::ode::CIrrOdeEventProgress *pEvt=reinterpret_cast<irr::ode::CIrrOdeEventProgress *>(pEvent);
        wchar_t s[0xFF];
        swprintf(s,0xFF,L"Physics Initialization: %i / %i",pEvt->getCurrent(),pEvt->getCount());
        m_pText->setText(s);
        m_pBar->setProgress(100*pEvt->getCurrent()/pEvt->getCount());

        m_pDriver->beginScene(true, true, SColor(0,200,200,200));
        m_pGuienv->drawAll();
        m_pDriver->endScene();
      }

      return bRet;
    }

    virtual bool handlesEvent(IIrrOdeEvent *pEvent) {
      return pEvent->getType()==irr::ode::eIrrOdeEventProgress;
    }

};

void removeFromScene(const c8 *sName, ISceneManager *smgr) {
  ISceneNode *pNode=smgr->getSceneNodeFromName(sName);
  if (pNode!=NULL) {
    CIrrOdeManager::getSharedInstance()->removeTreeFromPhysics(pNode);
    s32 iNodeId=pNode->getID();
    pNode->remove();

    irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
    CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
  }
}

void removeFromScene(ISceneNode *pNode) {
  CIrrOdeManager::getSharedInstance()->removeTreeFromPhysics(pNode);
  s32 iNodeId=pNode->getID();
  pNode->remove();

  irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
  CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
}

void fillBodyList(irr::core::list<ISceneNode *> &aPlanes, ISceneNode *pNode, const c8 *sClassName, u32 iMax) {
  if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
    irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
    if (p->getOdeClassname().equals_ignore_case(sClassName)) {
      printf("%s found (%i)\n",sClassName,aPlanes.size());
      if (aPlanes.size()<iMax)
        aPlanes.push_back(pNode);
      else {
        removeFromScene(pNode);
        return;
      }
    }
  }

  irr::core::list<ISceneNode *> children=pNode->getChildren();
  irr::core::list<ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) fillBodyList(aPlanes,*it,sClassName,iMax);
}

int main(int argc, char** argv) {
  //First thing to do: show the graphics options dialog to let the user choose the graphics options
  CSettings *pSettings=new CSettings("../../data/irrOdeCarSettings.xml",L"irrOdeCar - Graphics Setup",SColor(0x00,0x21,0xAD,0x10));

  pSettings->addValidDriver(L"Direct3D9",EDT_DIRECT3D9);
  pSettings->addValidDriver(L"OpenGL",EDT_OPENGL);
  pSettings->setMinResolution(dimension2du(640,480));

  u32 iRet=pSettings->run();
  if (iRet==2) {
    delete pSettings;
    return 0;
  }

  //now create the Irrlicht device from the chosen options
  IrrlichtDevice *device=pSettings->createDeviceFromSettings();
  device->setWindowCaption(L"IrrODE car demo");

  CIrrOdeManager::getSharedInstance()->install(device);
  irr::ode::CIrrOdeWorldObserver::getSharedInstance()->install();
  CCustomEventReceiver::setMembers(device,CIrrOdeManager::getSharedInstance());
  CCustomEventReceiver::getSharedInstance()->install();

  CProgress *pProg=new CProgress(device);

  CIrrCC *pController=new CIrrCC(device);
  pController->setSetsCanConflict(false);
  pController->setAllowFKeys(false);
  pController->setAllowMouse(false);
  CConfigFileManager::getSharedInstance()->addReader(pController);
  CConfigFileManager::getSharedInstance()->addWriter(pController);

  u32 iCtrls[4][24];  //an array for all the controls we are going to define

  //first up: the car's controls
  iCtrls[0][eCarForeward           ]=pController->addItem(0,stringw(L"Foreward"             ),KEY_UP    ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarBackward           ]=pController->addItem(0,stringw(L"Backward"             ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarLeft               ]=pController->addItem(0,stringw(L"Left"                 ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarRight              ]=pController->addItem(0,stringw(L"Right"                ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarBrake              ]=pController->addItem(0,stringw(L"Brake"                ),KEY_SPACE ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarBoost              ]=pController->addItem(0,stringw(L"Boost"                ),KEY_KEY_B ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarToggleAdaptiveSteer]=pController->addItem(0,stringw(L"Toggle adaptive Steer"),KEY_KEY_X ,CIrrCC::eCtrlToggleButton);
  iCtrls[0][eCarFlip               ]=pController->addItem(0,stringw(L"Flip"                 ),KEY_RETURN,CIrrCC::eCtrlToggleButton);
  iCtrls[0][eCarBackview           ]=pController->addItem(0,stringw(L"Backview"             ),KEY_KEY_R ,CIrrCC::eCtrlButton);

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
  iCtrls[2][eHeliYawLeft  ]=pController->addItem(2,stringw(L"Yaw Left"     ),KEY_KEY_A ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliYawRight ]=pController->addItem(2,stringw(L"Yaw Right"    ),KEY_KEY_D ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliRollLeft ]=pController->addItem(2,stringw(L"Roll Left"    ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliRollRight]=pController->addItem(2,stringw(L"Roll Right"   ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliPitchUp  ]=pController->addItem(2,stringw(L"Pitch Up"     ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliPitchDown]=pController->addItem(2,stringw(L"Pitch Down"   ),KEY_UP    ,CIrrCC::eCtrlAxis);
  iCtrls[2][eHeliPowerUp  ]=pController->addItem(2,stringw(L"Power Plus"   ),KEY_KEY_W ,CIrrCC::eCtrlFader);
  iCtrls[2][eHeliPowerDown]=pController->addItem(2,stringw(L"Power Minus"  ),KEY_KEY_S ,CIrrCC::eCtrlFader);
  iCtrls[2][eHeliFire     ]=pController->addItem(2,stringw(L"Fire Missile" ),KEY_SPACE ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eHeliToggleCam]=pController->addItem(2,stringw(L"Toggle Cam"   ),KEY_KEY_C ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eHeliFlip     ]=pController->addItem(2,stringw(L"Flip"         ),KEY_RETURN,CIrrCC::eCtrlButton);
  iCtrls[2][eHeliAutoPilot]=pController->addItem(2,stringw(L"Autopilot"    ),KEY_KEY_P ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eHeliTarget   ]=pController->addItem(2,stringw(L"Select Target"),KEY_KEY_T ,CIrrCC::eCtrlToggleButton);


  //the helicopter gets three axes: pitch, roll and yaw...
  pController->createAxis(iCtrls[2][eHeliYawLeft ],iCtrls[2][eHeliYawRight ]);
  pController->createAxis(iCtrls[2][eHeliRollLeft],iCtrls[2][eHeliRollRight]);
  pController->createAxis(iCtrls[2][eHeliPitchUp ],iCtrls[2][eHeliPitchDown]);

  //...and one fader for the helicopter's thrust control
  pController->createFader(iCtrls[2][eHeliPowerUp],iCtrls[2][eHeliPowerDown],10,0.01f);

  //last but not least: the airplane. This one gets the most controls
  iCtrls[3][ePlaneYawLeft  ]=pController->addItem(3,stringw(L"Yaw Left"     ),KEY_KEY_A ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlaneYawRight ]=pController->addItem(3,stringw(L"Yaw Right"    ),KEY_KEY_D ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlaneRollLeft ]=pController->addItem(3,stringw(L"Roll Left"    ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlaneRollRight]=pController->addItem(3,stringw(L"Roll Right"   ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlanePitchUp  ]=pController->addItem(3,stringw(L"Pitch Up"     ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlanePitchDown]=pController->addItem(3,stringw(L"Pitch Down"   ),KEY_UP    ,CIrrCC::eCtrlAxis);
  iCtrls[3][ePlanePowerUp  ]=pController->addItem(3,stringw(L"Power Plus"   ),KEY_KEY_W ,CIrrCC::eCtrlFader);
  iCtrls[3][ePlanePowerDown]=pController->addItem(3,stringw(L"Power Minus"  ),KEY_KEY_S ,CIrrCC::eCtrlFader);
  iCtrls[3][ePlanePowerZero]=pController->addItem(3,stringw(L"Power Zero"   ),KEY_KEY_Z ,CIrrCC::eCtrlButton);
  iCtrls[3][ePlaneBackview ]=pController->addItem(3,stringw(L"Backview"     ),KEY_KEY_R ,CIrrCC::eCtrlButton);
  iCtrls[3][ePlaneFire     ]=pController->addItem(3,stringw(L"Fire Missile" ),KEY_SPACE ,CIrrCC::eCtrlToggleButton);
  iCtrls[3][ePlaneGun      ]=pController->addItem(3,stringw(L"Fire Gun"     ),KEY_RETURN,CIrrCC::eCtrlToggleButton);
  iCtrls[3][ePlaneToggleCam]=pController->addItem(3,stringw(L"Toggle Cam"   ),KEY_KEY_C ,CIrrCC::eCtrlToggleButton );
  iCtrls[3][ePlaneBrake    ]=pController->addItem(3,stringw(L"Wheel Brake"  ),KEY_KEY_B ,CIrrCC::eCtrlButton);
  iCtrls[3][ePlaneFlip     ]=pController->addItem(3,stringw(L"Flip"         ),KEY_KEY_F ,CIrrCC::eCtrlButton);
  iCtrls[3][ePlaneAutoPilot]=pController->addItem(3,stringw(L"Autopilot"    ),KEY_KEY_P ,CIrrCC::eCtrlToggleButton);
  iCtrls[3][ePlaneTarget   ]=pController->addItem(3,stringw(L"Select Target"),KEY_KEY_T ,CIrrCC::eCtrlToggleButton);

  //just like the helicopter the plane gets three axes (pitch, roll, yaw)...
  pController->createAxis(iCtrls[3][ePlaneYawRight],iCtrls[3][ePlaneYawLeft  ]);
  pController->createAxis(iCtrls[3][ePlanePitchUp ],iCtrls[3][ePlanePitchDown]);
  pController->createAxis(iCtrls[3][ePlaneRollLeft],iCtrls[3][ePlaneRollRight]);

  //...and one fader for the engine control
  pController->createFader(iCtrls[3][ePlanePowerUp],iCtrls[3][ePlanePowerDown],10,0.01f);

  IVideoDriver *driver = device->getVideoDriver();
  ISceneManager *smgr = device->getSceneManager();
  IGUIEnvironment *guienv = device->getGUIEnvironment();

  //register the IrrOde scene node factory
  CIrrOdeSceneNodeFactory cFactory(smgr);
  smgr->registerSceneNodeFactory(&cFactory);

  CRandomForestFactory fFactory(smgr);
  smgr->registerSceneNodeFactory(&fFactory);

  CAdvancedParticleSystemNodeFactory *cParticleFactory=new CAdvancedParticleSystemNodeFactory(smgr);
  smgr->registerSceneNodeFactory(cParticleFactory);
  cParticleFactory->drop();

  //init the ODE
  CIrrOdeManager::getSharedInstance()->initODE();

  //load the scene
  CIrrOdeManager::getSharedInstance()->loadScene("../../data/IrrOdeCar.xml",smgr);

  for (irr::u32 i=0; i<smgr->getMeshCache()->getMeshCount(); i++) {
    irr::scene::IAnimatedMesh *p=smgr->getMeshCache()->getMeshByIndex(i);
    for (irr::u32 j=0; j<p->getMeshBufferCount(); j++) {
      p->getMeshBuffer(j)->setHardwareMappingHint(irr::scene::EHM_STATIC);
    }
  }

  irr::core::list<ISceneNode *> lCars;
  fillBodyList(lCars,smgr->getRootSceneNode(),"car",pSettings->getCountOf(0));

  irr::core::list<ISceneNode *> lPlanes;
  fillBodyList(lPlanes,smgr->getRootSceneNode(),"plane",pSettings->getCountOf(1));

  irr::core::list<ISceneNode *> lTanks;
  fillBodyList(lTanks,smgr->getRootSceneNode(),"tank",pSettings->getCountOf(2));

  irr::core::list<ISceneNode *> lHelis;
  fillBodyList(lHelis,smgr->getRootSceneNode(),"heli",pSettings->getCountOf(3));

  printf("\nvehiclies found:\n\ncars: %i\nplanes: %i\nhelicopters: %i\ntanks: %i\n\n",lCars.size(),lPlanes.size(),lHelis.size(),lTanks.size());

  if (!pSettings->isActive(0)) removeFromScene("roads"       ,smgr);
  if (!pSettings->isActive(1)) removeFromScene("bumps"       ,smgr);
  if (!pSettings->isActive(2)) removeFromScene("targets"     ,smgr);
  if (!pSettings->isActive(3)) removeFromScene("plane_course",smgr);
  if (!pSettings->isActive(4)) {
    removeFromScene("forests",smgr);
  }
  else {
   const c8 sForests[][255]={ "RandomForest1", "RandomForest2", "Forest1", "Forest2" };

    for (u32 i=0; i<2; i++) {
      printf("merging \"%s\"...\n",sForests[i]);
      scene::ISceneNode *p=smgr->getSceneNodeFromName(sForests[i]);
      CRandomForest *pForest=(CRandomForest *)p;
      if (pForest!=NULL) {
        CMeshCombiner *pCombine=new CMeshCombiner(0.8f);
        irr::core::array<irr::scene::IMeshSceneNode *> aMeshSceneNodes;
        irr::core::array<irr::scene::ISceneNode *> aTrees=pForest->getGeneratedTrees();

        for (u32 j=0; j<aTrees.size(); j++) {
          irr::scene::IMeshSceneNode *p=(irr::scene::IMeshSceneNode *)aTrees[j];
          aMeshSceneNodes.push_back(p);
        }

        printf("%i trees\n",aMeshSceneNodes.size());

        if (aMeshSceneNodes.size()>0) {
          c8 s[0xFF];
          sprintf(s,"MyCombinedTrees_%i",i);
          irr::scene::IMesh *pCombined=pCombine->combineMeshes(smgr,driver,aMeshSceneNodes,s);
          if (pCombined!=NULL) {
            irr::scene::ISceneNode *pRoot=smgr->getSceneNodeFromName(sForests[i+2]);
            irr::scene::IMeshSceneNode *pNode=smgr->addMeshSceneNode(pCombined,pRoot==NULL?smgr->getRootSceneNode():pRoot);
            for (u32 i=0; i<pNode->getMaterialCount(); i++) {
              pNode->getMaterial(i).setFlag(irr::video::EMF_LIGHTING,false);
              pNode->getMaterial(i).MaterialType=irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
            }
          }
        }
      }
    }
  }
  if (!pSettings->isActive(5)) {
    printf("removing terrain trimesh...\n");
    removeFromScene("terrain_trimesh",smgr);
  }
  else {
    printf("removing terrain heightfield...\n");
    removeFromScene("terrain_heightfield",smgr);
  }

  delete pSettings;

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

  dimension2d<u32> cSize=driver->getScreenSize();

  IGUIStaticText *pFps=guienv->addStaticText(L"Hello World!",rect<s32>(position2di(cSize.Width-155,5),dimension2di(150,22)),true);
  pFps->setBackgroundColor(SColor(0x80,0xFF,0xFF,0xFF));
  pFps->setDrawBackground(true);
  pFps->setVisible(false);

  //create the necessary state objects
  CIrrOdeCarState *pActive=NULL;
  array<CIrrOdeCarState *> aStates;

  CMenu       *theMenu=new CMenu      (device,pController); aStates.push_back(theMenu);
  CController *theCtrl=new CController(device,pController); aStates.push_back(theCtrl );

  list<ISceneNode *>::Iterator it;
  for (it=lCars.begin(); it!=lCars.end(); it++) {
    CCar *p=new CCar(device,*it,pController);
    p->setCtrl((const u32 *)iCtrls[0]); p->setFpsInfo(pFps);
    aStates.push_back(p);
    theMenu->addButtonForState(p);
  }

  scene::ISceneManager *pRttManager=smgr->createNewSceneManager();
	scene::ICameraSceneNode *pCam=pRttManager->addCameraSceneNode();

	pCam->setPosition(core::vector3df(0,0,20));
	pCam->setTarget(core::vector3df(0,0,0));
	pRttManager->setActiveCamera(pCam);

  CCockpitPlane *pCockpit=new CCockpitPlane(device,pRttManager);

  for (it=lPlanes.begin(); it!=lPlanes.end(); it++) {
    CPlane *p=new CPlane(device,*it,pController,pCockpit);
    p->setCtrl((const u32 *)iCtrls[3]); p->setFpsInfo(pFps);
    aStates.push_back(p);
    theMenu->addButtonForState(p);
  }

  for (it=lTanks.begin(); it!=lTanks.end(); it++) {
    CTank *p=new CTank(device,*it,pController);
    p->setCtrl((const u32 *)iCtrls[1]); p->setFpsInfo(pFps);
    aStates.push_back(p);
    theMenu->addButtonForState(p);
  }

  for (it=lHelis.begin(); it!=lHelis.end(); it++) {
    CHeli *p=new CHeli(device,*it,pController);
    p->setCtrl((const u32 *)iCtrls[2]); p->setFpsInfo(pFps);
    aStates.push_back(p);
    theMenu->addButtonForState(p);
  }


  theCtrl->setFpsInfo(pFps);

  //phyiscs initialization
  CIrrOdeManager::getSharedInstance()->initPhysics();

  delete pProg;


  //set the menu state to active
  pActive=theMenu;
  pActive->activate();

  CConfigFileManager::getSharedInstance()->loadConfig(device,"../../data/irrOdeCarControls.xml");
  pFps->setVisible(true);

  list<CIrrOdeSceneNode *> lNodes=irr::ode::CIrrOdeManager::getSharedInstance()->getIrrOdeNodes();
  list<CIrrOdeSceneNode *>::Iterator nit;

  for (nit=lNodes.begin(); nit!=lNodes.end(); nit++) {
    irr::ode::CIrrOdeSceneNode *p=*nit;
    if (!p->physicsInitialized()) printf("\t\t--> %i (%s)\n",p->getID(),p->getName());
  }

  u32 iFrames=0,iTotalFps=0;

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

    pActive->drawSpecifics();

    driver->endScene();
    int fps = driver->getFPS();

    if (lastFPS != fps) {
      iFrames++;
      iTotalFps+=fps;

      core::stringw str = L"Irrlicht Engine - IrrODE Car Demo [";
      str += driver->getName();
      str += "] FPS:";
      str += fps;
      str += " (avg: ";
      str += (iTotalFps/iFrames);
      str += ")";

      device->setWindowCaption(str.c_str());
      lastFPS = fps;

      wchar_t s[0xFF];
      swprintf(s,0xFE,L"FPS: %i\nPrimives Drawn: %i",lastFPS,driver->getPrimitiveCountDrawn());
      pFps->setText(s);
    }
  }

  CConfigFileManager::getSharedInstance()->writeConfig(device,"../../data/irrOdeCarControls.xml");
  irr::ode::CIrrOdeWorldObserver::getSharedInstance()->destall();

  //drop the world so it is destroyed
  device->drop();

  //and now some more cleanup...
  for (u32 i=0; i<aStates.size(); i++) {
    delete aStates[i];
  }
  aStates.clear();

  return 0;
}


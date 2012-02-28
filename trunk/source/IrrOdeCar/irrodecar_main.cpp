  #include <irrlicht.h>
  #include <IrrODE.h>
  #include <CSettings.h>
  #include <CAdvancedParticleSystemNode.h>
  #include <irrKlang.h>
  #include <CRearView.h>

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
  #include <CCockpitCar.h>
  #include <CRoadMeshLoader.h>

using namespace irr;

video::SColor g_cFogColor=video::SColor(0xFF,0x3A,0x34,0x00);
f32 g_fMinFog=1750.0f,
    g_fMaxFog=2100.0f;

class CShaderCallBack : public video::IShaderConstantSetCallBack {
  protected:
    IrrlichtDevice *m_pDevice;

  public:
    CShaderCallBack(IrrlichtDevice *pDevice) {
      m_pDevice=pDevice;
    }

    virtual void OnSetConstants(video::IMaterialRendererServices* services,s32 userData) {
      video::IVideoDriver* driver = services->getVideoDriver();

      // set inverted world matrix
      // if we are using highlevel shaders (the user can select this when
      // starting the program), we must set the constants by name.

      core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
      invWorld.makeInverse();

      services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);

      // set clip matrix

      core::matrix4 worldViewProj;
      worldViewProj = driver->getTransform(video::ETS_PROJECTION);
      worldViewProj *= driver->getTransform(video::ETS_VIEW);
      worldViewProj *= driver->getTransform(video::ETS_WORLD);

      services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);

      // set camera position

      core::vector3df pos=m_pDevice->getSceneManager()->getActiveCamera()->getAbsolutePosition();

      // set transposed world matrix

      core::matrix4 world = driver->getTransform(video::ETS_WORLD);
      world = world.getTransposed();

      services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

      services->setVertexShaderConstant("mMinFog",&g_fMinFog,1);
      services->setVertexShaderConstant("mMaxFog",&g_fMaxFog,1);
    }
};

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
      m_pImg->setImage(m_pDriver->getTexture("../../data/textures/standby.png"));

      m_pBar=new CProgressBar(m_pGuienv,rect<s32>(position2di(cScreenSize.Width/2-150,cScreenSize.Height-60),dimension2di(300,30)),-1,NULL);
      m_pText=m_pGuienv->addStaticText(L"",rect<s32>(position2di(cScreenSize.Width/2-150,cScreenSize.Height-60),dimension2di(300,30)),true);
      m_pText->setBackgroundColor(SColor(192,192,192,192));
      m_pText->setDrawBackground(true);
      m_pText->setTextAlignment(gui::EGUIA_CENTER,gui::EGUIA_CENTER);

      ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
    }

    virtual ~CProgress() {
      m_pImg->remove();
      m_pText->remove();
      m_pBar->remove();
      ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
    }

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent) {
      bool bRet=false;

      if (pEvent->getType()==ode::eIrrOdeEventProgress) {
        ::ode::CIrrOdeEventProgress *pEvt=reinterpret_cast<ode::CIrrOdeEventProgress *>(pEvent);
        wchar_t s[0xFF];

        if (pEvt->getCount()==0) {
          swprintf(s,0xFF,L"Loading scene...");
          m_pText->setText(s);
        }
        else {
          swprintf(s,0xFF,L"Physics Initialization: %i / %i",pEvt->getCurrent(),pEvt->getCount());
          m_pText->setText(s);
          m_pBar->setProgress(100*pEvt->getCurrent()/pEvt->getCount());
        }

        m_pDriver->beginScene(true, true, SColor(0,200,200,200));
        m_pGuienv->drawAll();
        m_pDriver->endScene();
      }

      return bRet;
    }

    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent) {
      return pEvent->getType()==irr::ode::eIrrOdeEventProgress;
    }
};

void enableFog(scene::ISceneNode *pNode) {
  if (pNode->getType()!=scene::ESNT_SKY_BOX)
    for (u32 i=0; i<pNode->getMaterialCount(); i++) {
      pNode->getMaterial(i).setFlag(video::EMF_FOG_ENABLE,true);
    }

  core::list<ISceneNode *> lChildList=pNode->getChildren();
  core::list<ISceneNode *>::Iterator it;

  for (it=lChildList.begin(); it!=lChildList.end(); it++) enableFog(*it);
}

void replaceMaterials(scene::ISceneNode *pNode, s32 iNewMaterial) {
  if (pNode->getType()==scene::ESNT_SKY_BOX) return;

  for (u32 i=0; i<pNode->getMaterialCount(); i++) {
    if (pNode->getMaterial(i).MaterialType==video::EMT_SOLID)
      pNode->getMaterial(i).MaterialType=(video::E_MATERIAL_TYPE)iNewMaterial;
  }

  core::list<scene::ISceneNode *> lChildren=pNode->getChildren();
  core::list<scene::ISceneNode *>::Iterator it;

  for (it=lChildren.begin(); it!=lChildren.end(); it++) {
    replaceMaterials(*it,iNewMaterial);
  }
}

void removeFromScene(const c8 *sName, ISceneManager *smgr) {
  ISceneNode *pNode=smgr->getSceneNodeFromName(sName);
  if (pNode!=NULL) {
    ode::CIrrOdeManager::getSharedInstance()->removeTreeFromPhysics(pNode);
    s32 iNodeId=pNode->getID();
    pNode->remove();

    ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
    ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
  }
}

void removeFromScene(ISceneNode *pNode) {
  ode::CIrrOdeManager::getSharedInstance()->removeTreeFromPhysics(pNode);
  s32 iNodeId=pNode->getID();
  pNode->remove();

  irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
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

  pSettings->setMinResolution(dimension2du(640,480));

  u32 iRet=pSettings->run();
  if (iRet==2) {
    delete pSettings;
    return 0;
  }

  //now create the Irrlicht device from the chosen options
  IrrlichtDevice *device=pSettings->createDeviceFromSettings();
  device->setWindowCaption(L"IrrODE car demo");

  irrklang::ISoundEngine *pSndEngine=irrklang::createIrrKlangDevice();

  ode::CIrrOdeManager::getSharedInstance()->install(device);
  irr::ode::CIrrOdeWorldObserver::getSharedInstance()->install();
  CCustomEventReceiver::setMembers(device,ode::CIrrOdeManager::getSharedInstance(),pSndEngine);
  CCustomEventReceiver::getSharedInstance()->install();

  CProgress *pProg=new CProgress(device);

  irr::ode::CIrrOdeEventProgress *p=new irr::ode::CIrrOdeEventProgress(0,0);
  pProg->onEvent(p);
  delete p;

  CIrrCC *pController=new CIrrCC(device);
  pController->setSetsCanConflict(false);
  pController->setAllowFKeys(false);
  pController->setAllowMouse(false);
  CConfigFileManager::getSharedInstance()->addReader(pController);
  CConfigFileManager::getSharedInstance()->addWriter(pController);

  u32 iCtrls[4][32];  //an array for all the controls we are going to define

  //first up: the car's controls
  iCtrls[0][eCarForeward           ]=pController->addItem(0,stringw(L"Foreward"             ),KEY_UP    ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarBackward           ]=pController->addItem(0,stringw(L"Backward"             ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarLeft               ]=pController->addItem(0,stringw(L"Left"                 ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarRight              ]=pController->addItem(0,stringw(L"Right"                ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
  iCtrls[0][eCarBrake              ]=pController->addItem(0,stringw(L"Brake"                ),KEY_SPACE ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarBoost              ]=pController->addItem(0,stringw(L"Boost"                ),KEY_KEY_B ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarToggleAdaptiveSteer]=pController->addItem(0,stringw(L"Toggle adaptive Steer"),KEY_KEY_T ,CIrrCC::eCtrlToggleButton);
  iCtrls[0][eCarFlip               ]=pController->addItem(0,stringw(L"Flip"                 ),KEY_RETURN,CIrrCC::eCtrlToggleButton);
  iCtrls[0][eCarInternal           ]=pController->addItem(0,stringw(L"Internal Camera"      ),KEY_KEY_I ,CIrrCC::eCtrlToggleButton);
  iCtrls[0][eCarCamLeft            ]=pController->addItem(0,stringw(L"Camera Left"          ),KEY_KEY_Y ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarCamRight           ]=pController->addItem(0,stringw(L"Camera Right"         ),KEY_KEY_C ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarCamUp              ]=pController->addItem(0,stringw(L"Camera Up"            ),KEY_KEY_F ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarCamDown            ]=pController->addItem(0,stringw(L"Camera Down"          ),KEY_KEY_V ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarCamCenter          ]=pController->addItem(0,stringw(L"Center Camera"        ),KEY_KEY_X ,CIrrCC::eCtrlButton);
  iCtrls[0][eCarDifferential       ]=pController->addItem(0,stringw(L"Toggle Differential"  ),KEY_KEY_D ,CIrrCC::eCtrlButton);

  //we need two axes for the car: acceleration and steering
  pController->createAxis(iCtrls[0][eCarForeward],iCtrls[0][eCarBackward]);
  pController->createAxis(iCtrls[0][eCarLeft    ],iCtrls[0][eCarRight   ]);
  pController->createAxis(iCtrls[0][eCarCamLeft ],iCtrls[0][eCarCamRight]);
  pController->createAxis(iCtrls[0][eCarCamDown ],iCtrls[0][eCarCamUp   ]);

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

  iCtrls[2][eAeroPitchUp      ]=pController->addItem(2,stringw("Pitch Up"      ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroPitchDown    ]=pController->addItem(2,stringw("Pitch Down"    ),KEY_UP    ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroRollLeft     ]=pController->addItem(2,stringw("Roll Left"     ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroRollRight    ]=pController->addItem(2,stringw("Roll Right"    ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroYawLeft      ]=pController->addItem(2,stringw("Yaw Left"      ),KEY_KEY_A ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroYawRight     ]=pController->addItem(2,stringw("Yaw Right"     ),KEY_KEY_D ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroPowerUp      ]=pController->addItem(2,stringw("Power Up"      ),KEY_KEY_W ,CIrrCC::eCtrlFader);
  iCtrls[2][eAeroPowerDown    ]=pController->addItem(2,stringw("Power Down"    ),KEY_KEY_S ,CIrrCC::eCtrlFader);
  iCtrls[2][eAeroPowerZero    ]=pController->addItem(2,stringw("Power Zero"    ),KEY_KEY_Z ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eAeroBrake        ]=pController->addItem(2,stringw("Wheel Brake"   ),KEY_KEY_B ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroFirePrimary  ]=pController->addItem(2,stringw("Fire Primary"  ),KEY_SPACE ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eAeroFireSecondary]=pController->addItem(2,stringw("Fire Secondary"),KEY_RETURN,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eAeroSelectTarget ]=pController->addItem(2,stringw("Select Target" ),KEY_KEY_T ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eAeroAutoPilot    ]=pController->addItem(2,stringw("Autopilot"     ),KEY_KEY_P ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eAeroToggleCam    ]=pController->addItem(2,stringw("Missile Cam"   ),KEY_KEY_M ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eAeroInternalView ]=pController->addItem(2,stringw("Internal View" ),KEY_KEY_I ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eAeroCamLeft      ]=pController->addItem(2,stringw("Camera Left"   ),KEY_KEY_Y ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroCamRight     ]=pController->addItem(2,stringw("Camera Right"  ),KEY_KEY_C ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroCamUp        ]=pController->addItem(2,stringw("Camera Up"     ),KEY_KEY_F ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroCamDown      ]=pController->addItem(2,stringw("Camera Down"   ),KEY_KEY_V ,CIrrCC::eCtrlAxis);
  iCtrls[2][eAeroCamCenter    ]=pController->addItem(2,stringw("Center Camera" ),KEY_KEY_X ,CIrrCC::eCtrlToggleButton);
  iCtrls[2][eAeroFlip         ]=pController->addItem(2,stringw("Flip"          ),KEY_KEY_L ,CIrrCC::eCtrlToggleButton);

  pController->createAxis(iCtrls[2][eAeroYawLeft ],iCtrls[2][eAeroYawRight ]);
  pController->createAxis(iCtrls[2][eAeroRollLeft],iCtrls[2][eAeroRollRight]);
  pController->createAxis(iCtrls[2][eAeroPitchUp ],iCtrls[2][eAeroPitchDown]);
  pController->createAxis(iCtrls[2][eAeroCamLeft ],iCtrls[2][eAeroCamRight ]);
  pController->createAxis(iCtrls[2][eAeroCamUp   ],iCtrls[2][eAeroCamDown  ]);

  pController->createFader(iCtrls[2][eAeroPowerUp],iCtrls[2][eAeroPowerDown],10,0.01f);

  IVideoDriver *driver = device->getVideoDriver();
  ISceneManager *smgr = device->getSceneManager();
  IGUIEnvironment *guienv = device->getGUIEnvironment();

  //register the IrrOde scene node factory
  ode::CIrrOdeSceneNodeFactory cFactory(smgr);
  smgr->registerSceneNodeFactory(&cFactory);

  if (pSettings->isActive(4)) {
    CEventInstallRandomForestPlugin *p=new CEventInstallRandomForestPlugin();
    ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
  }

  CAdvancedParticleSystemNodeFactory *cParticleFactory=new CAdvancedParticleSystemNodeFactory(smgr);
  smgr->registerSceneNodeFactory(cParticleFactory);
  cParticleFactory->drop();

  CRoadMeshLoader *pLoader=new CRoadMeshLoader(device);
  smgr->addExternalMeshLoader(pLoader);

  //init the ODE
  ode::CIrrOdeManager::getSharedInstance()->initODE();

  //load the scene
  ode::CIrrOdeManager::getSharedInstance()->loadScene("../../data/scenes/IrrOdeCar.xml",smgr);

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

  bool bRearCam=pSettings->isActive(6);
  printf("bRearCam=%s\n",bRearCam?"true":"false");

  if (!pSettings->isActive(0)) removeFromScene("roads"       ,smgr);
  if (!pSettings->isActive(1)) removeFromScene("bumps"       ,smgr);
  if (!pSettings->isActive(2)) removeFromScene("targets"     ,smgr);
  if (!pSettings->isActive(3)) removeFromScene("plane_course",smgr);
  if ( pSettings->isActive(4)) {
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

  bool bUseShader=pSettings->getSelectedDriver()==video::EDT_OPENGL;

  if (bUseShader) {
    if (!driver->queryFeature(video::EVDF_PIXEL_SHADER_1_1)) {
      printf("Pixel shader disabled!\n");
      bUseShader=false;
    }

    if (!driver->queryFeature(video::EVDF_VERTEX_SHADER_1_1)) {
      printf("Vertex shader disabled!\n");
      bUseShader=false;
    }
  }

  //I can only add shaders for OpenGL
  if (bUseShader) {
    video::IGPUProgrammingServices *pGpu=driver->getGPUProgrammingServices();
    if (pGpu) {
      printf("\n**** compiling GLSL shader ... \n\n");
      CShaderCallBack *pCallback=new CShaderCallBack(device);
      s32 iNewMaterial=pGpu->addHighLevelShaderMaterialFromFiles(
          "../../data/shaders/opengl.vert","vertexMain", video::EVST_VS_1_1,
          "../../data/shaders/opengl.frag", "pixelMain", video::EPST_PS_1_1,
          pCallback,video::EMT_SOLID);

      replaceMaterials(smgr->getRootSceneNode(),iNewMaterial);
      pCallback->drop();
      printf("Ready.\n\n");
    }
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

  //create the necessary state objects
  CIrrOdeCarState *pActive=NULL;
  array<CIrrOdeCarState *> aStates;

  CMenu       *theMenu=new CMenu      (device,pController); aStates.push_back(theMenu);
  CController *theCtrl=new CController(device,pController); aStates.push_back(theCtrl );

  list<ISceneNode *>::Iterator it;

  CRearView *pRearView=NULL;

  smgr->addCameraSceneNode();
  CCockpitPlane *pCockpit=new CCockpitPlane(device,"instruments");
  if (bRearCam) pRearView=new CRearView(device,"rearview.jpg",smgr->addCameraSceneNode());

  for (it=lPlanes.begin(); it!=lPlanes.end(); it++) {
    CPlane *p=new CPlane(device,*it,pController,pCockpit,pRearView);
    p->setCtrl((const u32 *)iCtrls[2]);
    aStates.push_back(p);
    theMenu->addButtonForState(p);
  }

  CCockpitCar *pCarCockpit=new CCockpitCar(device,"z_instru.jpg");

  for (it=lCars.begin(); it!=lCars.end(); it++) {
    CCar *p=new CCar(device,*it,pController,pCarCockpit,pRearView);
    p->setCtrl((const u32 *)iCtrls[0]);
    aStates.push_back(p);
    theMenu->addButtonForState(p);
  }

  for (it=lTanks.begin(); it!=lTanks.end(); it++) {
    CTank *p=new CTank(device,*it,pController);
    p->setCtrl((const u32 *)iCtrls[1]);
    aStates.push_back(p);
    theMenu->addButtonForState(p);
  }

  for (it=lHelis.begin(); it!=lHelis.end(); it++) {
    CHeli *p=new CHeli(device,*it,pController,pCockpit,pRearView);
    p->setCtrl((const u32 *)iCtrls[2]);
    aStates.push_back(p);
    theMenu->addButtonForState(p);
  }

  //phyiscs initialization
  ode::CIrrOdeManager::getSharedInstance()->initPhysics();

  delete pProg;


  //set the menu state to active
  pActive=theMenu;
  pActive->activate();

  CConfigFileManager::getSharedInstance()->loadConfig(device,"../../data/irrOdeCarControls.xml");

  list<ode::CIrrOdeSceneNode *> lNodes=ode::CIrrOdeManager::getSharedInstance()->getIrrOdeNodes();
  list<ode::CIrrOdeSceneNode *>::Iterator nit;

  for (nit=lNodes.begin(); nit!=lNodes.end(); nit++) {
    ode::CIrrOdeSceneNode *p=*nit;
    if (!p->physicsInitialized()) printf("\t\t--> %i (%s)\n",p->getID(),p->getName());
  }

  driver->setFog(g_cFogColor,video::EFT_FOG_LINEAR,g_fMinFog,g_fMaxFog,0.00001f,true,false);
  enableFog(smgr->getRootSceneNode());

  u32 iFrames=0,iTotalFps=0;

  //let's run the loop
  while(device->run()) {
    //step the simulation
    ode::CIrrOdeManager::getSharedInstance()->step();

    if (pSndEngine && pActive) {
      scene::ICameraSceneNode *pCam=smgr->getActiveCamera();
      core::vector3df irrPos=pCam->getPosition(),
                      irrTgt=pCam->getTarget(),
                      irrUp =pCam->getUpVector(),
                      irrVel=pActive->getCameraVelocity();

      irrklang::vec3df pos=irrklang::vec3df(irrPos.X,irrPos.Y,irrPos.Z),
                       tgt=irrklang::vec3df(irrTgt.X,irrTgt.Y,irrTgt.Z),
                       up =irrklang::vec3df(irrUp .X,irrUp .Y,irrUp .Z),
                       vel=irrklang::vec3df(irrVel.X,irrVel.Y,irrVel.Z);

      pSndEngine->setListenerPosition(pos,tgt,vel,up);
      pSndEngine->setRolloffFactor(0.125f);
    }

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
    driver->beginScene(true,true,video::SColor(0xFF,0xA0,0xA0,0xC0));

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
    }
  }

  CConfigFileManager::getSharedInstance()->writeConfig(device,"../../data/irrOdeCarControls.xml");
  irr::ode::CIrrOdeWorldObserver::getSharedInstance()->destall();

  //drop the world so it is destroyed
  device->drop();

  if (pSndEngine) pSndEngine->drop();

  //and now some more cleanup...
  for (u32 i=0; i<aStates.size(); i++) {
    delete aStates[i];
  }
  aStates.clear();

  return 0;
}


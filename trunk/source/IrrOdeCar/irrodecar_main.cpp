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
  #include <CCameraController.h>

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
      m_pText->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);

      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
    }

    virtual ~CProgress() {
      m_pImg->remove();
      m_pText->remove();
      m_pBar->remove();
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
    }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent) {
      bool bRet=false;

      if (pEvent->getType()==irr::ode::eIrrOdeEventProgress) {
        irr::ode::CIrrOdeEventProgress *pEvt=reinterpret_cast<irr::ode::CIrrOdeEventProgress *>(pEvent);
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

    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
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
    irr::ode::CIrrOdeManager::getSharedInstance()->removeTreeFromPhysics(pNode);
    s32 iNodeId=pNode->getID();
    pNode->remove();

    irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
    irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
  }
}

void removeFromScene(ISceneNode *pNode) {
  irr::ode::CIrrOdeManager::getSharedInstance()->removeTreeFromPhysics(pNode);
  s32 iNodeId=pNode->getID();
  pNode->remove();

  irr::ode::CIrrOdeEventNodeRemoved *p=new irr::ode::CIrrOdeEventNodeRemoved(iNodeId);
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
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

class CIrrOdeCar : public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  private:
    irr::gui::IGUIStaticText  *m_pRecording,
                              *m_pSaveFile;
    irr::ode::CIrrOdeRecorder *m_pRecorder;
    irr::u32 m_iCount;

    irr::IrrlichtDevice *m_pDevice;

    video::IVideoDriver  *m_pDriver;
    scene::ISceneManager *m_pSmgr;
    gui::IGUIEnvironment *m_pGui;

    irrklang::ISoundEngine *m_pSndEngine;
    CMenu *m_pMenu;

    CIrrCC *m_pController;

    CIrrOdeCarState *m_pActive;

    irr::u32 m_iCtrls[4][32];  //an array for all the controls we are going to define

    bool m_bHelp,
         m_bSwitchToMenu;

    irr::core::list<IRenderToTexture *> m_lCockpits;

    CCameraController *m_pCamCtrl;

    void initControls() {
      //first up: the car's controls
      m_iCtrls[0][eCarForeward           ]=m_pController->addItem(0,stringw(L"Foreward"             ),KEY_UP    ,CIrrCC::eCtrlAxis);
      m_iCtrls[0][eCarBackward           ]=m_pController->addItem(0,stringw(L"Backward"             ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
      m_iCtrls[0][eCarLeft               ]=m_pController->addItem(0,stringw(L"Left"                 ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
      m_iCtrls[0][eCarRight              ]=m_pController->addItem(0,stringw(L"Right"                ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
      m_iCtrls[0][eCarBrake              ]=m_pController->addItem(0,stringw(L"Brake"                ),KEY_SPACE ,CIrrCC::eCtrlButton);
      m_iCtrls[0][eCarBoost              ]=m_pController->addItem(0,stringw(L"Boost"                ),KEY_KEY_B ,CIrrCC::eCtrlButton);
      m_iCtrls[0][eCarFlip               ]=m_pController->addItem(0,stringw(L"Flip"                 ),KEY_RETURN,CIrrCC::eCtrlToggleButton);
      m_iCtrls[0][eCarDifferential       ]=m_pController->addItem(0,stringw(L"Toggle Differential"  ),KEY_KEY_D ,CIrrCC::eCtrlButton);

      //we need two axes for the car: acceleration and steering
      m_pController->createAxis(m_iCtrls[0][eCarForeward],m_iCtrls[0][eCarBackward]);
      m_pController->createAxis(m_iCtrls[0][eCarLeft    ],m_iCtrls[0][eCarRight   ]);

      //next up: the tank
      m_iCtrls[1][eTankForeward     ]=m_pController->addItem(1,stringw(L"Foreward"         ),KEY_UP    ,CIrrCC::eCtrlAxis);
      m_iCtrls[1][eTankBackward     ]=m_pController->addItem(1,stringw(L"Backward"         ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
      m_iCtrls[1][eTankLeft         ]=m_pController->addItem(1,stringw(L"Left"             ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
      m_iCtrls[1][eTankRight        ]=m_pController->addItem(1,stringw(L"Right"            ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
      m_iCtrls[1][eTankCannonLeft   ]=m_pController->addItem(1,stringw(L"Cannon Left"      ),KEY_KEY_A ,CIrrCC::eCtrlAxis);
      m_iCtrls[1][eTankCannonRight  ]=m_pController->addItem(1,stringw(L"Cannon Right"     ),KEY_KEY_D ,CIrrCC::eCtrlAxis);
      m_iCtrls[1][eTankCannonUp     ]=m_pController->addItem(1,stringw(L"Cannon Up"        ),KEY_KEY_W ,CIrrCC::eCtrlAxis);
      m_iCtrls[1][eTankCannonDown   ]=m_pController->addItem(1,stringw(L"Cannon Down"      ),KEY_KEY_S ,CIrrCC::eCtrlAxis);
      m_iCtrls[1][eTankFire         ]=m_pController->addItem(1,stringw(L"Fire"             ),KEY_SPACE ,CIrrCC::eCtrlToggleButton);
      m_iCtrls[1][eTankFlip         ]=m_pController->addItem(1,stringw(L"Flip"             ),KEY_RETURN,CIrrCC::eCtrlToggleButton);
      m_iCtrls[1][eTankFastCollision]=m_pController->addItem(1,stringw(L"Fast Collision"   ),KEY_KEY_F ,CIrrCC::eCtrlToggleButton);

      //this time we're gonna create four axes: acceleration, steering, turret movement and cannon angle
      m_pController->createAxis(m_iCtrls[1][eTankForeward  ],m_iCtrls[1][eTankBackward   ]);
      m_pController->createAxis(m_iCtrls[1][eTankLeft      ],m_iCtrls[1][eTankRight      ]);
      m_pController->createAxis(m_iCtrls[1][eTankCannonUp  ],m_iCtrls[1][eTankCannonDown ]);
      m_pController->createAxis(m_iCtrls[1][eTankCannonLeft],m_iCtrls[1][eTankCannonRight]);

      m_iCtrls[2][eAeroPitchUp      ]=m_pController->addItem(2,stringw("Pitch Up"      ),KEY_DOWN  ,CIrrCC::eCtrlAxis);
      m_iCtrls[2][eAeroPitchDown    ]=m_pController->addItem(2,stringw("Pitch Down"    ),KEY_UP    ,CIrrCC::eCtrlAxis);
      m_iCtrls[2][eAeroRollLeft     ]=m_pController->addItem(2,stringw("Roll Left"     ),KEY_LEFT  ,CIrrCC::eCtrlAxis);
      m_iCtrls[2][eAeroRollRight    ]=m_pController->addItem(2,stringw("Roll Right"    ),KEY_RIGHT ,CIrrCC::eCtrlAxis);
      m_iCtrls[2][eAeroYawLeft      ]=m_pController->addItem(2,stringw("Yaw Left"      ),KEY_KEY_A ,CIrrCC::eCtrlAxis);
      m_iCtrls[2][eAeroYawRight     ]=m_pController->addItem(2,stringw("Yaw Right"     ),KEY_KEY_D ,CIrrCC::eCtrlAxis);
      m_iCtrls[2][eAeroPowerUp      ]=m_pController->addItem(2,stringw("Power Up"      ),KEY_KEY_W ,CIrrCC::eCtrlFader);
      m_iCtrls[2][eAeroPowerDown    ]=m_pController->addItem(2,stringw("Power Down"    ),KEY_KEY_S ,CIrrCC::eCtrlFader);
      m_iCtrls[2][eAeroPowerZero    ]=m_pController->addItem(2,stringw("Power Zero"    ),KEY_KEY_Z ,CIrrCC::eCtrlToggleButton);
      m_iCtrls[2][eAeroBrake        ]=m_pController->addItem(2,stringw("Wheel Brake"   ),KEY_KEY_B ,CIrrCC::eCtrlAxis);
      m_iCtrls[2][eAeroFirePrimary  ]=m_pController->addItem(2,stringw("Fire Primary"  ),KEY_SPACE ,CIrrCC::eCtrlToggleButton);
      m_iCtrls[2][eAeroFireSecondary]=m_pController->addItem(2,stringw("Fire Secondary"),KEY_RETURN,CIrrCC::eCtrlToggleButton);
      m_iCtrls[2][eAeroSelectTarget ]=m_pController->addItem(2,stringw("Select Target" ),KEY_KEY_T ,CIrrCC::eCtrlToggleButton);
      m_iCtrls[2][eAeroAutoPilot    ]=m_pController->addItem(2,stringw("Autopilot"     ),KEY_KEY_P ,CIrrCC::eCtrlToggleButton);
      m_iCtrls[2][eAeroFlip         ]=m_pController->addItem(2,stringw("Flip"          ),KEY_KEY_L ,CIrrCC::eCtrlToggleButton);

      m_pController->createAxis(m_iCtrls[2][eAeroYawLeft ],m_iCtrls[2][eAeroYawRight ]);
      m_pController->createAxis(m_iCtrls[2][eAeroRollLeft],m_iCtrls[2][eAeroRollRight]);
      m_pController->createAxis(m_iCtrls[2][eAeroPitchUp ],m_iCtrls[2][eAeroPitchDown]);

      m_pController->createFader(m_iCtrls[2][eAeroPowerUp],m_iCtrls[2][eAeroPowerDown],10,0.01f);

      m_iCtrls[3][eCameraLeft    ] = m_pController->addItem(3, stringw("Camera Left"    ), KEY_KEY_Y, CIrrCC::eCtrlAxis);
      m_iCtrls[3][eCameraRight   ] = m_pController->addItem(3, stringw("Camera Right"   ), KEY_KEY_C, CIrrCC::eCtrlAxis);
      m_iCtrls[3][eCameraUp      ] = m_pController->addItem(3, stringw("Camera Up"      ), KEY_KEY_F, CIrrCC::eCtrlAxis);
      m_iCtrls[3][eCameraDown    ] = m_pController->addItem(3, stringw("Camera Down"    ), KEY_KEY_V, CIrrCC::eCtrlAxis);
      m_iCtrls[3][eCameraCenter  ] = m_pController->addItem(3, stringw("Center Camera"  ), KEY_KEY_X, CIrrCC::eCtrlButton);
      m_iCtrls[3][eCameraInternal] = m_pController->addItem(3, stringw("Toggle Internal"), KEY_KEY_I, CIrrCC::eCtrlToggleButton);

      m_pController->createAxis(m_iCtrls[3][eCameraLeft], m_iCtrls[3][eCameraRight]);
      m_pController->createAxis(m_iCtrls[3][eCameraUp  ], m_iCtrls[3][eCameraDown ]);
    }

  public:
    CIrrOdeCar(irr::IrrlichtDevice *pDevice) {
      m_pDevice = pDevice;

      m_pDriver = m_pDevice->getVideoDriver  ();
      m_pSmgr   = m_pDevice->getSceneManager  ();
      m_pGui    = m_pDevice->getGUIEnvironment();

      m_pSndEngine=irrklang::createIrrKlangDevice();

      m_bSwitchToMenu = false;
      m_bHelp = false;
      m_iCount = 0;

      m_pActive=NULL;

      irr::ode::CIrrOdeManager::getSharedInstance()->install(m_pDevice);
      irr::ode::CIrrOdeWorldObserver::getSharedInstance()->install();
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);

      CCustomEventReceiver::setMembers(m_pDevice,irr::ode::CIrrOdeManager::getSharedInstance(),m_pSndEngine);
      CCustomEventReceiver::getSharedInstance()->install();

      irr::core::dimension2d<irr::u32> cSize=m_pDriver->getScreenSize();
      irr::core::rect<irr::s32> cRect=irr::core::rect<irr::s32>(cSize.Width/2-100,25,cSize.Width/2+100,40);
      m_pSaveFile=m_pGui->addStaticText(L"Replay file saved.",cRect,true,true,0,-1,true);
      m_pSaveFile->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);
      m_pSaveFile->setVisible(false);

      m_pRecorder  = NULL;
      m_pRecording = m_pGui->addStaticText(L"Recording"       , core::rect<s32>(cSize.Width/2 - 100, 10, cSize.Width/2 + 100, 30), true); m_pRecording->setVisible(false);
      m_pSaveFile  = m_pGui->addStaticText(L"Saving Replay...", core::rect<s32>(cSize.Width/2 - 100, 35, cSize.Width/2 + 100, 55), true); m_pSaveFile ->setVisible(false);

      m_pController=new CIrrCC(m_pDevice);
      m_pController->setSetsCanConflict(false);
      m_pController->setAllowFKeys(false);
      m_pController->setAllowMouse(false);
      CConfigFileManager::getSharedInstance()->addReader(m_pController);
      CConfigFileManager::getSharedInstance()->addWriter(m_pController);

      m_pDevice->setEventReceiver(this);

      m_pCamCtrl = NULL;
    }

    ~CIrrOdeCar() {
      irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
    }

    void run(CSettings *pSettings) {
      CProgress *pProg=new CProgress(m_pDevice);

      irr::ode::CIrrOdeEventProgress *p=new irr::ode::CIrrOdeEventProgress(0,0);
      pProg->onEvent(p);
      delete p;

      initControls();

      //register the IrrOde scene node factory
      irr::ode::CIrrOdeSceneNodeFactory cFactory(m_pSmgr);
      m_pSmgr->registerSceneNodeFactory(&cFactory);

      if (pSettings->isActive(4)) {
        CEventInstallRandomForestPlugin *p=new CEventInstallRandomForestPlugin();
        irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->postEvent(p);
      }

      CAdvancedParticleSystemNodeFactory *cParticleFactory=new CAdvancedParticleSystemNodeFactory(m_pSmgr);
      m_pSmgr->registerSceneNodeFactory(cParticleFactory);
      cParticleFactory->drop();

      CRoadMeshLoader *pLoader=new CRoadMeshLoader(m_pDevice);
      m_pSmgr->addExternalMeshLoader(pLoader);

      //init the ODE
      irr::ode::CIrrOdeManager::getSharedInstance()->initODE();

      //load the scene
      irr::ode::CIrrOdeManager::getSharedInstance()->loadScene("../../data/scenes/IrrOdeCar.xml",m_pSmgr);

      for (irr::u32 i=0; i<m_pSmgr->getMeshCache()->getMeshCount(); i++) {
        irr::scene::IAnimatedMesh *p=m_pSmgr->getMeshCache()->getMeshByIndex(i);
        for (irr::u32 j=0; j<p->getMeshBufferCount(); j++) {
          p->getMeshBuffer(j)->setHardwareMappingHint(irr::scene::EHM_STATIC);
        }
      }

      irr::core::list<ISceneNode *> lCars;
      fillBodyList(lCars,m_pSmgr->getRootSceneNode(),"car",pSettings->getCountOf(0));

      irr::core::list<ISceneNode *> lPlanes;
      fillBodyList(lPlanes,m_pSmgr->getRootSceneNode(),"plane",pSettings->getCountOf(1));

      irr::core::list<ISceneNode *> lTanks;
      fillBodyList(lTanks,m_pSmgr->getRootSceneNode(),"tank",pSettings->getCountOf(2));

      irr::core::list<ISceneNode *> lHelis;
      fillBodyList(lHelis,m_pSmgr->getRootSceneNode(),"heli",pSettings->getCountOf(3));

      printf("\nvehiclies found:\n\ncars: %i\nplanes: %i\nhelicopters: %i\ntanks: %i\n\n",lCars.size(),lPlanes.size(),lHelis.size(),lTanks.size());

      bool bRearCam=pSettings->isActive(6);
      printf("bRearCam=%s\n",bRearCam?"true":"false");

      if (!pSettings->isActive(0)) removeFromScene("roads"       ,m_pSmgr);
      if (!pSettings->isActive(1)) removeFromScene("bumps"       ,m_pSmgr);
      if (!pSettings->isActive(2)) removeFromScene("targets"     ,m_pSmgr);
      if (!pSettings->isActive(3)) removeFromScene("plane_course",m_pSmgr);
      if ( pSettings->isActive(4)) {
       const c8 sForests[][255]={ "RandomForest1", "RandomForest2", "Forest1", "Forest2" };

        for (u32 i=0; i<2; i++) {
          printf("merging \"%s\"...\n",sForests[i]);
          scene::ISceneNode *p=m_pSmgr->getSceneNodeFromName(sForests[i]);
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
              irr::scene::IMesh *pCombined=pCombine->combineMeshes(m_pSmgr,m_pDriver,aMeshSceneNodes,s);
              if (pCombined!=NULL) {
                irr::scene::ISceneNode *pRoot=m_pSmgr->getSceneNodeFromName(sForests[i+2]);
                irr::scene::IMeshSceneNode *pNode=m_pSmgr->addMeshSceneNode(pCombined,pRoot==NULL?m_pSmgr->getRootSceneNode():pRoot);
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
        removeFromScene("terrain_trimesh",m_pSmgr);
      }
      else {
        printf("removing terrain heightfield...\n");
        removeFromScene("terrain_heightfield",m_pSmgr);
      }

      bool bUseShader=pSettings->getSelectedDriver()==video::EDT_OPENGL;

      if (bUseShader) {
        if (!m_pDriver->queryFeature(video::EVDF_PIXEL_SHADER_1_1)) {
          printf("Pixel shader disabled!\n");
          bUseShader=false;
        }

        if (!m_pDriver->queryFeature(video::EVDF_VERTEX_SHADER_1_1)) {
          printf("Vertex shader disabled!\n");
          bUseShader=false;
        }
      }

      //I can only add shaders for OpenGL
      if (bUseShader) {
        video::IGPUProgrammingServices *pGpu=m_pDriver->getGPUProgrammingServices();
        if (pGpu) {
          printf("\n**** compiling GLSL shader ... \n\n");
          CShaderCallBack *pCallback=new CShaderCallBack(m_pDevice);
          s32 iNewMaterial=pGpu->addHighLevelShaderMaterialFromFiles(
              "../../data/shaders/opengl.vert","vertexMain", video::EVST_VS_1_1,
              "../../data/shaders/opengl.frag", "pixelMain", video::EPST_PS_1_1,
              pCallback,video::EMT_SOLID);

          replaceMaterials(m_pSmgr->getRootSceneNode(),iNewMaterial);
          pCallback->drop();
          printf("Ready.\n\n");
        }
      }

      delete pSettings;

      m_pCamCtrl = new CCameraController(m_pDevice, m_pSndEngine, m_pController, irr::ode::CIrrOdeManager::getSharedInstance());
      m_pCamCtrl->setCtrl(m_iCtrls[3]);

      //modify the textures of the car segment and the tank segment to
      IAnimatedMeshSceneNode *pNode=(IAnimatedMeshSceneNode *)m_pSmgr->getSceneNodeFromName("car_segment");
      if (pNode) pNode->getMaterial(0).getTextureMatrix(0).setTextureScale(50.0f,50.0f);
      pNode=(IAnimatedMeshSceneNode *)m_pSmgr->getSceneNodeFromName("tank_segment");
      if (pNode) pNode->getMaterial(0).getTextureMatrix(0).setTextureScale(50.0f,50.0f);

      int lastFPS=-1;

      //create the necessary state objects
      array<CIrrOdeCarState *> aStates;

                   m_pMenu=new CMenu      (m_pDevice,m_pController); aStates.push_back(m_pMenu);
      CController *theCtrl=new CController(m_pDevice,m_pController); aStates.push_back(theCtrl );

      list<ISceneNode *>::Iterator it;

      CRearView *pRearView=NULL;

      if (bRearCam) pRearView=new CRearView(m_pDevice,"rearview.jpg",m_pSmgr->addCameraSceneNode());

      for (it=lPlanes.begin(); it!=lPlanes.end(); it++) {
        CPlane *p=new CPlane(m_pDevice,*it,m_pController,NULL,pRearView);
        CCockpitPlane *pCockpit=new CCockpitPlane(m_pDevice,"instruments",p->getBody());
        p->setCtrl((const u32 *)m_iCtrls[2]);
        p->setCockpit(pCockpit);
        aStates.push_back(p);
        m_pMenu->addButtonForState(p);
        m_lCockpits.push_back(pCockpit);
      }

      for (it=lCars.begin(); it!=lCars.end(); it++) {
        CCar *p=new CCar(m_pDevice,*it,m_pController,pRearView);
        CCockpitCar *pCarCockpit=new CCockpitCar(m_pDevice,"z_instru.jpg",p->getBody());
        p->setCockpit(pCarCockpit);
        p->setCtrl((const u32 *)m_iCtrls[0]);
        aStates.push_back(p);
        m_pMenu->addButtonForState(p);
        m_lCockpits.push_back(pCarCockpit);
      }

      for (it=lTanks.begin(); it!=lTanks.end(); it++) {
        CTank *p=new CTank(m_pDevice,*it,m_pController);
        p->setCtrl((const u32 *)m_iCtrls[1]);
        aStates.push_back(p);
        m_pMenu->addButtonForState(p);
      }

      for (it=lHelis.begin(); it!=lHelis.end(); it++) {
        CHeli *p=new CHeli(m_pDevice,*it,m_pController,pRearView);
        CCockpitPlane *pCockpit=new CCockpitPlane(m_pDevice,"instruments",p->getBody());
        p->setCtrl((const u32 *)m_iCtrls[2]);
        p->setCockpit(pCockpit);
        aStates.push_back(p);
        m_pMenu->addButtonForState(p);
        m_lCockpits.push_back(pCockpit);
      }

      //phyiscs initialization
      irr::ode::CIrrOdeManager::getSharedInstance()->initPhysics();

      delete pProg;


      //set the menu state to active
      m_pActive=m_pMenu;
      m_pActive->activate();

      CConfigFileManager::getSharedInstance()->loadConfig(m_pDevice,"../../data/irrOdeCarControls.xml");

      list<irr::ode::CIrrOdeSceneNode *> lNodes=irr::ode::CIrrOdeManager::getSharedInstance()->getIrrOdeNodes();
      list<irr::ode::CIrrOdeSceneNode *>::Iterator nit;

      for (nit=lNodes.begin(); nit!=lNodes.end(); nit++) {
        irr::ode::CIrrOdeSceneNode *p=*nit;
        if (!p->physicsInitialized()) printf("\t\t--> %i (%s)\n",p->getID(),p->getName());
      }

      m_pDriver->setFog(g_cFogColor,video::EFT_FOG_LINEAR,g_fMinFog,g_fMaxFog,0.00001f,true,false);
      enableFog(m_pSmgr->getRootSceneNode());

      u32 iFrames=0,iTotalFps=0;

      //let's run the loop
      while(m_pDevice->run()) {
        //step the simulation
        irr::ode::CIrrOdeManager::getSharedInstance()->step();

        //call the update method of the currently active state
        u32 iSwitch=m_pActive->update();

        if (m_bSwitchToMenu) {
          iSwitch = 1;
          m_bSwitchToMenu = false;
        }

        //change the active state if wished, i.e. a value other than zero was returned
        if (iSwitch) {
          iSwitch--;
          m_pActive->deactivate();
          m_pActive=aStates[iSwitch];
          m_pActive->activate();
          m_pCamCtrl->setTarget(m_pActive->getBody());
          iSwitch=0;
        }

        m_pCamCtrl->update();

        //now for the normal Irrlicht stuff ... begin, draw and end scene and update window caption
        m_pDriver->beginScene(true,true,video::SColor(0xFF,0xA0,0xA0,0xC0));

        m_pSmgr->drawAll();
        m_pGui->drawAll();

        m_pActive->drawSpecifics();

        m_pDriver->endScene();
        int fps = m_pDriver->getFPS();

        if (lastFPS != fps) {
          iFrames++;
          iTotalFps+=fps;

          core::stringw str = L"Irrlicht Engine - IrrODE Car Demo [";
          str += m_pDriver->getName();
          str += "] FPS:";
          str += fps;
          str += " (avg: ";
          str += (iTotalFps/iFrames);
          str += ")";

          m_pDevice->setWindowCaption(str.c_str());
          lastFPS = fps;
        }
        if (m_pSaveFile->isVisible() && m_pDevice->getTimer()->getTime()>m_iCount) m_pSaveFile->setVisible(false);
      }

      CConfigFileManager::getSharedInstance()->writeConfig(m_pDevice,"../../data/irrOdeCarControls.xml");
      irr::ode::CIrrOdeWorldObserver::getSharedInstance()->destall();

      //drop the world so it is destroyed
      m_pDevice->drop();

      if (m_pSndEngine) m_pSndEngine->drop();

      //and now some more cleanup...
      for (u32 i=0; i<aStates.size(); i++) {
        delete aStates[i];
      }
      aStates.clear();
    }

    virtual bool OnEvent(const irr::SEvent &event) {
      if (m_pController!=NULL) m_pController->OnEvent(event);
      if (m_pCamCtrl   !=NULL) m_pCamCtrl   ->OnEvent(event);

      if (m_pActive) m_pActive->OnEvent(event);

      if (event.EventType==irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.PressedDown) {
          switch (event.KeyInput.Key) {
            //if F1 is pressed the help text should be toggled
            case irr::KEY_F1:
              m_bHelp=!m_bHelp;
              return true;
              break;

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

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType() == irr::ode::eIrrOdeEventStep) {
        m_pMenu->setVisible(false);
        irr::core::list<IRenderToTexture *>::Iterator it;
        for (it = m_lCockpits.begin(); it!=m_lCockpits.end(); it++) (*it)->update();
        m_pMenu->setVisible(true);
      }

      return false;
    }

    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
      return pEvent->getType() == irr::ode::eIrrOdeEventStep;
    }
};

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
  irr::IrrlichtDevice *pDevice=pSettings->createDeviceFromSettings();
  pDevice->setWindowCaption(L"IrrODE car demo");

  CIrrOdeCar CProgram(pDevice);
  CProgram.run(pSettings);

  return 0;
}


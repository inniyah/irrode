  #include <irrlicht.h>
  #include <IrrOde.h>
  #include <CSettings.h>
#ifndef NO_IRRKLANG
  #include <irrKlang.h>
#endif
  #include <CRearView.h>

  #include <CEventVehicleState.h>
  #include <CIrrOdeCarState.h>
  #include <CProgressBar.h>
  #include <irrCC.h>
  #include <CProjectile.h>
  #include <CCustomEventReceiver.h>
  #include <CRandomForestNode.h>
  #include <CMeshCombiner.h>
  #include <CCockpitPlane.h>
  #include <CCockpitCar.h>
  #include <CRoadMeshLoader.h>
  #include <CControlReceiver.h>
  #include <CCameraController.h>
  #include <thread/IThread.h>

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

class CProgress : public ode::IIrrOdeEventListener {
  protected:
    IrrlichtDevice *m_pDevice;
    video::IVideoDriver *m_pDriver;
    gui::IGUIEnvironment *m_pGuienv;

    gui::IGUIImage *m_pImg;
    gui::IGUIStaticText *m_pText;
    CProgressBar *m_pBar;

  public:
    CProgress(IrrlichtDevice *pDevice) {
      m_pDevice=pDevice;
      m_pDriver=pDevice->getVideoDriver();
      m_pGuienv=pDevice->getGUIEnvironment();

      //let's add a "please stand by" image filling the complete window / screen
      core::dimension2du cScreenSize=m_pDevice->getVideoDriver()->getScreenSize();

      m_pImg=m_pGuienv->addImage(core::rect<s32>(0,0,cScreenSize.Width,cScreenSize.Height));
      m_pImg->setScaleImage(true);
      m_pImg->setImage(m_pDriver->getTexture(DATADIR "/textures/standby.png"));

      m_pBar=new CProgressBar(m_pGuienv,core::rect<s32>(core::position2di(cScreenSize.Width/2-150,cScreenSize.Height-60),core::dimension2di(300,30)),-1,NULL);
      m_pText=m_pGuienv->addStaticText(L"",core::rect<s32>(core::position2di(cScreenSize.Width/2-150,cScreenSize.Height-60),core::dimension2di(300,30)),true);
      m_pText->setBackgroundColor(video::SColor(192,192,192,192));
      m_pText->setDrawBackground(true);
      m_pText->setTextAlignment(gui::EGUIA_CENTER,gui::EGUIA_CENTER);

      ode::CIrrOdeManager::getSharedInstance()->getIrrThread()->getInputQueue()->addEventListener(this);
    }

    virtual ~CProgress() {
      m_pImg->remove();
      m_pText->remove();
      m_pBar->remove();
      ode::CIrrOdeManager::getSharedInstance()->getIrrThread()->getInputQueue()->removeEventListener(this);
    }

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent) {
      bool bRet=false;

      if (pEvent->getType()==ode::eIrrOdeEventProgress) {
        ode::CIrrOdeEventProgress *pEvt=reinterpret_cast<ode::CIrrOdeEventProgress *>(pEvent);
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

        m_pDriver->beginScene(true, true, video::SColor(0,200,200,200));
        m_pGuienv->drawAll();
        m_pDriver->endScene();
      }

      return bRet;
    }

    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent) {
      return pEvent->getType()==ode::eIrrOdeEventProgress;
    }
};

void enableFog(scene::ISceneNode *pNode) {
  if (pNode->getType()!=scene::ESNT_SKY_BOX)
    for (u32 i=0; i<pNode->getMaterialCount(); i++) {
      pNode->getMaterial(i).setFlag(video::EMF_FOG_ENABLE,true);
    }

  core::list<scene::ISceneNode *> lChildList=pNode->getChildren();
  core::list<scene::ISceneNode *>::Iterator it;

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

class CIrrOdeCar : public IEventReceiver {
  private:
    IrrlichtDevice *m_pDevice;

    video::IVideoDriver  *m_pDriver;
    scene::ISceneManager *m_pSmgr;
    gui::IGUIEnvironment *m_pGui;

    irrklang::ISoundEngine *m_pSndEngine;

    core::list<IRenderToTexture *> m_lCockpits;

    CControlReceiver *m_pCtrlReceiver;

    void fillBodyList(core::list<scene::ISceneNode *> &aPlanes, scene::ISceneNode *pNode, const c8 *sClassName, u32 iMax, ode::CIrrOdeWorld *pWorld) {
      if (pNode->getType()==ode::IRR_ODE_BODY_ID) {
        ode::CIrrOdeBody *p=(ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case(sClassName)) {
          printf("%s found (%i)\n",sClassName,aPlanes.size());
          if (aPlanes.size()<iMax)
            aPlanes.push_back(pNode);
          else {
            m_pCtrlReceiver->removeFromScene(pNode);
            return;
          }
        }
      }

      core::list<scene::ISceneNode *> children=pNode->getChildren();
      core::list<scene::ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) fillBodyList(aPlanes,*it,sClassName,iMax, pWorld);
    }

  public:
    CIrrOdeCar(IrrlichtDevice *pDevice) {
      m_pDevice = pDevice;

      m_pDriver = m_pDevice->getVideoDriver  ();
      m_pSmgr   = m_pDevice->getSceneManager  ();
      m_pGui    = m_pDevice->getGUIEnvironment();

      m_pCtrlReceiver = NULL;

#ifndef NO_IRRKLANG
      m_pSndEngine=irrklang::createIrrKlangDevice();
#else
      m_pSndEngine=NULL;
#endif

      ode::CIrrOdeManager::getSharedInstance()->install(m_pDevice);
      ode::CIrrOdeWorldObserver::getSharedInstance()->install();

      CCustomEventReceiver::setMembers(m_pDevice,ode::CIrrOdeManager::getSharedInstance(),m_pSndEngine);
      CCustomEventReceiver::getSharedInstance()->install();

      m_pDevice->setEventReceiver(this);
    }

    ~CIrrOdeCar() {
    }

    void run(CSettings *pSettings) {
      CProgress *pProg=new CProgress(m_pDevice);

      ode::CIrrOdeEventProgress *p=new ode::CIrrOdeEventProgress(0,0);
      pProg->onEvent(p);
      delete p;

      //register the IrrOde scene node factory
      ode::CIrrOdeSceneNodeFactory cFactory(m_pSmgr);
      m_pSmgr->registerSceneNodeFactory(&cFactory);

      if (pSettings->isActive(4)) {
        CEventInstallRandomForestPlugin *p=new CEventInstallRandomForestPlugin();
        ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
        //delete p;
      }

#ifdef WITH_PARTICLES
      CAdvancedParticleSystemNodeFactory *cParticleFactory=new CAdvancedParticleSystemNodeFactory(m_pSmgr);
      m_pSmgr->registerSceneNodeFactory(cParticleFactory);
      cParticleFactory->drop();
#endif

      CRoadMeshLoader *pLoader=new CRoadMeshLoader(m_pDevice);
      m_pSmgr->addExternalMeshLoader(pLoader);

      //init the ODE
      ode::CIrrOdeManager::getSharedInstance()->initODE();

      //load the scene
      ode::CIrrOdeManager::getSharedInstance()->loadScene(DATADIR "/scenes/IrrOdeCar.xml",m_pSmgr);

      for (u32 i=0; i<m_pSmgr->getMeshCache()->getMeshCount(); i++) {
        scene::IAnimatedMesh *p=m_pSmgr->getMeshCache()->getMeshByIndex(i);
        for (u32 j=0; j<p->getMeshBufferCount(); j++) {
          p->getMeshBuffer(j)->setHardwareMappingHint(scene::EHM_STATIC);
        }
      }

      u32 iCars   = pSettings->getCountOf(0),
               iPlanes = pSettings->getCountOf(1),
               iTanks  = pSettings->getCountOf(2),
               iHelis  = pSettings->getCountOf(3);

      bool bRearCam=pSettings->isActive(6);

      core::dimension2du cScreenSize=m_pDevice->getVideoDriver()->getScreenSize();

      m_pCtrlReceiver = new CControlReceiver(m_pDevice, m_pSndEngine, cScreenSize.Width / (2.0f * cScreenSize.Height));

      if (!pSettings->isActive(0)) m_pCtrlReceiver->removeFromScene("roads"       ,m_pSmgr);
      if (!pSettings->isActive(2)) m_pCtrlReceiver->removeFromScene("targets"     ,m_pSmgr);
      if (!pSettings->isActive(3)) m_pCtrlReceiver->removeFromScene("plane_course",m_pSmgr);
      if (!pSettings->isActive(1)) {
        m_pCtrlReceiver->removeFromScene("ActiveSigns" ,m_pSmgr);
        m_pCtrlReceiver->removeFromScene("PassiveSigns",m_pSmgr);
      }
      if ( pSettings->isActive(4)) {
       const c8 sForests[][255]={ "RandomForest1", "RandomForest2", "Forest1", "Forest2" };

        for (u32 i=0; i<2; i++) {
          printf("merging \"%s\"...\n",sForests[i]);
          scene::ISceneNode *p=m_pSmgr->getSceneNodeFromName(sForests[i]);
          CRandomForest *pForest=(CRandomForest *)p;
          if (pForest!=NULL) {
            CMeshCombiner *pCombine=new CMeshCombiner(0.8f);
            core::array<scene::IMeshSceneNode *> aMeshSceneNodes;
            core::array<scene::ISceneNode *> aTrees=pForest->getGeneratedTrees();

            for (u32 j=0; j<aTrees.size(); j++) {
              scene::IMeshSceneNode *p=(scene::IMeshSceneNode *)aTrees[j];
              aMeshSceneNodes.push_back(p);
            }

            printf("%i trees\n",aMeshSceneNodes.size());

            if (aMeshSceneNodes.size()>0) {
              c8 s[0xFF];
              sprintf(s,"MyCombinedTrees_%i",i);
              scene::IMesh *pCombined=pCombine->combineMeshes(m_pSmgr,m_pDriver,aMeshSceneNodes,s);
              if (pCombined!=NULL) {
                scene::ISceneNode *pRoot=m_pSmgr->getSceneNodeFromName(sForests[i+2]);
                scene::IMeshSceneNode *pNode=m_pSmgr->addMeshSceneNode(pCombined,pRoot==NULL?m_pSmgr->getRootSceneNode():pRoot);
                for (u32 i=0; i<pNode->getMaterialCount(); i++) {
                  pNode->getMaterial(i).setFlag(video::EMF_LIGHTING,false);
                  pNode->getMaterial(i).MaterialType=video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
                }
              }
            }
          }
        }
      }
      if (!pSettings->isActive(5)) {
        printf("removing terrain trimesh...\n");
        m_pCtrlReceiver->removeFromScene("terrain_trimesh",m_pSmgr);
      }
      else {
        printf("removing terrain heightfield...\n");
        m_pCtrlReceiver->removeFromScene("terrain_heightfield",m_pSmgr);
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
              DATADIR "/shaders/opengl.vert","vertexMain", video::EVST_VS_1_1,
              DATADIR "/shaders/opengl.frag", "pixelMain", video::EPST_PS_1_1,
              pCallback,video::EMT_SOLID);

          replaceMaterials(m_pSmgr->getRootSceneNode(),iNewMaterial);
          pCallback->drop();
          printf("Ready.\n\n");
        }
      }

      delete pSettings;

      //modify the textures of the car segment and the tank segment to
      scene::IAnimatedMeshSceneNode *pNode=(scene::IAnimatedMeshSceneNode *)m_pSmgr->getSceneNodeFromName("car_segment");
      if (pNode) pNode->getMaterial(0).getTextureMatrix(0).setTextureScale(50.0f,50.0f);
      pNode=(scene::IAnimatedMeshSceneNode *)m_pSmgr->getSceneNodeFromName("tank_segment");
      if (pNode) pNode->getMaterial(0).getTextureMatrix(0).setTextureScale(50.0f,50.0f);

      int lastFPS=-1;

      //create the necessary state objects
      core::array<CIrrOdeCarState *> aStates;

      m_pCtrlReceiver->createMenu(iCars, iPlanes, iHelis, iTanks, bRearCam);

      //phyiscs initialization
      ode::CIrrOdeManager::getSharedInstance()->initPhysics();

      delete pProg;

      m_pDriver->setFog(g_cFogColor,video::EFT_FOG_LINEAR,g_fMinFog,g_fMaxFog,0.00001f,true,false);
      enableFog(m_pSmgr->getRootSceneNode());

      u32 iFrames=0,iTotalFps=0;
      m_pCtrlReceiver->start();

      CCameraController *pCamCtrl = m_pCtrlReceiver->getCameraController();

      //let's run the loop
      while(m_pDevice->run()) {
        //step the simulation
        ode::CIrrOdeManager::getSharedInstance()->step();

        m_pCtrlReceiver->update();

        //now for the normal Irrlicht stuff ... begin, draw and end scene and update window caption
        m_pDriver->beginScene(true,true,video::SColor(0xFF,0xA0,0xA0,0xC0));
        pCamCtrl->render();
        m_pDriver->setMaterial(m_pDriver->getMaterial2D());   //Fix the flipped texture problem
        m_pGui->drawAll();
        m_pCtrlReceiver->drawSpecifics();

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

          pCamCtrl->setFps(str.c_str());
        }
      }

      ode::CIrrOdeWorldObserver::getSharedInstance()->destall();

      CConfigFileManager::getSharedInstance()->writeConfig(m_pDevice,DATADIR "/irrOdeCarControls.xml");

#ifndef NO_IRRKLANG
      //drop the world so it is destroyed
      m_pDevice->drop();

      if (m_pSndEngine) m_pSndEngine->drop();
#endif

      //and now some more cleanup...
      for (u32 i=0; i<aStates.size(); i++) {
        delete aStates[i];
      }
      aStates.clear();
    }

    virtual bool OnEvent(const SEvent &event) {
      if (m_pCtrlReceiver!=NULL) m_pCtrlReceiver->OnEvent(event);
      return false;
    }
};

int main(int argc, char** argv) {
  //First thing to do: show the graphics options dialog to let the user choose the graphics options
  CSettings *pSettings=new CSettings(DATADIR "/irrOdeCarSettings.xml",L"irrOdeCar - Graphics Setup",video::SColor(0x00,0x21,0xAD,0x10));

  pSettings->setMinResolution(core::dimension2du(640,480));

  u32 iRet=pSettings->run();
  if (iRet==2) {
    delete pSettings;
    return 0;
  }

  //now create the Irrlicht device from the chosen options
  IrrlichtDevice *pDevice=pSettings->createDeviceFromSettings();
  pDevice->setWindowCaption(L"IrrODE car demo");

  CIrrOdeCar CProgram(pDevice);
  CProgram.run(pSettings);

  return 0;
}


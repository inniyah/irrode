  #include <irrlicht.h>
  #include <IrrODE.h>
  #include <CSettings.h>
  #include <CAdvancedParticleSystemNode.h>
  #include <irrKlang.h>
  #include <CRearView.h>

  #include "CCar.h"
  #include "CTank.h"
  #include "CHeli.h"
  #include "CPlane.h"
  #include "CController.h"
  #include "CIrrOdeCarState.h"
  #include "CProgressBar.h"
  #include "irrCC.h"
  #include "CProjectile.h"
  #include "CCustomEventReceiver.h"
  #include <CRandomForestNode.h>
  #include <CMeshCombiner.h>
  #include <CCockpitPlane.h>
  #include <CCockpitCar.h>
  #include <CRoadMeshLoader.h>
  #include <CControlReceiver.h>

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

class CIrrOdeCar : public irr::IEventReceiver {
  private:
    irr::IrrlichtDevice *m_pDevice;

    video::IVideoDriver  *m_pDriver;
    scene::ISceneManager *m_pSmgr;
    gui::IGUIEnvironment *m_pGui;

    irrklang::ISoundEngine *m_pSndEngine;

    irr::core::list<IRenderToTexture *> m_lCockpits;

    CControlReceiver *m_pCtrlReceiver;

    void fillBodyList(irr::core::list<ISceneNode *> &aPlanes, ISceneNode *pNode, const c8 *sClassName, u32 iMax, irr::ode::CIrrOdeWorld *pWorld) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
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

      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) fillBodyList(aPlanes,*it,sClassName,iMax, pWorld);
    }

  public:
    CIrrOdeCar(irr::IrrlichtDevice *pDevice) {
      m_pDevice = pDevice;

      m_pDriver = m_pDevice->getVideoDriver  ();
      m_pSmgr   = m_pDevice->getSceneManager  ();
      m_pGui    = m_pDevice->getGUIEnvironment();

      m_pCtrlReceiver = NULL;

      m_pSndEngine=irrklang::createIrrKlangDevice();

      irr::ode::CIrrOdeManager::getSharedInstance()->install(m_pDevice);
      irr::ode::CIrrOdeWorldObserver::getSharedInstance()->install();

      CCustomEventReceiver::setMembers(m_pDevice,irr::ode::CIrrOdeManager::getSharedInstance(),m_pSndEngine);
      CCustomEventReceiver::getSharedInstance()->install();

      m_pDevice->setEventReceiver(this);
    }

    ~CIrrOdeCar() {
    }

    void run(CSettings *pSettings) {
      CProgress *pProg=new CProgress(m_pDevice);

      irr::ode::CIrrOdeEventProgress *p=new irr::ode::CIrrOdeEventProgress(0,0);
      pProg->onEvent(p);
      delete p;

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

      irr::u32 iCars   = pSettings->getCountOf(0),
               iPlanes = pSettings->getCountOf(1),
               iTanks  = pSettings->getCountOf(2),
               iHelis  = pSettings->getCountOf(3);

      bool bRearCam=pSettings->isActive(6);

      m_pCtrlReceiver = new CControlReceiver(m_pDevice, new irr::ode::CIrrOdeEventQueue(), m_pSndEngine);

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
              "../../data/shaders/opengl.vert","vertexMain", video::EVST_VS_1_1,
              "../../data/shaders/opengl.frag", "pixelMain", video::EPST_PS_1_1,
              pCallback,video::EMT_SOLID);

          replaceMaterials(m_pSmgr->getRootSceneNode(),iNewMaterial);
          pCallback->drop();
          printf("Ready.\n\n");
        }
      }

      delete pSettings;

      //modify the textures of the car segment and the tank segment to
      IAnimatedMeshSceneNode *pNode=(IAnimatedMeshSceneNode *)m_pSmgr->getSceneNodeFromName("car_segment");
      if (pNode) pNode->getMaterial(0).getTextureMatrix(0).setTextureScale(50.0f,50.0f);
      pNode=(IAnimatedMeshSceneNode *)m_pSmgr->getSceneNodeFromName("tank_segment");
      if (pNode) pNode->getMaterial(0).getTextureMatrix(0).setTextureScale(50.0f,50.0f);

      int lastFPS=-1;

      //create the necessary state objects
      array<CIrrOdeCarState *> aStates;

      m_pCtrlReceiver->createMenu(iCars, iPlanes, iHelis, iTanks, bRearCam);

      //phyiscs initialization
      irr::ode::CIrrOdeManager::getSharedInstance()->initPhysics();

      delete pProg;


      //set the menu state to active
      //m_pActive=m_pMenu;
      //m_pActive->activate();

      m_pDriver->setFog(g_cFogColor,video::EFT_FOG_LINEAR,g_fMinFog,g_fMaxFog,0.00001f,true,false);
      enableFog(m_pSmgr->getRootSceneNode());

      u32 iFrames=0,iTotalFps=0;
      m_pCtrlReceiver->start();

      //let's run the loop
      while(m_pDevice->run()) {
        //step the simulation
        irr::ode::CIrrOdeManager::getSharedInstance()->step();

        m_pCtrlReceiver->update();

        //now for the normal Irrlicht stuff ... begin, draw and end scene and update window caption
        m_pDriver->beginScene(true,true,video::SColor(0xFF,0xA0,0xA0,0xC0));

        m_pSmgr->drawAll();

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
        }
      }

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
      if (m_pCtrlReceiver!=NULL) m_pCtrlReceiver->OnEvent(event);
      return false;
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


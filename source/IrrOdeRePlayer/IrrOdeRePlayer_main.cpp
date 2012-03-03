  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <CPluginInfo.h>
  #include <IState.h>

  #include <CReplayerStateReplay.h>
  #include <SSharedManagers.h>

  #include <irrklang.h>

//Note: move the IrrOde lib to be the first linked lib if you get
//the unresolved external of the IdentityMaterial

int main(int argc, char** argv) {
  irr::IrrlichtDevice *device =irr::createDevice(irr::video::EDT_OPENGL,irr::core::dimension2d<irr::u32>(1024,768),24,false,false,false,0);
  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);

  device->setWindowCaption(L"IrrOdeRePlayer");

  irr::video::IVideoDriver  *driver = device->getVideoDriver();
  irr::scene::ISceneManager *smgr   = device->getSceneManager();
  irr::gui::IGUIEnvironment *guienv = device->getGUIEnvironment();

  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);
  irr::ode::CIrrOdeSceneNodeFactory cFactory(smgr);
  smgr->registerSceneNodeFactory(&cFactory);

  CPluginInfo *pPlugin=new CPluginInfo("RePlayerPlugins/RePlayerPlugin.dll",device);
  irrklang::ISoundEngine *pSndEngine=irrklang::createIrrKlangDevice();

  SSharedManagers cManagers;
  cManagers.m_pOdeManager=(void *)irr::ode::CIrrOdeManager::getSharedInstance();
  cManagers.m_pEventFactory=(void *)irr::ode::CIrrOdeEventFactory::getSharedEventFactory();
  cManagers.m_pSndEngine=(void *)pSndEngine;

  if (pPlugin->dllLoaded())
    pPlugin->pluginInstall(&cManagers);
  else
    printf("Plugin not installed!\n");

  IState *pActiveState=new CReplayerStateReplay(device,"../../data/replay/car.rec");
  pActiveState->activate();

  irr::u32 iRet=0;
  irr::s32 lastFPS=-1;

  while(iRet==0 && device->run()) {
    iRet=pActiveState->update();

    irr::core::vector3df vPos=smgr->getActiveCamera()->getPosition(),
                         vTgt=smgr->getActiveCamera()->getTarget(),
                         vUp =smgr->getActiveCamera()->getUpVector();

    irrklang::vec3df vLstPos=irrklang::vec3df(vPos.X,vPos.Y,vPos.Z),
                     vLstTgt=irrklang::vec3df(vTgt.X,vTgt.Y,vTgt.Z),
                     vLstUp =irrklang::vec3df(vUp .X,vUp .Y,vUp .Z);

    pSndEngine->setListenerPosition(vLstPos,vLstTgt,irrklang::vec3df(0.0f,0.0f,0.0f),vLstUp);
    pSndEngine->setRolloffFactor(0.125f);

    driver->beginScene(true, true, irr::video::SColor(0,200,200,200));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
    int fps = driver->getFPS();

    if (lastFPS != fps) {
      irr::core::stringw s="IrrOdeRePlayer - ";
      s+=fps;
      s+=" Frames per Second";
      device->setWindowCaption(s.c_str());
    }
  }

  pActiveState->deactivate();

  if (pPlugin->dllLoaded())
    pPlugin->pluginDestall(&cManagers);

  if (pSndEngine) pSndEngine->drop();
  delete pActiveState;
  device->drop();

  return 0;
}


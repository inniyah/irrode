  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <CPluginInfo.h>
  #include <IState.h>

  #include <CReplayerStateReplay.h>
  #include <SSharedManagers.h>

  #include <irrklang.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//Note: move the IrrOde lib to be the first linked lib if you get
//the unresolved external of the IdentityMaterial

int main(int argc, char** argv) {
  irr::IrrlichtDevice *device =createDevice(EDT_DIRECT3D9,dimension2d<u32>(1024,768),24,false,false,false,0);
  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);

  device->setWindowCaption(L"IrrOdeRePlayer");

  IVideoDriver* driver = device->getVideoDriver();
  ISceneManager* smgr = device->getSceneManager();
  IGUIEnvironment* guienv = device->getGUIEnvironment();

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

  u32 iRet=0;
  s32 lastFPS=-1;

  while(iRet==0 && device->run()) {
    iRet=pActiveState->update();

    core::vector3df vPos=smgr->getActiveCamera()->getPosition(),
                    vTgt=smgr->getActiveCamera()->getTarget(),
                    vUp =smgr->getActiveCamera()->getUpVector();

    irrklang::vec3df vLstPos=irrklang::vec3df(vPos.X,vPos.Y,vPos.Z),
                     vLstTgt=irrklang::vec3df(vTgt.X,vTgt.Y,vTgt.Z),
                     vLstUp =irrklang::vec3df(vUp .X,vUp .Y,vUp .Z);

    pSndEngine->setListenerPosition(vLstPos,vLstTgt,irrklang::vec3df(0.0f,0.0f,0.0f),vLstUp);
    pSndEngine->setRolloffFactor(0.125f);

    driver->beginScene(true, true, SColor(0,200,200,200));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
    int fps = driver->getFPS();

    if (lastFPS != fps) {
      core::stringw s="IrrOdeRePlayer - ";
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


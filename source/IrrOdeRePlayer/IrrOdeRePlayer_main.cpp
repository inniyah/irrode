  #include <irrlicht.h>
  #include <IrrOde.h>
  #include <observer/CIrrOdeRePlayer.h>

  #include <CPluginInfo.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//Note: move the IrrOde lib to be the first linked lib if you get
//the unresolved external of the IdentityMaterial

int main(int argc, char** argv) {
  irr::IrrlichtDevice *device =createDevice(EDT_DIRECT3D9,dimension2d<u32>(800,600),24,false,false,false,0);
  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);

  device->setWindowCaption(L"IrrOdeRePlayer");

  IVideoDriver* driver = device->getVideoDriver();
  ISceneManager* smgr = device->getSceneManager();
  IGUIEnvironment* guienv = device->getGUIEnvironment();

  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);
  irr::ode::CIrrOdeSceneNodeFactory cFactory(smgr);
  smgr->registerSceneNodeFactory(&cFactory);

  ICameraSceneNode *pCam=smgr->addCameraSceneNodeFPS();

  CPluginInfo *pPlugin=new CPluginInfo("RePlayerPlugins/RePlayerPlugin.dll",device);
  if (pPlugin->dllLoaded())
    pPlugin->pluginInstall((void *)irr::ode::CIrrOdeManager::getSharedInstance());
  else
    printf("Plugin not installed!\n");

  irr::ode::CIrrOdeRePlayer *pPlayer=new irr::ode::CIrrOdeRePlayer(device,"../../data/replay/car.rec");

  vector3df v=pCam->getAbsolutePosition();
  smgr->setActiveCamera(pCam);

  while(device->run()) {
    pPlayer->update();
    pCam->setParent(smgr->getRootSceneNode());
    vector3df v=pCam->getAbsolutePosition();
    v=pCam->getPosition();

    driver->beginScene(true, true, SColor(0,200,200,200));


    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
  }

  if (pPlugin->dllLoaded())
    pPlugin->pluginDestall((void *)irr::ode::CIrrOdeManager::getSharedInstance());

  device->drop();

  return 0;
}


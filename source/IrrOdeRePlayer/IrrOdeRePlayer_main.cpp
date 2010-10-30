  #include <irrlicht.h>
  #include <IrrOde.h>
  #include <CIrrOdeRePlayer.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//Note: move the IrrOde lib to be the first linked lib if you get
//the unresolved external of the IdentityMaterial

int main(int argc, char** argv) {
  IrrlichtDevice *device =createDevice(EDT_DIRECT3D9,dimension2d<u32>(800,600),24,false,false,false,0);
  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);

  device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

  IVideoDriver* driver = device->getVideoDriver();
  ISceneManager* smgr = device->getSceneManager();
  IGUIEnvironment* guienv = device->getGUIEnvironment();

  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);
  irr::ode::CIrrOdeSceneNodeFactory cFactory(smgr);
  smgr->registerSceneNodeFactory(&cFactory);

  CIrrOdeRePlayer *pPlayer=new CIrrOdeRePlayer(device,"../../data/replay/car.rec");

  ICameraSceneNode *pCam=smgr->addCameraSceneNodeFPS();
  vector3df v=pCam->getAbsolutePosition();
  smgr->setActiveCamera(pCam);
  printf("--> %.2f, %.2f, %.2f\n",v.X,v.Y,v.Z);

  while(device->run()) {
    pPlayer->update();
    pCam->setParent(smgr->getRootSceneNode());
    vector3df v=pCam->getAbsolutePosition();
    printf("==> %.2f, %.2f, %.2f\n",v.X,v.Y,v.Z);
    v=pCam->getPosition();
    printf("--> %.2f, %.2f, %.2f\n",v.X,v.Y,v.Z);

    driver->beginScene(true, true, SColor(0,200,200,200));


    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
  }
  device->drop();

  return 0;
}


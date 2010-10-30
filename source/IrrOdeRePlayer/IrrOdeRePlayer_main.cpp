  #include <irrlicht.h>
  #include <IrrOde.h>
  #include <CIrrOdeReplayLoader.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//Note: move the IrrOde lib to be the first linked lib if you get
//the unresolved external of the IdentityMaterial

int main(int argc, char** argv) {
  IrrlichtDevice *device =createDevice(EDT_DIRECT3D9,dimension2d<u32>(1024,768),24,false,false,false,0);

  device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

  IVideoDriver* driver = device->getVideoDriver();
  ISceneManager* smgr = device->getSceneManager();
  IGUIEnvironment* guienv = device->getGUIEnvironment();

  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);
  irr::ode::CIrrOdeSceneNodeFactory cFactory(smgr);
  smgr->registerSceneNodeFactory(&cFactory);

  CIrrOdeReplayLoader *pLoader=new CIrrOdeReplayLoader(device,"../../data/recordings/car.rec");

  smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));

  while(device->run()) {
    driver->beginScene(true, true, SColor(0,200,200,200));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
  }
  device->drop();

  return 0;
}


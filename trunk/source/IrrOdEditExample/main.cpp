  #include <irrlicht.h>
  #include <IrrOde.h>

int main(int argc, char** argv) {
  irr::IrrlichtDevice *device=irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(640,480),16,false,false,false,0);
  device->setWindowCaption(L"Hello World! - Irrlicht-ODE Engine Demo");

  irr::video::IVideoDriver *driver = device->getVideoDriver();
  irr::scene::ISceneManager *smgr = device->getSceneManager();
  irr::gui::IGUIEnvironment *guienv = device->getGUIEnvironment();
  irr::gui::ICursorControl *pCursor=device->getCursorControl();
  pCursor->setVisible(false);

  irr::ode::CIrrOdeManager::getSharedInstance()->install(device);

  irr::ode::CIrrOdeSceneNodeFactory cFactory(smgr);
  smgr->registerSceneNodeFactory(&cFactory);

  smgr->loadScene("../../data/IrrOdEdit_example.xml");

  //init the ODE
  irr::ode::CIrrOdeManager::getSharedInstance()->initODE();
  irr::ode::CIrrOdeManager::getSharedInstance()->initPhysics();

  irr::scene::ICameraSceneNode *cam=reinterpret_cast<irr::scene::ICameraSceneNode *>(smgr->getSceneNodeFromName("theCamera"));
  cam->setTarget(irr::core::vector3df(0,0,0));

  //let's run the loop
  while(device->run()) {
    //step the simulation
    irr::ode::CIrrOdeManager::getSharedInstance()->step();

    //now for the normal Irrlicht stuff ... begin, draw and end scene and update window caption
    driver->beginScene(true, true, irr::video::SColor(0,200,200,200));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
  }

  //drop the world so it is destroyed
  device->drop();

  return 0;
}


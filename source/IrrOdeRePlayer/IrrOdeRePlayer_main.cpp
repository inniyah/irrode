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

class COdeEventListener : public ode::IIrrOdeEventListener {
  protected:
    IGUIStaticText *m_pLblBodies;
    array<ode::CIrrOdeBody *> m_aBodies;
    ode::CIrrOdeManager *m_pOdeManager;
    ISceneManager *m_pSmgr;

    bool m_bSceneLoaded;

    void updateBodyList() {
      stringw s;
      for (u32 i=0; i<m_aBodies.size(); i++) {
        ode::CIrrOdeBody *p=m_aBodies[i];
        if (p!=NULL) {
          s+=p->getID();
          s+=" -- (";
          s+=p->getName();
          s+=")";
          s+="\n";
        }
      }
      m_pLblBodies->setText(s.c_str());
    }

    void removeNode(ISceneNode *pNode) {
      if (pNode==NULL) return;

      list<ISceneNode *> children=pNode->getChildren();
      list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) removeNode(*it);

      if (pNode->getType()==ode::IRR_ODE_BODY_ID) {
        ode::CIrrOdeBody *pBody=(ode::CIrrOdeBody *)pNode;
        for (u32 i=0; i<m_aBodies.size(); i++) if (m_aBodies[i]==pBody) {
          m_aBodies.erase(i); break;
          updateBodyList();
        }
      }
    }
  public:
    COdeEventListener(IGUIStaticText *pLblBodies, ISceneManager *pSmgr) {
      m_pOdeManager=ode::CIrrOdeManager::getSharedInstance();
      m_pSmgr=pSmgr;
      m_pLblBodies=pLblBodies;
      m_bSceneLoaded=false;
    }

    ~COdeEventListener() {
      for (u32 i=0; i<m_aBodies.size(); i++) m_aBodies[i]->drop();
    }

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent) {
      if (!m_bSceneLoaded) {
        if (pEvent->getType()==ode::eIrrOdeEventLoadScene) {
          core::list<ode::CIrrOdeSceneNode *> lBodies=m_pOdeManager->getIrrOdeNodes();
          core::list<ode::CIrrOdeSceneNode *>::Iterator it;

          for (it=lBodies.begin(); it!=lBodies.end(); it++) {
            ode::CIrrOdeSceneNode *pNode=*it;
            if (pNode->getType()==ode::IRR_ODE_BODY_ID) {
              ode::CIrrOdeBody *pBody=(ode::CIrrOdeBody *)pNode;
              if (pBody->getParentBody()==NULL && pBody->getID()!=-1) {
                pBody->grab();
                m_aBodies.push_back(pBody);
              }
            }
          }
          updateBodyList();
          m_bSceneLoaded=true;
          return true;
        }
      }
      else {
        if (pEvent->getType()==ode::eIrrOdeEventNodeCloned) {
          ode::CIrrOdeEventNodeCloned *p=(ode::CIrrOdeEventNodeCloned *)pEvent;
          ISceneNode *pNode=m_pSmgr->getSceneNodeFromId(p->getNewId());
          if (pNode->getType()==ode::IRR_ODE_BODY_ID) {
            ode::CIrrOdeBody *pBody=(ode::CIrrOdeBody *)pNode;
            if (pBody->getParentBody()==NULL) {
              pBody->grab();
              m_aBodies.push_back(pBody);
            }
          }
          updateBodyList();
          return true;
        }

        if (pEvent->getType()==ode::eIrrOdeEventNodeRemoved) {
          ode::CIrrOdeEventNodeRemoved *p=(ode::CIrrOdeEventNodeRemoved *)pEvent;
          ISceneNode *pNode=m_pSmgr->getSceneNodeFromId(p->getRemovedNodeId());
          if (pNode!=NULL) removeNode(pNode);
          updateBodyList();
          return true;
        }
      }

      return false;
    }

    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent) {
      return pEvent->getType()==ode::eIrrOdeEventNodeCloned  ||
             pEvent->getType()==ode::eIrrOdeEventNodeRemoved ||
             pEvent->getType()==ode::eIrrOdeEventLoadScene;
    }
};

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

  ICameraSceneNode *pCam=smgr->addCameraSceneNodeFPS();

  CPluginInfo *pPlugin=new CPluginInfo("RePlayerPlugins/RePlayerPlugin.dll",device);
  if (pPlugin->dllLoaded())
    pPlugin->pluginInstall((void *)irr::ode::CIrrOdeManager::getSharedInstance());
  else
    printf("Plugin not installed!\n");

  IGUIStaticText *pLblBodies=guienv->addStaticText(L"Hello World",rect<s32>(5,5,200,300),true,true,0,-1,true);
  COdeEventListener *pListener=new COdeEventListener(pLblBodies,smgr);
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(pListener);

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

  ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(pListener);
  delete pListener;
  device->drop();

  return 0;
}


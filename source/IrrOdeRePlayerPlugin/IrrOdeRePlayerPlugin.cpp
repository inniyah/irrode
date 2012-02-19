  #include <IrrOdeRePLayerPlugin.h>

  #include <CAdvancedParticleSystemNode.h>

  #include <IrrOde.h>
  #include <observer/IEventFactory.h>
  #include <observer/CIrrOdeEventFactory.h>

  #include <CEventVehicleState.h>
  #include <SSharedManagers.h>
  #include <CCustomEventReceiver.h>

  #include <CRoadMeshLoader.h>
  #include <irrklang.h>
  #include <CMeshCombiner.h>
  #include <CRandomForestNode.h>

/**
 * We use the plugin to add an addition event listener to the IrrOdeRePlayer. This
 * listener must deactivate the particle systems of the missiles and shells so that
 * the replay looks like the original
 */
class APS_EventListener : public irr::ode::IIrrOdeEventListener {
  protected:
    irr::IrrlichtDevice *m_pDevice;


  public:
    APS_EventListener(irr::IrrlichtDevice *pDevice) {
      m_pDevice=pDevice;
    }

    void searchPlaneBodies(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case("plane")) {
          printf("\t\t#### add plane \"%s\"\n",pNode->getName());
          CCustomEventReceiver::getSharedInstance()->addPlane(p);
        }
      }
      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) searchPlaneBodies(*it);
    }

    void searchTankBodies(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case("tank")) {
          printf("\t\t#### add tank \"%s\"\n",pNode->getName());
          CCustomEventReceiver::getSharedInstance()->addTank(p);
        }
      }
      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) searchTankBodies(*it);
    }

    void searchCarBodies(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case("car")) {
          printf("\t\t#### add car \"%s\"\n",pNode->getName());
          CCustomEventReceiver::getSharedInstance()->addCar(p);
        }
      }
      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) searchCarBodies(*it);
    }

    void searchHeliBodies(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)pNode;
        if (p->getOdeClassname().equals_ignore_case("heli")) {
          printf("\t\t#### add heli\"%s\"\n",pNode->getName());
          CCustomEventReceiver::getSharedInstance()->addCar(p);
        }
      }
      irr::core::list<ISceneNode *> children=pNode->getChildren();
      irr::core::list<ISceneNode *>::Iterator it;

      for (it=children.begin(); it!=children.end(); it++) searchHeliBodies(*it);
    }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType()==irr::ode::eIrrOdeEventLoadScene) {
        searchPlaneBodies(m_pDevice->getSceneManager()->getRootSceneNode());
        searchTankBodies (m_pDevice->getSceneManager()->getRootSceneNode());
        searchCarBodies  (m_pDevice->getSceneManager()->getRootSceneNode());
        searchHeliBodies (m_pDevice->getSceneManager()->getRootSceneNode());

        const c8 sForests[][255]={ "RandomForest1", "RandomForest2", "Forest1", "Forest2" };

        for (u32 i=0; i<2; i++) {
          printf("merging \"%s\"...\n",sForests[i]);
          irr::scene::ISceneManager *smgr=m_pDevice->getSceneManager();
          scene::ISceneNode *p=smgr->getSceneNodeFromName(sForests[i]);
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
              irr::scene::IMesh *pCombined=pCombine->combineMeshes(smgr,m_pDevice->getVideoDriver(),aMeshSceneNodes,s);
              if (pCombined!=NULL) {
                irr::scene::ISceneNode *pRoot=smgr->getSceneNodeFromName(sForests[i+2]);
                irr::scene::IMeshSceneNode *pNode=smgr->addMeshSceneNode(pCombined,pRoot==NULL?smgr->getRootSceneNode():pRoot);
                for (u32 i=0; i<pNode->getMaterialCount(); i++) {
                  pNode->getMaterial(i).setFlag(irr::video::EMF_LIGHTING,false);
                  pNode->getMaterial(i).MaterialType=irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF;
                }
              }
            }
          }
        }
        return true;
      }

      return false;
    }

    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
      return pEvent->getType()==irr::ode::eIrrOdeEventLoadScene;
    }
};

class APS_EventFactory : public irr::ode::IIrrOdeEventFactory {
  public:
    virtual irr::ode::IIrrOdeEvent *createMessage(irr::ode::CSerializer *pData, irr::scene::ISceneManager *pSmgr) {
      pData->resetBufferPos();
      irr::u16 iCode=pData->getU16();

      if (iCode==EVENT_PLANE_STATE_ID) return new CEventPlaneState(pData);
      if (iCode==EVENT_TANK_STATE_ID ) return new CEventTankState (pData);
      if (iCode==EVENT_CAR_STATE_ID  ) return new CEventCarState  (pData);
      if (iCode==EVENT_HELI_STATE_ID ) return new CEventHeliState (pData);
      if (iCode==EVENT_FIRE_SND_ID   ) return new CEventFireSound (pData);

      if (iCode==EVENT_INST_FOREST_ID) return new CEventInstallRandomForestPlugin(pData);
      if (iCode==EVENT_LAP_TIME_ID   ) return new CEventLapTime                  (pData);

      return NULL;
    }
};


static CAdvancedParticleSystemNodeFactory *g_pFactory=NULL;
static APS_EventListener *g_pListener=NULL;
static APS_EventFactory *g_pEventFactory=NULL;

int DLL_EXPORT install(irr::IrrlichtDevice *pDevice, void *pUserData) {
  SSharedManagers *pManagers=(SSharedManagers *)pUserData;

  irr::ode::CIrrOdeManager *pMgr=((irr::ode::CIrrOdeManager *)pManagers->m_pOdeManager);
  irr::ode::CIrrOdeEventFactory *pFac=((irr::ode::CIrrOdeEventFactory *)pManagers->m_pEventFactory);
  irrklang::ISoundEngine *pSndEngine=((irrklang::ISoundEngine *)pManagers->m_pSndEngine);

  if (g_pFactory==NULL) {
    printf("registering scenenode factory...\n");
    g_pFactory=new CAdvancedParticleSystemNodeFactory(pDevice->getSceneManager());
    pDevice->getSceneManager()->registerSceneNodeFactory(g_pFactory);
  }

  printf("registering event listener...\n");
  g_pListener=new APS_EventListener(pDevice);
  pMgr->getQueue()->addEventListener(g_pListener);

  printf("registering event factory...\n");
  g_pEventFactory=new APS_EventFactory();
  pFac->registerEventFactory(g_pEventFactory);

  CRoadMeshLoader *pLoader=new CRoadMeshLoader(pDevice);
  pDevice->getSceneManager()->addExternalMeshLoader(pLoader);

  CCustomEventReceiver::setMembers(pDevice,pMgr,pSndEngine);
  CCustomEventReceiver::getSharedInstance()->install();

  return 0;
}

int DLL_EXPORT destall(irr::IrrlichtDevice *pDevice, void *pUserData) {
  SSharedManagers *pManagers=(SSharedManagers *)pUserData;

  ((irr::ode::CIrrOdeManager *)pManagers->m_pOdeManager)->getQueue()->removeEventListener(g_pListener);
  delete g_pListener;
  g_pListener=NULL;

  ((irr::ode::CIrrOdeEventFactory *)pManagers->m_pEventFactory)->removeEventFactory(g_pEventFactory);
  delete g_pEventFactory;
  g_pEventFactory=NULL;

  CCustomEventReceiver::getSharedInstance()->destall();

  return 0;
}

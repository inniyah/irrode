  #include <IrrOdeRePLayerPlugin.h>

  #include <CAdvancedParticleSystemNode.h>

  #include <IrrOde.h>
  #include <observer/IEventFactory.h>
  #include <observer/CIrrOdeEventFactory.h>

  #include <CEventVehicleState.h>
  #include <SSharedManagers.h>
  #include <CCustomEventReceiver.h>

  #include <CRoadMeshLoader.h>

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

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType()==irr::ode::eIrrOdeEventLoadScene) {
        searchPlaneBodies(m_pDevice->getSceneManager()->getRootSceneNode());
        searchTankBodies (m_pDevice->getSceneManager()->getRootSceneNode());
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

  CCustomEventReceiver::setMembers(pDevice,pMgr);
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

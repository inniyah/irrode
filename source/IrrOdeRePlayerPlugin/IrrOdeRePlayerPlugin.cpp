  #include <IrrOdeRePLayerPlugin.h>

  #include <CAdvancedParticleSystemNode.h>

  #include <IrrOde.h>
  #include <observer/IEventFactory.h>
  #include <observer/CIrrOdeEventFactory.h>

  #include <CEventRudderPositions.h>
  #include <SSharedManagers.h>

/**
 * We use the plugin to add an addition event listener to the IrrOdeRePlayer. This
 * listener must deactivate the particle systems of the missiles and shells so that
 * the replay looks like the original
 */
class APS_EventListener : public irr::ode::IIrrOdeEventListener {
  protected:
    irr::IrrlichtDevice *m_pDevice;

    /**
     * This method deactivates all particles systems that are children of a node
     * @param pNode the node
     */
    void deactivateParticleSystems(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==ADVANCED_PARTICLE_NODE_ID) {
        CAdvancedParticleSystemNode *p=(CAdvancedParticleSystemNode *)pNode;
        //We deactivate the normal particle systems and activate those for explosions
        //which will automatically be deactivated once finished.
        p->setIsActive(!p->isActive());
      }
      list<ISceneNode *> children=pNode->getChildren();
      list<ISceneNode *>::Iterator i;
      for (i=children.begin(); i!=children.end(); i++) deactivateParticleSystems(*i);
    }

    /**
     * This method hides the projectile which is an animated mesh
     * @param pNode the node to check
     */
    void hideAnimatedMesh(irr::scene::ISceneNode *pNode) {
      if (pNode->getType()==irr::scene::ESNT_ANIMATED_MESH) {
        irr::scene::IAnimatedMeshSceneNode *p=(irr::scene::IAnimatedMeshSceneNode *)pNode;
        p->setVisible(false);
      }
      list<ISceneNode *> children=pNode->getChildren();
      list<ISceneNode *>::Iterator i;
      for (i=children.begin(); i!=children.end(); i++) hideAnimatedMesh(*i);
    }

  public:
    APS_EventListener(irr::IrrlichtDevice *pDevice) {
      m_pDevice=pDevice;
    }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent) {
      if (pEvent->getType()==irr::ode::eIrrOdeEventBodyRemoved) {
        irr::ode::CIrrOdeEventBodyRemoved *p=(irr::ode::CIrrOdeEventBodyRemoved *)pEvent;
        ISceneNode *pNode=m_pDevice->getSceneManager()->getSceneNodeFromId(p->getBodyId());
        if (pNode!=NULL) {
          deactivateParticleSystems(pNode);
          hideAnimatedMesh(pNode);
        }
      }

      if (pEvent->getType()==EVENT_RUDDER_ID) {
        CEventRudderPositions *p=(CEventRudderPositions *)pEvent;
        irr::scene::ISceneManager *pMgr=m_pDevice->getSceneManager();

        irr::scene::ISceneNode *pYaw[2]={pMgr->getSceneNodeFromName("yaw"),pMgr->getSceneNodeFromName("yaw2") },
                               *pRoll[2]={pMgr->getSceneNodeFromName("roll1"),pMgr->getSceneNodeFromName("roll2") },
                               *pPitch[2]={pMgr->getSceneNodeFromName("pitch1"),pMgr->getSceneNodeFromName("pitch2") };

        u32 i;
        f32 fYaw=p->getYaw(),fPitch=p->getPitch(),fRoll=p->getRoll();
        for(i=0; i<2; i++) pRoll [i]->setRotation(vector3df(i==0?15.0f*fRoll:-15.0f*fRoll,i==0?4:-4,0));
        for(i=0; i<2; i++) pPitch[i]->setRotation(vector3df(fPitch*-15.0f,i==0?-8:8,0));

        pYaw[0]->setRotation(vector3df(-15.0f*fYaw,13,90));
        pYaw[1]->setRotation(vector3df(0, 10.0f*fYaw, 0));
      }
      return false;
    }

    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
      return pEvent->getType()==irr::ode::eIrrOdeEventBodyRemoved ||
             pEvent->getType()==EVENT_RUDDER_ID;
    }
};

class APS_EventFactory : public irr::ode::IIrrOdeEventFactory {
  public:
    virtual irr::ode::IIrrOdeEvent *createMessage(irr::ode::CSerializer *pData, irr::scene::ISceneManager *pSmgr) {
      pData->resetBufferPos();
      irr::u16 iCode=pData->getU16();

      if (iCode==EVENT_RUDDER_ID) {
        printf("rudder event!");
        return new CEventRudderPositions(pData);
      }

      return NULL;
    }
};


static CAdvancedParticleSystemNodeFactory *g_pFactory=NULL;
static APS_EventListener *g_pListener=NULL;
static APS_EventFactory *g_pEventFactory=NULL;

int DLL_EXPORT install(irr::IrrlichtDevice *pDevice, void *pUserData) {
  SSharedManagers *pManagers=(SSharedManagers *)pUserData;

  if (g_pFactory==NULL) {
    printf("registering scenenode factory...\n");
    g_pFactory=new CAdvancedParticleSystemNodeFactory(pDevice->getSceneManager());
    pDevice->getSceneManager()->registerSceneNodeFactory(g_pFactory);
  }

  printf("registering event listener...\n");
  g_pListener=new APS_EventListener(pDevice);
  ((irr::ode::CIrrOdeManager *)pManagers->m_pOdeManager)->getQueue()->addEventListener(g_pListener);

  printf("registering event factory...\n");
  g_pEventFactory=new APS_EventFactory();
  ((irr::ode::CIrrOdeEventFactory *)pManagers->m_pEventFactory)->registerEventFactory(g_pEventFactory);

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

  return 0;
}

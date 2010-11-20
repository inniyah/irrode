  #include <IrrOdeRePLayerPlugin.h>

  #include <CAdvancedParticleSystemNode.h>

  #include <IrrOde.h>

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
      return false;
    }

    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
      return pEvent->getType()==irr::ode::eIrrOdeEventBodyRemoved;
    }
};

static CAdvancedParticleSystemNodeFactory *g_pFactory=NULL;
static APS_EventListener *g_pListener=NULL;

int DLL_EXPORT install(irr::IrrlichtDevice *pDevice, void *pUserData) {
  if (g_pFactory==NULL) {
    g_pFactory=new CAdvancedParticleSystemNodeFactory(pDevice->getSceneManager());
    pDevice->getSceneManager()->registerSceneNodeFactory(g_pFactory);
  }

  g_pListener=new APS_EventListener(pDevice);
  irr::ode::CIrrOdeManager *pMgr=(irr::ode::CIrrOdeManager *)pUserData;
  pMgr->getQueue()->addEventListener(g_pListener);
  return 0;
}

int DLL_EXPORT destall(irr::IrrlichtDevice *pDevice, void *pUserData) {
  irr::ode::CIrrOdeManager *pMgr=(irr::ode::CIrrOdeManager *)pUserData;
  pMgr->getQueue()->removeEventListener(g_pListener);
  delete g_pListener;
  return 0;
}

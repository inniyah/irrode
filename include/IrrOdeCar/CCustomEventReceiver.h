#ifndef _IRRODE_CAR_CUSTOM_EVENT_RECEIVER
  #define _IRRODE_CAR_CUSTOM_EVENT_RECEIVER

  #include <irrlicht.h>
  #include <IrrOde.h>

class CCustomEventReceiver : public irr::ode::IIrrOdeEventListener {
  protected:
    typedef struct SPlaneNodes {
      irr::s32 iNodeId;
      irr::core::array<irr::scene::ISceneNode *> aYaw, aPitch, aRoll;
    } SPlaneNodes;

    irr::IrrlichtDevice *m_pDevice;
    irr::ode::CIrrOdeManager *m_pOdeManager;

    bool m_bInstalled;

    CCustomEventReceiver();
    ~CCustomEventReceiver();

    list<SPlaneNodes *> m_lPlanes;

    void searchPlaneNodes(irr::scene::ISceneNode *pNode, SPlaneNodes *pPlane);

    /**
     * This method deactivates all particles systems that are children of a node
     * @param pNode the node
     */
    void deactivateParticleSystems(irr::scene::ISceneNode *pNode);

    /**
     * This method hides the projectile which is an animated mesh
     * @param pNode the node to check
     */
    void hideAnimatedMesh(irr::scene::ISceneNode *pNode);
  public:
    static void setMembers(irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMgr);

    static CCustomEventReceiver *getSharedInstance();

    void install();
    void destall();

    void addPlane(irr::scene::ISceneNode *pPlane);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

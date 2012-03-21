#ifndef _IRR_ODE_MANAGER
  #define _IRR_ODE_MANAGER

  #include <irrlicht.h>
  #include <event/IIrrOdeEventListener.h>
  #include <observer/CIrrOdeEventFactory.h>
  #include <CIrrOdeSurfaceParameters.h>

namespace irr {
namespace ode {

class CIrrOdeDampable;
class CIrrOdeSceneNode;
class IIrrOdeDevice;
class IIrrOdeEvent;
class CIrrOdeWorld;
class IIrrOdeStepMotor;
class IIrrOdeEventQueue;
class IIrrOdeEventWriter;

/**
 * @class CIrrOdeManager
 * This class is the central manager of IrrODE.
 */
class CIrrOdeManager : public IIrrOdeEventListener {
  protected:
    u32 m_iLastStep,            /**< the time in ms left over from the last step */
        m_iFrameNo,             /**< counter of rendered frames */
        m_iNodesInitialized;    /**< the number of nodes that have already been initialized */
    s32 m_iNextOdeNodeId;       /**< the next id for ODE scene nodes */

    bool m_bPhysicsInitialized; /**< was the initPhysics call already done? */

    irr::core::list<irr::ode::CIrrOdeSceneNode *> m_pSceneNodes;     /**< the scene nodes */
    irr::core::list<irr::ode::IIrrOdeStepMotor *> m_lStepMotors;     /**< all step motors */
    irr::core::list<irr::ode::CIrrOdeWorld *> m_lWorlds;             /**< the worlds */

    /**
     * This is a irr::core::list of all objects that want to post events
     * about their state change after the step has been done.
     */
    irr::core::list<irr::ode::IIrrOdeEventWriter *> m_lChanged;

    IIrrOdeEventQueue *m_pQueue;

    ITimer *m_pTimer;                                           /**< the Irrlicht timer */

    irr::scene::ISceneManager *m_pSmgr;
    IrrlichtDevice *m_pDevice;

		irr::core::list<irr::ode::CIrrOdeSurfaceParameters *> m_lParamList;      /**< global irr::core::list of surface parameters */
		CIrrOdeSurfaceParameters m_cNullSurface;

	  IIrrOdeDevice *m_pOdeDevice;

    CIrrOdeManager();  /**< protected contructor for singleton */
    ~CIrrOdeManager();

    bool isRegisteredOdeSceneNode(irr::scene::ISceneNode *pNode);
    void updateSurfaceParameterList();

  public:
    static CIrrOdeManager *getSharedInstance();   /*!< get a pointer to the singleton instance of this class */

    /**
     * This method must be called before any other IrrOde methods are called.
     * @param pDevice the Irrlicht device
     */
    void install(IrrlichtDevice *pDevice);

    void addOdeSceneNode(CIrrOdeSceneNode *pNode);    /*!< add an ode scenenode to the manager */
    void removeOdeSceneNode(CIrrOdeSceneNode *pNode); /*!< remove an ode scenenode from the manager */

    void initODE();   /*!< init ODE */
    void closeODE();  /*!< close ODE */
    void clearODE();  /*!< remove all objects from the simulation */

    void step();      /*!< step the world(s) */

    void addWorld(CIrrOdeWorld *pWorld);     /*!< add a world */
    void removeWorld(CIrrOdeWorld *pWorld);  /*!< remove a world */

    IIrrOdeDevice *getOdeDevice();															/*!< get a pointer to the ODE device */
    void setOdeDevice(IIrrOdeDevice *pDevice);                  /*!< set a new ODE device */
    void initPhysics();																					/*!< init all dynamic objects */
    irr::core::list<CIrrOdeSceneNode *> &getIrrOdeNodes();						/*!< get a irr::core::list of all registered IrrOde scene nodes */
    irr::core::list<CIrrOdeWorld *> &getWorlds();                    /*!< get a irr::core::list of all registered IrrOde worlds */

		void addSurfaceParameter(CIrrOdeSurfaceParameters *pParam);
		void removeSurfaceParameter(CIrrOdeSurfaceParameters *pParam);
		CIrrOdeSurfaceParameters *getSurfaceParameter(irr::core::stringw sName);

    const c8 *const *getSurfaceParameterList();

		u32 getFrameNo() { return m_iFrameNo; }

		void addStepMotor(IIrrOdeStepMotor *pMotor);    /*!< add a step motor */
		void removeStepMotor(IIrrOdeStepMotor *pMotor); /*!< remove a step motor */
		void stepStepMotors();                          /*!< this method is called from the IrrOdeDevice to step the step motors */

		void sceneNodeInitialized(CIrrOdeSceneNode *pNode);

		CIrrOdeDampable *getDampableWithParamName(const wchar_t *sName);

		bool physicsInitialized() { return m_bPhysicsInitialized; }

		/**
		 * This is the actual clone method
     * @param pSource the node to be cloned
     * @param newParent the parent the clone will be attached to
     * @param newSmgr the new scene manager
     * @param iNewId the new ID of the node ("-1" (default) will auto generate new id)
		 */
    irr::scene::ISceneNode *cloneOdeNode(irr::scene::ISceneNode *pSource, irr::scene::ISceneNode *newParent, irr::scene::ISceneManager *newSmgr, s32 iNewId=-1);

		/**
		 * This methdo is used to clone a complete (sub)tree with ODE scene nodes in (can as well clone any other type of node. Actually this method
     * doesn't do any cloning, it posts an IrrOde event and call CIrrOdeManager::cloneOdeNode
     * @param pSource the node to be cloned
     * @param newParent the parent the clone will be attached to
     * @param newSmgr the new scene manager
     * @see cloneOdeNode
     */
    irr::scene::ISceneNode *cloneTree(irr::scene::ISceneNode *pSource, irr::scene::ISceneNode *newParent, irr::scene::ISceneManager *newSmgr);

    s32 getNextId() { return ++m_iNextOdeNodeId; }
    void updateNextId(s32 iId) { if (iId>m_iNextOdeNodeId) m_iNextOdeNodeId=iId; }

    /**
     * This method removes all IrrOde objects of a (sub)tree from physics. Note that pNode will not be
     * removed, this method is called from CIrrOdeBody::removeFromPhysics
     * @param pNode the root node of the tree to be removed
     * @see CIrrOdeBody::removeFromPhysics
     */
    void removeTreeFromPhysics(irr::scene::ISceneNode *pNode);

    void removeEventWriter(IIrrOdeEventWriter *p);

    /**
     * Remove a node from the scene. This method sends a CIrrOdeEventNodeRemoved event
     * @param pNode node to remove
     * @see CIrrOdeEventNodeRemoved
     */
    void removeSceneNode(irr::scene::ISceneNode *pNode);

    /**
     * This method encapsulates the "loadScene" method of the Irrlicht scene manager. It loads the
     * scene and posts an event of type "CIrrOdeEventSceneLoaded" to the IrrOde event queue
     * @param sScene the scene to load
     * @param pSmgr the scene manager
     * @return "true" if the scene was loaded, "false" otherwise
     * @see irr::ode::CIrrOdeEventLoadScene
     */
     bool loadScene(const c8 *sScene, irr::scene::ISceneManager *pSmgr);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    IIrrOdeEventQueue *getQueue() { return m_pQueue; }

    ITimer *getTimer() { return m_pTimer; }

    CIrrOdeEventFactory *getEventFactory() { return CIrrOdeEventFactory::getSharedEventFactory(); }

    void objectChanged(IIrrOdeEventWriter *p);
};

} //namespace ode
} //namespace irr

#endif

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
class IIrrOdeEventQueue;

/**
 * @class CIrrOdeManager
 * This class is the central manager of IrrODE.
 */
class CIrrOdeManager : public IIrrOdeEventListener {
  protected:
    u32 m_iLastStep,            /**< the time in ms left over from the last step */
        m_iFrameNo;             /**< counter of rendered frames */

    bool m_bPhysicsInitialized; /**< was the initPhysics call already done? */

    irr::core::list<irr::ode::CIrrOdeWorld *> m_lWorlds;             /**< the worlds */

    IIrrOdeEventQueue *m_pQueue;

    ITimer *m_pTimer;                                           /**< the Irrlicht timer */

    irr::scene::ISceneManager *m_pSmgr;
    IrrlichtDevice *m_pDevice;

	  IIrrOdeDevice *m_pOdeDevice;

    CIrrOdeManager();  /**< protected contructor for singleton */
    ~CIrrOdeManager();

    void findWorlds(irr::scene::ISceneNode *pNode);

  public:
    static CIrrOdeManager *getSharedInstance();   /*!< get a pointer to the singleton instance of this class */

    /**
     * This method must be called before any other IrrOde methods are called.
     * @param pDevice the Irrlicht device
     */
    void install(IrrlichtDevice *pDevice);

    void initODE();   /*!< init ODE */
    void closeODE();  /*!< close ODE */

    void step();      /*!< step the world(s) */

    IIrrOdeDevice *getOdeDevice();															/*!< get a pointer to the ODE device */
    void setOdeDevice(IIrrOdeDevice *pDevice);                  /*!< set a new ODE device */
    void initPhysics();																					/*!< init all dynamic objects */

		u32 getFrameNo() { return m_iFrameNo; }
    CIrrOdeDampable *getDampableWithParamName(const wchar_t *sName);

		bool physicsInitialized() { return m_bPhysicsInitialized; }

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
};

} //namespace ode
} //namespace irr

#endif

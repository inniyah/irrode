#ifndef _IRRODE_CAR_CUSTOM_EVENT_RECEIVER
  #define _IRRODE_CAR_CUSTOM_EVENT_RECEIVER

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <CCockpitCar.h>

namespace irrklang {
  class ISoundEngine;
  class ISound;
};

class CAdvancedParticleSystemNode;

class CCustomEventReceiver : public irr::ode::IIrrOdeEventListener {
  protected:
    typedef struct SPlaneNodes {
      irr::s32 iNodeId;
      irr::core::array<irr::scene::ISceneNode *> aYaw, aPitch, aRoll;
      irrklang::ISound *pEngine,
                       *pWind;
      irr::ode::CIrrOdeBody *pPlane;
    } SPlaneNodes;

    typedef struct sTankNodes {
      irr::ode::CIrrOdeBody *pTank;
      irrklang::ISound *pEngine;
      irr::s32 iNodeId;
      irr::core::array<irr::scene::ISceneNode *> aAxes;
      irr::scene::ISceneNode *pCannon,
                             *pTurret;
    } STankNodes;

    typedef struct sCarNodes {
      irr::s32 iNodeId;
      irr::f32 fSteerAngle;
      irr::core::vector3df vOldSpeed;
      irr::scene::ISceneNode *pSuspension,
                             *pRearWheels[2],
                             *pFrontAxes[2],
                             *pSteering,
                             *pBody;
      irr::ode::CIrrOdeBody *pFrontWheels[2],*pCar;
      CAdvancedParticleSystemNode *pSmoke[2];
      irrklang::ISound *pEngine,
                       *pWind,
                       *pWheels;
    } SCarNodes;

    typedef struct sHeliNodes {
      irr::s32 iNodeId;
      irr::ode::CIrrOdeBody *pHeli;
      irrklang::ISound *pEngine;
    } SHeliNodes;

    typedef struct sMissileNodes {
      irr::s32 iNodeId;
      irr::ode::CIrrOdeBody *pNode;
      irrklang::ISound *pEngine;
    } SMissileNodes;

    irr::IrrlichtDevice *m_pDevice;
    irr::ode::CIrrOdeManager *m_pOdeManager;
    irr::video::ITexture *m_pRearLights[2];

    bool m_bInstalled;

    irrklang::ISoundEngine *m_pSndEngine;

    CCockpitCar *m_pCockpitCar;

    CCustomEventReceiver();
    ~CCustomEventReceiver();

    irr::core::list<SPlaneNodes   *> m_lPlanes;
    irr::core::list<STankNodes    *> m_lTanks;
    irr::core::list<SCarNodes     *> m_lCars;
    irr::core::list<SHeliNodes    *> m_lHelis;
    irr::core::list<SMissileNodes *> m_lMissiles;

    void searchPlaneNodes(irr::scene::ISceneNode *pNode, SPlaneNodes *pPlane);
    void searchTankNodes(irr::scene::ISceneNode *pNode, STankNodes *pTank);
    void searchCarNodes(irr::scene::ISceneNode *pNode, SCarNodes *pCar);

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

    void updateSound(irrklang::ISound *pSound, irr::ode::CIrrOdeBody *pBody);
  public:
    static void setMembers(irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMgr, irrklang::ISoundEngine *pSndEngine);

    static CCustomEventReceiver *getSharedInstance();

    void install();
    void destall();

    void addPlane(irr::scene::ISceneNode *pPlane);
    void addTank (irr::scene::ISceneNode *pTank );
    void addCar  (irr::scene::ISceneNode *pCar  );
    void addHeli (irr::scene::ISceneNode *pHeli );

    void setCockpitCar(CCockpitCar *p) { m_pCockpitCar = p; }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

#ifndef _IRRODE_CAR_CUSTOM_EVENT_RECEIVER
  #define _IRRODE_CAR_CUSTOM_EVENT_RECEIVER

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <CCockpitCar.h>

#ifndef NO_IRRKLANG
namespace irrklang {
  class ISoundEngine;
  class ISound;
};
#else
namespace irrklang {
  typedef void ISoundEngine;
}
#endif

class CAdvancedParticleSystemNode;
class CEventPlaneState;
class CEventTankState;
class CEventHeliState;
class CEventCarState;
class CCockpitPlane;
class CCockpitCar;

class CCustomEventReceiver : public irr::ode::IIrrOdeEventListener {
  protected:
    class CPlaneNodes {
      private:
        irr::s32 m_iNodeId;
        irr::core::array<irr::scene::ISceneNode *> m_aYaw, m_aPitch, m_aRoll;
        irr::ode::CIrrOdeBody *m_pPlane;
#ifndef NO_IRRKLANG
        irrklang::ISound *m_pEngine,
                         *m_pWind;
#endif
        irrklang::ISoundEngine *m_pSndEngine;

        CCockpitPlane *m_pCockpit;

        void searchPlaneNodes(irr::scene::ISceneNode *pNode);

      public:
        CPlaneNodes(irr::scene::ISceneNode *pPlane, irrklang::ISoundEngine *pSndEngine, irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMgr);
        ~CPlaneNodes();

        void handlePlaneEvent(CEventPlaneState *p);
        irr::s32 getNodeId() { return m_iNodeId; }
        void triggerUpdateSound();
        irr::ode::CIrrOdeBody *getBody() { return m_pPlane; }
    };

    class CTankNodes {
      private:
        irr::s32 m_iNodeId;
        irr::ode::CIrrOdeBody *m_pTank;
        irr::core::array<irr::scene::ISceneNode *> m_aAxes;
        irr::scene::ISceneNode *m_pCannon,
                               *m_pTurret;
#ifndef NO_IRRKLANG
        irrklang::ISound *m_pEngine;
#endif
        irrklang::ISoundEngine *m_pSndEngine;

        void searchTankNodes(irr::scene::ISceneNode *pNode);
      public:
        CTankNodes(irr::scene::ISceneNode *pTank, irrklang::ISoundEngine *pSndEngine);
        ~CTankNodes();

        void handleTankEvent(CEventTankState *p);
        irr::s32 getNodeId() { return m_iNodeId; }
        void triggerUpdateSound();
    };

    class CCarNodes {
      private:
        irr::s32 m_iNodeId;
        irr::f32 m_fSteerAngle;
        irr::core::vector3df m_vOldSpeed;
        irr::scene::ISceneNode *m_pSuspension,
                               *m_pRearWheels[2],
                               *m_pFrontAxes[2],
                               *m_pFrontWheels[2],
                               *m_pSteering,
                               *m_pBody,
                               *m_pSteer[2];
        irr::ode::CIrrOdeBody *m_pCar;
        irr::ode::CIrrOdeManager *m_pOdeMgr;
        CAdvancedParticleSystemNode *m_pSmoke[2],
                                    *m_pFire[2];
        irrklang::ISoundEngine *m_pSndEngine;
#ifndef NO_IRRKLANG
        irrklang::ISound *m_pEngine,
                         *m_pWind,
                         *m_pWheels;
#endif

        CCockpitCar *m_pCockpit;

        irr::video::ITexture *m_pRearLights[2];
        void searchCarNodes(irr::scene::ISceneNode *pNode);

      public:
        CCarNodes(irr::scene::ISceneNode *pCar, irrklang::ISoundEngine *pSndEngine, irr::video::ITexture *pRearLights[2], irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMgr);
        ~CCarNodes();

        void handleCarEvent(CEventCarState *p);
        irr::s32 getNodeId() { return m_iNodeId; }
        void triggerUpdateSound();
    };

    class CHeliNodes {
      private:
        irr::s32 m_iNodeId;
        irr::ode::CIrrOdeBody *m_pHeli;
#ifndef NO_IRRKLANG
        irrklang::ISound *m_pEngine;
#endif
        irrklang::ISoundEngine *m_pSndEngine;

        CCockpitPlane *m_pCockpit;

      public:
        CHeliNodes(irr::scene::ISceneNode *pHeli, irrklang::ISoundEngine *pSndEngine, irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMgr);
        ~CHeliNodes();

        void handleHeliEvent(CEventHeliState *p);
        irr::s32 getNodeId() { return m_iNodeId; }
        void triggerUpdateSound();
        irr::ode::CIrrOdeBody *getBody() { return m_pHeli; }
    };

    class CMissileNodes {
      private:
        irr::s32 m_iNodeId;
        irr::ode::CIrrOdeBody *m_pNode;
#ifndef NO_IRRKLANG
        irrklang::ISound *m_pEngine;
#endif
        irrklang::ISoundEngine *m_pSndEngine;

      public:
        CMissileNodes(irr::scene::ISceneNode *pMissile, irrklang::ISoundEngine *pSndEngine, irr::ode::CIrrOdeEventNodeCloned *p);
        ~CMissileNodes();

        void handleMissileEvent(irr::ode::IIrrOdeEvent *p);
        irr::s32 getNodeId() { return m_iNodeId; }
        void triggerUpdateSound();
    };

    irr::IrrlichtDevice *m_pDevice;
    irr::ode::CIrrOdeManager *m_pOdeManager;
    irr::video::ITexture *m_pRearLights[2];

    bool m_bInstalled;

    irrklang::ISoundEngine *m_pSndEngine;

    CCustomEventReceiver();
    ~CCustomEventReceiver();

    irr::core::list<CPlaneNodes   *> m_lPlanes;
    irr::core::list<CTankNodes    *> m_lTanks;
    irr::core::list<CCarNodes     *> m_lCars;
    irr::core::list<CHeliNodes    *> m_lHelis;
    irr::core::list<CMissileNodes *> m_lMissiles;

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

#ifndef NO_IRRKLANG
    static void updateSound(irrklang::ISound *pSound, irr::ode::CIrrOdeBody *pBody);
#endif
  public:
    static void setMembers(irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMgr, irrklang::ISoundEngine *pSndEngine);

    static CCustomEventReceiver *getSharedInstance();

    void install();
    void destall();

    void addPlane(irr::scene::ISceneNode *pPlane);
    void addTank (irr::scene::ISceneNode *pTank );
    void addCar  (irr::scene::ISceneNode *pCar  );
    void addHeli (irr::scene::ISceneNode *pHeli );

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

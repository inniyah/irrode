#ifndef _C_PROJECTILE
  #define _C_PROJECTILE

  #include <irrlicht.h>
  #include <irrOde.h>

class CAdvancedParticleSystemNode;
class CIrrOdeCarState;
class CAutoPilot;

class CProjectile {
  protected:
    irr::ode::CIrrOdeBody *m_pBody,
                          *m_pTarget;
    irr::s32 m_iTtl;
    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::scene::ISceneManager *m_pSmgr;
    irr::scene::IAnimatedMeshSceneNode *m_pNode;

    irr::ode::CIrrOdeAeroDrag *m_pAero;
    irr::ode::CIrrOdeTorqueMotor *m_pTorque;

    CAutoPilot *m_pAutoPilot;

    array<CAdvancedParticleSystemNode *> m_aParticleSystems;

    CIrrOdeCarState *m_pShooter;

    bool m_bActive;

    void findParticleSystems(irr::scene::ISceneNode *pNode);
    void findMesh(irr::scene::ISceneNode *pNode);

  public:
    /**
     * The projectile's constructor
     * @param pSmgr the scene manager
     * @param vPos the initial position
     * @param vRot the initial rotation
     * @param vVel the initial velocity
     * @param sSource name of the scene node to copy
     * @param iTtl the time to live (in ODE steps)
     * @param vAcc acceleration vector for missiles
     * @param pWorld the ODE world
     * @param fScale the scale of the meshscenenode
     * @param bFastCollision true for fast moving projectiles
     */
    CProjectile(irr::scene::ISceneManager *pSmgr, irr::core::vector3df vPos, irr::core::vector3df vRot, irr::core::vector3df vVel, const irr::c8 *sSource, irr::s32 iTtl, irr::scene::ISceneNode *pWorld, bool bFastCollision, CIrrOdeCarState *pSource);
    ~CProjectile();

    irr::ode::CIrrOdeBody *getBody();
    irr::s32 getTtl();

    void step();
    void collide(bool bBodyHit);

    bool particlesActive();

    void setTarget(irr::ode::CIrrOdeBody *pTarget);
};

class CProjectileManager : irr::ode::IIrrOdeEventListener {
  protected:
    irr::u32 m_iShots,m_iHits;
    irr::core::list<CProjectile *> m_lShots;

    CProjectile *m_pLast;

    CProjectileManager();
    virtual ~CProjectileManager();

  public:
    static CProjectileManager *getSharedInstance();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    void addShot(CProjectile *p);

    irr::u32 getShots() { return m_iShots; }
    irr::u32 getHits () { return m_iHits ; }

    CProjectile *getLast();
};

#endif

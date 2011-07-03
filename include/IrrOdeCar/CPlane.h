#ifndef _C_PLANE
  #define _C_PLANE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"
  #include <CEventVehicleState.h>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

enum ePlaneControls {
  ePlaneYawLeft,
  ePlaneYawRight,
  ePlaneRollLeft,
  ePlaneRollRight,
  ePlanePitchUp,
  ePlanePitchDown,
  ePlanePowerUp,
  ePlanePowerDown,
  ePlanePowerZero,
  ePlaneFire,
  ePlaneGun,
  ePlaneToggleCam,
  ePlaneBackview,
  ePlaneBrake,
  ePlaneFlip,
  ePlaneAutoPilot,
  ePlaneTarget
};

class CTargetSelector;
class CProjectile;
class CAutoPilot;

class CPlane : public CIrrOdeCarState, public IEventReceiver, public irr::ode::IIrrOdeEventListener {
  protected:
    s32 m_iThrustDir,
        m_iNextCp;
    f32 m_fThrust,
        m_fYaw,
        m_fPitch,
        m_fRoll,
        m_fApDist;
    bool m_bFollowBombCam,
         m_bBackView,
         m_bLeftMissile,
         m_bThreeWheeler,
         m_bAutoPilot;

    IGUIStaticText *m_pInfo,
                   *m_pApInfo;

    ICameraSceneNode *m_pCam;
    ITerrainSceneNode *m_pTerrain;

    CAutoPilot *m_pAutoPilot;
    CTargetSelector *m_pTargetSelector;

    irr::ode::CIrrOdeWorld *m_pWorld;
    irr::ode::CIrrOdeBody *m_pPlaneBody;
    irr::ode::CIrrOdeServo *m_pSteer;
    irr::ode::CIrrOdeBody *m_pFrontWheel;
    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::ode::CIrrOdeMotor *m_pBrakes[2];
    irr::ode::CIrrOdeAeroDrag *m_pAero;
    irr::ode::CIrrOdeTorqueMotor *m_pTorque;

    irr::core::array<irr::scene::ISceneNode *> m_aCheckPoints;

    irr::scene::ISceneCollisionManager *m_pColMgr;

    ISceneNode *m_pRoll[2],*m_pPitch[2],*m_pYaw[2];
    const u32 *m_pCtrls;
    u32 m_iLastShot,m_iLastMissile;

    irr::core::list<irr::ode::CIrrOdeSceneNode *> m_lOdeNodes;
    irr::core::list<irr::ode::CIrrOdeSceneNode *>::Iterator m_itTarget;

    irr::core::dimension2du m_cScreen;
  public:
    CPlane(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl);
    virtual ~CPlane();

    virtual void activate();
    virtual void deactivate();
    virtual u32 update();

    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual stringc &getButtonText() { static stringc s=stringc("select airplane"); return s; }

    virtual void drawSpecifics();
};

#endif



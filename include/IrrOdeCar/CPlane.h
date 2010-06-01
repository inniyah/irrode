#ifndef _C_PLANE
  #define _C_PLANE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

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
  ePlaneToggleCam,
  ePlaneBackview,
  ePlaneBrake
};

class CProjectile;

class CPlane : public CIrrOdeCarState, public IEventReceiver, public irr::ode::IIrrOdeEventListener {
  protected:
    s32 m_iThrustDir;
    f32 m_fThrust;
    bool m_bFollowBombCam,
         m_bBackView,
         m_bActive;

    IGUIStaticText *m_pInfo;

    ICameraSceneNode *m_pCam;
    ITerrainSceneNode *m_pTerrain;

    irr::ode::CIrrOdeWorld *m_pWorld;
    irr::ode::CIrrOdeBody *m_pPlaneBody;
    irr::ode::CIrrOdeServo *m_pSteer;
    irr::ode::CIrrOdeBody *m_pRearWheels[2];
    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::ode::CIrrOdeAeroDrag *m_pAero;
    irr::ode::CIrrOdeTorqueMotor *m_pTorque;

    ISceneNode *m_pRoll[2],*m_pPitch[2],*m_pYaw;
    const u32 *m_pCtrls;

  public:
    CPlane(IrrlichtDevice *pDevice, CIrrCC *pCtrl);
    virtual ~CPlane();

    virtual void activate();
    virtual void deactivate();
    virtual u32 update();

    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }
};

#endif



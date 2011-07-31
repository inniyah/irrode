#ifndef _C_AERO_VEHICLE
  #define _C_AERO_VEHICLE

  #include <irrlicht.h>
  #include <CIrrOdeCarState.h>
  #include <IrrOde.h>

using namespace irr;

class CAutoPilot;
class CCockpitPlane;
class CTargetSelector;

enum eAerealControls {
  eAeroPitchUp,
  eAeroPitchDown,
  eAeroRollLeft,
  eAeroRollRight,
  eAeroYawLeft,
  eAeroYawRight,
  eAeroPowerUp,
  eAeroPowerDown,
  eAeroPowerZero,
  eAeroBrake,
  eAeroFirePrimary,
  eAeroFireSecondary,
  eAeroSelectTarget,
  eAeroAutoPilot,
  eAeroToggleCam,
  eAeroInternalView,
  eAeroCamLeft,
  eAeroCamRight,
  eAeroCamCenter,
  eAeroFlip
};

class CAeroVehicle : public CIrrOdeCarState, public IEventReceiver, public IIrrOdeEventListener {
  protected:
    f32 m_fPitch,
        m_fRoll,
        m_fYaw,
        m_fThrust,
        m_fCamAngle,
        m_fApDist;

    s32 m_iNextCp,
        m_aCtrlBuffer[0xFF];

    u32 m_iLastShot1,
        m_iLastShot2,
        m_iShotsFired;

    bool m_bInternal,
         m_bWeaponCam,
         m_bFirePrimary,
         m_bFireSecondary,
         m_bThreeWheeler,
         m_bRudderChanged;

    IGUIStaticText *m_pInfo,
                   *m_pApInfo;
    IGUITab *m_pTab;

    irr::ode::CIrrOdeWorld *m_pWorld;
    irr::ode::CIrrOdeBody *m_pBody;

    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::ode::CIrrOdeTorqueMotor  *m_pTorque;
    irr::ode::CIrrOdeAeroDrag     *m_pAero;
    irr::ode::CIrrOdeGeomRay      *m_pRay;
    irr::ode::CIrrOdeServo        *m_pSteer;
    irr::ode::CIrrOdeMotor        *m_pBrakes[2];

    ICameraSceneNode *m_pCam;
    ITerrainSceneNode *m_pTerrain;

    CAutoPilot *m_pAutoPilot;
    CTargetSelector *m_pTargetSelector;

    const u32 *m_pCtrls;

    irr::core::array<irr::scene::ISceneNode *> m_aCheckPoints;

    CCockpitPlane *m_pCockpit;

  public:
    CAeroVehicle(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit);
    virtual ~CAeroVehicle();

    virtual void activate();
    virtual void deactivate();
    //virtual u32 update();

    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual void odeStep(u32 iStep)=0;
};

#endif

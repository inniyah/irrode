#ifndef _C_AERO_VEHICLE
  #define _C_AERO_VEHICLE

  #include <irrlicht.h>
  #include <CIrrOdeCarState.h>
  #include <IrrOde.h>

class CRearView;
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
  eAeroCamUp,
  eAeroCamDown,
  eAeroCamCenter,
  eAeroFlip
};

class CAeroVehicle : public CIrrOdeCarState, public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  protected:
    irr::f32 m_fPitch,
             m_fRoll,
             m_fYaw,
             m_fThrust,
             m_fCamAngleH,
             m_fCamAngleV;

    irr::s32 m_iNextCp,
             m_aCtrlBuffer[0xFF];

    irr::u32 m_iLastShot1,
             m_iLastShot2,
             m_iShotsFired;

    bool m_bInternal,
         m_bWeaponCam,
         m_bFirePrimary,
         m_bFireSecondary,
         m_bThreeWheeler,
         m_bDataChanged,
         m_bBrakes;

    CRearView *m_pRView;

    irr::ode::CIrrOdeWorld *m_pWorld;
    irr::ode::CIrrOdeBody *m_pBody;

    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::ode::CIrrOdeTorqueMotor  *m_pTorque;
    irr::ode::CIrrOdeAeroDrag     *m_pAero;
    irr::ode::CIrrOdeGeomRay      *m_pRay;
    irr::ode::CIrrOdeServo        *m_pSteer;
    irr::ode::CIrrOdeMotor        *m_pBrakes[2];

    irr::scene::ICameraSceneNode *m_pCam;
    irr::scene::ITerrainSceneNode *m_pTerrain;

    CAutoPilot *m_pAutoPilot;
    CTargetSelector *m_pTargetSelector;

    const irr::u32 *m_pCtrls;

    irr::core::array<irr::scene::ISceneNode *> m_aCheckPoints;

    CCockpitPlane *m_pCockpit;
    //irrklang::ISound *m_pSound;

  public:
    CAeroVehicle(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CIrrCC *pCtrl, CRearView *pRView);
    virtual ~CAeroVehicle();

    virtual void activate();
    virtual void deactivate();
    //virtual u32 update();

    virtual bool OnEvent(const irr::SEvent &event);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    void setCtrl(const irr::u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual void odeStep(irr::u32 iStep)=0;

    irr::ode::CIrrOdeBody *getBody() { return m_pBody; }

    void setCockpit(CCockpitPlane *p) { m_pCockpit = p; }
};

#endif

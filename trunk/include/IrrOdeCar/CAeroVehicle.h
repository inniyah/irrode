#ifndef _C_AERO_VEHICLE
  #define _C_AERO_VEHICLE

  #include <irrlicht.h>
  #include <CIrrOdeCarState.h>
  #include <IrrOde.h>

class CRearView;
class CAutoPilot;
class CCockpitPlane;
class CTargetSelector;

class CAeroVehicle : public CIrrOdeCarState, public irr::ode::IIrrOdeEventWriter {
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

    bool m_bFirePrimary,
         m_bFireSecondary,
         m_bDataChanged,
         m_bBrakes,
         m_bFlip;

    CRearView *m_pRView;

    irr::ode::CIrrOdeBody *m_pBody;

    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::ode::CIrrOdeTorqueMotor  *m_pTorque;
    irr::ode::CIrrOdeAeroDrag     *m_pAero;
    irr::ode::CIrrOdeGeomRay      *m_pRay;
    irr::ode::CIrrOdeServo        *m_pSteer;
    irr::ode::CIrrOdeMotor        *m_pBrakes[2];

    irr::scene::ITerrainSceneNode *m_pTerrain;

    CAutoPilot *m_pAutoPilot;
    CTargetSelector *m_pTargetSelector;

    irr::core::array<irr::scene::ISceneNode *> m_aCheckPoints;

    CCockpitPlane *m_pCockpit;

    void postShotEvent();

  public:
    CAeroVehicle(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CRearView *pRView, irr::ode::IIrrOdeEventQueue *pInputQueue);
    virtual ~CAeroVehicle();

    virtual void activate();
    virtual void deactivate();
    //virtual u32 update();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    virtual void odeStep(irr::u32 iStep)=0;

    irr::ode::CIrrOdeBody *getBody() { return m_pBody; }

    void setCockpit(CCockpitPlane *p) { m_pCockpit = p; }

    virtual void incHitsScored();
    virtual void incHitsTaken();
    void incShotsFired();
};

#endif

#ifndef _C_AUTO_PILOT_PLANE
  #define _C_AUTO_PILOT_PLANE

  #include <irrlicht.h>
  #include <event/IIrrOdeEventListener.h>

namespace irr {
  namespace ode {
    class CIrrOdeBody;
    class CIrrOdeGeomRay;
    class CIrrOdeAeroDrag;
    class CIrrOdeTorqueMotor;
    class CIrrOdeImpulseMotor;
  }
}

class CAutoPilot {
  public:
    enum eAutoPilotState {
      eApPlaneLowAlt,
      eApPlaneCruise,
      eApHeliLowAlt,
      eApHeliCruise,
      eApMissile
    };

  protected:
    irr::scene::ISceneNode        *m_pTarget;
    irr::ode::CIrrOdeBody         *m_pPlane,
                                  *m_pTargetBody;
    irr::ode::CIrrOdeAeroDrag     *m_pAero;
    irr::ode::CIrrOdeTorqueMotor  *m_pTorque;
    irr::ode::CIrrOdeImpulseMotor *m_pMotor;

    irr::core::vector2df m_vCpVel;

    eAutoPilotState m_iState;
    irr::u32  m_iHeliCheckCount;
    irr::f32  m_fApDist,
             *m_fHeliDistLeft,
             *m_fHeliDistRight,
              m_fHeliCheckMax,
              m_fHeliCheckLength;

    irr::core::vector3df m_vSideward,
                         m_vForeward,
                         m_vUpward,
                         m_vSideward2,
                         m_vVelocityLin,
                         m_vVelocityAng,
                         m_vVATransformed,
                         m_vInvRot,
                         m_vRotation,
                         m_vPosition,
                         m_vCheckPos;

    bool m_bEnabled,
         m_bLinkYawRoll;

    irr::f32 getRollControl(irr::f32 fYaw, wchar_t *sInfo=NULL);
    irr::f32 getYawControl(bool b, wchar_t *sInfo=NULL);
    irr::f32 getHeliPitch(wchar_t *sInfo, irr::f32 fPitchTarget);

    void updateApDist();

    irr::ode::CIrrOdeGeomRay *m_pRay;

    irr::gui::IGUIStaticText *m_pAutoPilotInfo;

  public:
    CAutoPilot(irr::ode::CIrrOdeBody         *pPlane,
               irr::ode::CIrrOdeAeroDrag     *pAero,
               irr::ode::CIrrOdeTorqueMotor  *pTorque,
               irr::ode::CIrrOdeImpulseMotor *pMotor);

    CAutoPilot(irr::ode::CIrrOdeBody         *pPlane,
               irr::ode::CIrrOdeAeroDrag     *pAero,
               irr::ode::CIrrOdeTorqueMotor  *pTorque,
               irr::ode::CIrrOdeImpulseMotor *pMotor,
               irr::ode::CIrrOdeGeomRay      *pRay);

    ~CAutoPilot();

    void setTarget(irr::scene::ISceneNode *pTarget);

    void setState(eAutoPilotState iState);

    eAutoPilotState getState() { return m_iState; }

    irr::f32 getApDist() { return m_fApDist; }

    void step(irr::f32 &fYaw, irr::f32 &fPitch, irr::f32 &fRoll, irr::f32 &fThrust);

    void setEnabled(bool b);
    bool isEnabled() { return m_bEnabled; }

    void setLinkYawRoll(bool b) { m_bLinkYawRoll=b; }

    void setAutoPilotInfo(irr::gui::IGUIStaticText *pInfo);
};

#endif

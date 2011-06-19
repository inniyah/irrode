#ifndef _C_AUTO_PILOT_PLANE
  #define _C_AUTO_PILOT_PLANE

  #include <irrlicht.h>

namespace irr {
  namespace ode {
    class CIrrOdeBody;
    class CIrrOdeAeroDrag;
    class CIrrOdeTorqueMotor;
    class CIrrOdeImpulseMotor;
  }
}

class CAutoPilot {
  protected:
    irr::scene::ISceneNode        *m_pTarget;
    irr::ode::CIrrOdeBody         *m_pPlane,
                                  *m_pTargetBody;
    irr::ode::CIrrOdeAeroDrag     *m_pAero;
    irr::ode::CIrrOdeTorqueMotor  *m_pTorque;
    irr::ode::CIrrOdeImpulseMotor *m_pMotor;

    irr::core::vector2di m_vArtificialHorizon;
    irr::core::vector2df m_vCpVel;

    irr::u32 m_iState;
    irr::f32 m_fApDist;

    bool m_bEnabled,
         m_bLinkYawRoll;

  public:
    CAutoPilot(irr::ode::CIrrOdeBody         *pPlane,
                    irr::ode::CIrrOdeAeroDrag     *pAero,
                    irr::ode::CIrrOdeTorqueMotor  *pTorque,
                    irr::ode::CIrrOdeImpulseMotor *pMotor);

    void setTarget(irr::scene::ISceneNode *pTarget);

    void setState(irr::u32 iState) { m_iState=iState; }

    irr::f32 getApDist() { return m_fApDist; }

    void step(irr::f32 &fYaw, irr::f32 &fPitch, irr::f32 &fRoll, irr::f32 &fThrust);

    void drawSpecifics(irr::video::IVideoDriver *pDrv);

    void setEnabled(bool b) { m_bEnabled=b; }
    bool isEnabled() { return m_bEnabled; }

    void setLinkYawRoll(bool b) { m_bLinkYawRoll=b; }
};

#endif

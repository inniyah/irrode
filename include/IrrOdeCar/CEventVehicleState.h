#ifndef _C_EVENT_RUDDER_POSITIONS
  #define _C_EVENT_RUDDER_POSITIONS


  #include <event/IIrrOdeEvent.h>
  #include <irrlicht.h>

#define EVENT_PLANE_STATE_ID irr::ode::eIrrOdeEventUser+1
#define EVENT_TANK_STATE_ID  irr::ode::eIrrOdeEventUser+2

class CEventPlaneState : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::s8 m_iYaw,
            m_iRoll,
            m_iPitch;
    bool m_bThreeWheeler;

  public:
    CEventPlaneState();
    CEventPlaneState(irr::s32 iId, irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll, bool bThreeWheeler);
    CEventPlaneState(irr::ode::CSerializer *pData);

    virtual ~CEventPlaneState() { }

    virtual irr::u16 getType() { return EVENT_PLANE_STATE_ID; }

    virtual irr::ode::CSerializer *serialize();
    virtual const irr::c8 *toString();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::f32 getYaw() { return ((irr::f32)m_iYaw)/100.0f; }
    irr::f32 getPitch() { return ((irr::f32)m_iPitch)/100.0f; }
    irr::f32 getRoll() { return ((irr::f32)m_iRoll)/100.0f; }

    irr::s32 getNodeId() { return m_iNodeId; }

    bool isThreeWheeler() { return m_bThreeWheeler; }
};

class CEventTankState : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::s8 m_aAngles[4];
    irr::f32 m_fCannonAngle,
             m_fTurretAngle;

  public:
    CEventTankState();
    CEventTankState(irr::s32 iId, const irr::s8 *aAngles, irr::f32 fCannonAngle, irr::f32 fTurretAngle);
    CEventTankState(irr::ode::CSerializer *pData);

    virtual ~CEventTankState() { }

    virtual irr::u16 getType() { return EVENT_TANK_STATE_ID; }

    virtual irr::ode::CSerializer *serialize();
    virtual const irr::c8 *toString();
    virtual irr::ode::IIrrOdeEvent *clone();

    const irr::s8 *getAngles() { return m_aAngles; }
    irr::s32 getNodeId() { return m_iNodeId; }
    irr::f32 getCannonAngle() { return m_fCannonAngle; }
    irr::f32 getTurretAngle() { return m_fTurretAngle; }
};

#endif

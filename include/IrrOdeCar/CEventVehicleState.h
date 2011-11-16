#ifndef _C_EVENT_RUDDER_POSITIONS
  #define _C_EVENT_RUDDER_POSITIONS


  #include <event/IIrrOdeEvent.h>
  #include <irrlicht.h>

#define EVENT_PLANE_STATE_ID irr::ode::eIrrOdeEventUser+1
#define EVENT_TANK_STATE_ID  irr::ode::eIrrOdeEventUser+2
#define EVENT_CAR_STATE_ID  irr::ode::eIrrOdeEventUser+3
#define EVENT_FIRE_SND_ID irr::ode::eIrrOdeEventUser+4
#define EVENT_HELI_STATE_ID irr::ode::eIrrOdeEventUser+5

class CEventPlaneState : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::s8 m_iYaw,
            m_iRoll,
            m_iPitch;
    irr::f32 m_fSound;
    bool m_bThreeWheeler;

  public:
    CEventPlaneState();
    CEventPlaneState(irr::s32 iId, irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll, irr::f32 fSound, bool bThreeWheeler);
    CEventPlaneState(irr::ode::CSerializer *pData);

    virtual ~CEventPlaneState() { }

    virtual irr::u16 getType() { return EVENT_PLANE_STATE_ID; }

    virtual irr::ode::CSerializer *serialize();
    virtual const irr::c8 *toString();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::f32 getYaw() { return ((irr::f32)m_iYaw)/100.0f; }
    irr::f32 getPitch() { return ((irr::f32)m_iPitch)/100.0f; }
    irr::f32 getRoll() { return ((irr::f32)m_iRoll)/100.0f; }
    irr::f32 getSound() { return m_fSound; }
    irr::s32 getNodeId() { return m_iNodeId; }

    bool isThreeWheeler() { return m_bThreeWheeler; }
};

class CEventTankState : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::s8 m_aAngles[4];
    irr::f32 m_fCannonAngle,
             m_fTurretAngle,
             m_fSound;

  public:
    CEventTankState();
    CEventTankState(irr::s32 iId, const irr::s8 *aAngles, irr::f32 fCannonAngle, irr::f32 fTurretAngle, irr::f32 fSound);
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
    irr::f32 getSound() { return m_fSound; }
};

class CEventCarState : public irr::ode::IIrrOdeEvent {
  public:
    enum eCarFlags {
      eCarFlagBoost=1,
      eCarFlagBrake=2,
      eCarFlagReverse=4
    };

  protected:
    irr::s32 m_iNodeId;
    irr::f32 m_fSuspension,
             m_fLeftWheel,
             m_fRightWheel,
             m_fRpm,
             m_fDiff,
             m_fSound;
    irr::u8  m_iFlags;
  public:
    CEventCarState();
    CEventCarState(irr::s32 iId, irr::f32 fSuspension, irr::f32 fLeftWheel, irr::f32 fRightWheel, irr::f32 fRpm, irr::f32 fDiff, irr::f32 fSound, irr::u8 iFlags);
    CEventCarState(irr::ode::CSerializer *pData);

    virtual irr::u16 getType() { return EVENT_CAR_STATE_ID; }
    virtual const irr::c8 *toString();

    virtual irr::ode::CSerializer *serialize();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::s32 getNodeId() { return m_iNodeId; }
    irr::f32 getSuspension() { return m_fSuspension; }
    irr::f32 getLeftWheel() { return m_fLeftWheel; }
    irr::f32 getRightWheel() { return m_fRightWheel; }
    irr::f32 getRpm() { return m_fRpm; }
    irr::f32 getDiff() { return m_fDiff; }
    irr::f32 getEngineSound() { return m_fSound; }
    irr::u8 getFlags() { return m_iFlags; }
};

class CEventFireSound : public irr::ode::IIrrOdeEvent {
  public:
    enum enSound {
      eSndNone,
      eSndFireShell,
      eSndExplode,
      eSndCrash,
      eSndSkid
    };

  protected:
    irr::u8 m_iSound;
    irr::f32 m_fVolume;
    irr::core::vector3df m_vPos;

  public:
    CEventFireSound();
    CEventFireSound(enSound iSound, irr::f32 fVolume, irr::core::vector3df vPos);
    CEventFireSound(irr::ode::CSerializer *pData);

    virtual irr::u16 getType() { return EVENT_FIRE_SND_ID; }
    virtual const irr::c8 *toString();

    virtual irr::ode::CSerializer *serialize();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::u8 getSound() { return m_iSound; }
    irr::f32 getVolume() { return m_fVolume; }
    const irr::core::vector3df &getPosition() { return m_vPos; }
};

class CEventHeliState : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::f32 m_fSound;

  public:
    CEventHeliState();
    CEventHeliState(irr::s32 iNodeId, irr::f32 fSound);
    CEventHeliState(irr::ode::CSerializer *pData);

    virtual irr::u16 getType() { return EVENT_HELI_STATE_ID; }
    virtual const irr::c8 *toString();

    virtual irr::ode::CSerializer *serialize();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::f32 getSound() { return m_fSound; }
    irr::s32 getNodeId() { return m_iNodeId; }
};
#endif

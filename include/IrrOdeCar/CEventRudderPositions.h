#ifndef _C_EVENT_RUDDER_POSITIONS
  #define _C_EVENT_RUDDER_POSITIONS


  #include <event/IIrrOdeEvent.h>
  #include <irrlicht.h>

#define EVENT_RUDDER_ID irr::ode::eIrrOdeEventUser+1

class CEventRudderPositions : public irr::ode::IIrrOdeEvent {
  protected:
    irr::s32 m_iNodeId;
    irr::s8 m_iYaw,
            m_iRoll,
            m_iPitch;

  public:
    CEventRudderPositions();
    CEventRudderPositions(irr::s32 iId, irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll);
    CEventRudderPositions(irr::ode::CSerializer *pData);

    virtual ~CEventRudderPositions() { }

    virtual irr::u16 getType() { return EVENT_RUDDER_ID; }

    virtual irr::ode::CSerializer *serialize();
    virtual const irr::c8 *toString();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::f32 getYaw() { return ((irr::f32)m_iYaw)/100.0f; }
    irr::f32 getPitch() { return ((irr::f32)m_iPitch)/100.0f; }
    irr::f32 getRoll() { return ((irr::f32)m_iRoll)/100.0f; }
};

#endif

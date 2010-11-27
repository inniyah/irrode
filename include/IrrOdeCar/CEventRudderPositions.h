#ifndef _C_EVENT_RUDDER_POSITIONS
  #define _C_EVENT_RUDDER_POSITIONS


  #include <event/IIrrOdeEvent.h>
  #include <irrlicht.h>

#define EVENT_RUDDER_ID irr::ode::eIrrOdeEventUser+1

class CEventRudderPositions : public irr::ode::IIrrOdeEvent {
  protected:
    irr::f32 m_fYaw,
             m_fRoll,
             m_fPitch;

  public:
    CEventRudderPositions();
    CEventRudderPositions(irr::f32 fYaw, irr::f32 fPitch, irr::f32 fRoll);
    CEventRudderPositions(irr::ode::CSerializer *pData);

    virtual ~CEventRudderPositions() { }

    virtual irr::u16 getType() { return EVENT_RUDDER_ID; }

    virtual irr::ode::CSerializer *serialize();
    virtual const irr::c8 *toString();
    virtual irr::ode::IIrrOdeEvent *clone();

    irr::f32 getYaw() { return m_fYaw; }
    irr::f32 getPitch() { return m_fPitch; }
    irr::f32 getRoll() { return m_fRoll; }
};

#endif

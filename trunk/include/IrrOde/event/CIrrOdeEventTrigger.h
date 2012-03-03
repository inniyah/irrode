#ifndef _IRR_ODE_EVENT_TRIGGER
  #define _IRR_ODE_EVENT_TRIGGER

  #include <irrlicht.h>
  #include <event/IIrrOdeEvent.h>

namespace irr {
namespace ode {

/**
 * @class CIrrOdeEventTrigger
 * This event is posted after the ODE was closed
 */
class CIrrOdeEventTrigger : public IIrrOdeEvent {
  protected:
    s32 m_iTriggerId,
        m_iBodyId;
    irr::core::vector3df m_vPos;

  public:
    CIrrOdeEventTrigger() {
      m_iTriggerId=-1;
      m_iBodyId=-1;
      m_vPos=irr::core::vector3df(0.0f,0.0f,0.0f);
    }

    CIrrOdeEventTrigger(s32 iTrigger, s32 iBody, irr::core::vector3df vPos) {
      m_iTriggerId=iTrigger;
      m_iBodyId=iBody;
      m_vPos=vPos;
    }

    CIrrOdeEventTrigger(CSerializer *pData) : IIrrOdeEvent() {
      pData->resetBufferPos();
      if (pData->getU16()==eIrrOdeEventTrigger) {
        m_iTriggerId=pData->getS32();
        m_iBodyId=pData->getS32();
        pData->getVector3df(m_vPos);
      }
    }

    CIrrOdeEventTrigger(CIrrOdeEventTrigger *p) {
      m_iTriggerId=p->getTriggerId();
      m_iBodyId=p->getBodyId();
      m_vPos=p->getPosition();
    }

    virtual u16 getType() { return eIrrOdeEventTrigger; }

    s32 getTriggerId() { return m_iTriggerId; }

    s32 getBodyId() { return m_iBodyId; }

    const irr::core::vector3df &getPosition() { return m_vPos; }

    virtual CSerializer *serialize() {
      if (m_pSerializer==NULL) {
        m_pSerializer=new CSerializer();
        m_pSerializer->addU16(eIrrOdeEventTrigger);
        m_pSerializer->addS32(m_iTriggerId);
        m_pSerializer->addS32(m_iBodyId);
      }
      return m_pSerializer;
    }

    virtual const c8 *toString() {
      sprintf(m_sString,"IrrOdeEventTrigger: %i, %i",m_iTriggerId,m_iBodyId);
      return m_sString;
    }

    virtual IIrrOdeEvent *clone() { return new CIrrOdeEventTrigger(this); }
};

} //namespace ode
} //namespace irr

#endif




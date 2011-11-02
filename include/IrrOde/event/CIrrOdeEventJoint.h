#ifndef _C_IRR_ODE_EVENT_JOINT
  #define _C_IRR_ODE_EVENT_JOINT

  #include <irrlicht.h>
  #include <CSerializer.h>
  #include <event/IIrrOdeEvent.h>
  #include <joints/CIrrOdeJoint.h>

namespace irr {
  namespace ode {
    enum eIrrOdeEventJointType {
      eIrrOdeEventJointUnknown,
      eIrrOdeEventJointHinge,
      eIrrOdeEventJointHinge2,
      eIrrOdeEventJointSlider
    };

    class CIrrOdeEventJoint : public IIrrOdeEvent {
      protected:
        CIrrOdeEventJoint *m_pToCopy;
        CIrrOdeJoint *m_pJoint;
        u16 m_eEventJointType;
        s32 m_iJointID;

        bool m_bSerialize;

      public:
        CIrrOdeEventJoint() {
          m_pJoint=NULL;
          m_iJointID=-1;
          m_eEventJointType=eIrrOdeEventJointUnknown;
          m_bSerialize=true;
        }

        CIrrOdeEventJoint(u16 eEventJointType, CIrrOdeJoint *pJoint) : IIrrOdeEvent() {
          m_eEventJointType=eEventJointType;
          m_pJoint=pJoint;
          m_iJointID=m_pJoint->getID();
          m_pToCopy=NULL;
          m_bSerialize=pJoint->doesSerializeEvents();
        }

        CIrrOdeEventJoint(IIrrOdeEvent *pIn) {
          if (pIn->getType()==eIrrOdeEventJoint) {
            m_pToCopy=(CIrrOdeEventJoint *)pIn;
            m_pJoint=m_pToCopy->getJoint();
            m_iJointID=m_pToCopy->getJointId();
            m_eEventJointType=m_pToCopy->getJointEventType();
          }
          else m_pToCopy=NULL;
        }

        virtual u16 getType() { return eIrrOdeEventJoint; }

        virtual CSerializer *serialize() {
          if (m_pSerializer==NULL) {
            m_pSerializer=new CSerializer();
            m_pSerializer->addU16(eIrrOdeEventJoint);
            m_pSerializer->addS32(m_iJointID);
            m_pSerializer->addU16(m_eEventJointType);
          }

          return m_pSerializer;
        }

        virtual bool isObservable() { return false; }

        void setJointId(s32 iId) {
          m_iJointID=iId;
        }

        void setJointEventType(eIrrOdeEventJointType eType) {
          m_eEventJointType=eType;
        }

        eIrrOdeEventJointType getJointEventType() {
          return (eIrrOdeEventJointType)m_eEventJointType;
        }

        s32 getJointId() {
          return m_iJointID;
        }

        CIrrOdeJoint *getJoint() {
          return m_pJoint;
        }
    };

  } //namespace ode
}   //namespace irr;

#endif

  #include <irrlicht.h>
  #include <event/CIrrOdeEventJointHinge.h>

namespace irr {
  namespace ode {

    CIrrOdeEventJointHinge::CIrrOdeEventJointHinge(CIrrOdeJoint *pJoint, f32 fAngle, f32 fAngleRate) : CIrrOdeEventJoint(eIrrOdeEventJointHinge,pJoint) {
      m_fAngle=fAngle;
      m_fAngleRate=fAngleRate;
    }

    CIrrOdeEventJointHinge::CIrrOdeEventJointHinge(CSerializer *pData, ISceneManager *pSmgr) : CIrrOdeEventJoint() {
      m_fAngle=0.0f;
      m_fAngleRate=0.0f;

      pData->resetBufferPos();
      u16 iCode=pData->getU16();
      if (iCode==eIrrOdeEventJoint) {
        m_iJointID=pData->getS32();
        iCode=pData->getU16();
        if (iCode==eIrrOdeEventJointHinge) {
          m_pJoint=(CIrrOdeJoint *)pSmgr->getSceneNodeFromId(m_iJointID);
          m_fAngle=pData->getF32();
          m_fAngleRate=pData->getF32();
        }
      }
    }

    CIrrOdeEventJointHinge::CIrrOdeEventJointHinge(IIrrOdeEvent *pIn) : CIrrOdeEventJoint(pIn) {
      if (m_pToCopy!=NULL) {
        if (m_pToCopy->getJointEventType()==eIrrOdeEventJointHinge) {
          CIrrOdeEventJointHinge *pCopy=(CIrrOdeEventJointHinge *)m_pToCopy;
          m_fAngleRate=pCopy->getAngleRate();
          m_fAngle=pCopy->getAngle();
        }
      }
    }

    CSerializer *CIrrOdeEventJointHinge::serialize() {
      if (m_pSerializer==NULL) {
        CIrrOdeEventJoint::serialize();
        if (m_pSerializer!=NULL) {
          m_pSerializer->addF32(m_fAngle);
          m_pSerializer->addF32(m_fAngleRate);
        }
      }
      return m_pSerializer;
    }

    const c8 *CIrrOdeEventJointHinge::toString() {
      sprintf(m_sString,"CIrrOdeEventJointHinge: angle=%.2f, anglerate=%.2f\n",m_fAngle,m_fAngleRate);
      return m_sString;
    }

  }
}

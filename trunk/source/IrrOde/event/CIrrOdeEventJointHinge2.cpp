  #include <event/CIrrOdeEventJointHinge2.h>

namespace irr {
  namespace ode {

    CIrrOdeEventJointHinge2::CIrrOdeEventJointHinge2(CIrrOdeJoint *pJoint, f32 fAngle1, f32 fAngleRate1, f32 fAngleRate2) : CIrrOdeEventJoint(eIrrOdeEventJointHinge2,pJoint) {
      m_fAngle1=fAngle1;
      m_fAngleRate1=fAngleRate1;
      m_fAngleRate2=fAngleRate2;
    }

    CIrrOdeEventJointHinge2::CIrrOdeEventJointHinge2(CSerializer *pData, irr::scene::ISceneManager *pSmgr) : CIrrOdeEventJoint() {
      m_fAngle1=0.0f;
      m_fAngleRate1=0.0f;
      m_fAngleRate2=0.0f;

      pData->resetBufferPos();
      u16 iCode=pData->getU16();
      if (iCode==eIrrOdeEventJoint) {
        m_iJointID=pData->getS32();
        iCode=pData->getU16();
        if (iCode==eIrrOdeEventJointHinge2) {
          m_pJoint=(CIrrOdeJoint *)pSmgr->getSceneNodeFromId(m_iJointID);
          m_fAngle1=pData->getF32();
          m_fAngleRate1=pData->getF32();
          m_fAngleRate2=pData->getF32();
        }
      }
    }

    CIrrOdeEventJointHinge2::CIrrOdeEventJointHinge2(IIrrOdeEvent *pIn) : CIrrOdeEventJoint(pIn) {
      if (m_pToCopy!=NULL) {
        if (m_pToCopy->getJointEventType()==eIrrOdeEventJointHinge2) {
          CIrrOdeEventJointHinge2 *pCopy=(CIrrOdeEventJointHinge2 *)m_pToCopy;
          m_fAngle1=pCopy->getAngle1();
          m_fAngleRate1=pCopy->getAngleRate1();
          m_fAngleRate2=pCopy->getAngleRate2();
        }
      }
    }

    CSerializer *CIrrOdeEventJointHinge2::serialize() {
      if (!m_bSerialize) return NULL;

      if (m_pSerializer==NULL) {
        CIrrOdeEventJoint::serialize();
        if (m_pSerializer!=NULL) {
          m_pSerializer->addF32(m_fAngle1);
          m_pSerializer->addF32(m_fAngleRate1);
          m_pSerializer->addF32(m_fAngleRate2);
        }
      }
      return m_pSerializer;
    }

    const c8 *CIrrOdeEventJointHinge2::toString() {
      sprintf(m_sString,"CIrrOdeEventJointHinge2: angle1=%.2f, anglerate1=%.2f, anglerate2=%.2f\n",m_fAngle1,m_fAngleRate1,m_fAngleRate2);
      return m_sString;
    }
  }
}

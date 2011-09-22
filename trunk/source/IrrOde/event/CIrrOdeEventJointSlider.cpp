  #include <irrlicht.h>
  #include <event/CIrrOdeEventJointSlider.h>

namespace irr {
  namespace ode {

    CIrrOdeEventJointSlider::CIrrOdeEventJointSlider(CIrrOdeJoint *pJoint, f32 fSliderPosition, f32 fSliderPositionRate) : CIrrOdeEventJoint(eIrrOdeEventJointSlider,pJoint) {
      m_fSliderPosition=fSliderPosition;
      m_fSliderPositionRate=fSliderPositionRate;
    }

    CIrrOdeEventJointSlider::CIrrOdeEventJointSlider(CSerializer *pData, ISceneManager *pSmgr) : CIrrOdeEventJoint() {
      m_fSliderPosition=0.0f;
      m_fSliderPositionRate=0.0f;

      pData->resetBufferPos();
      u16 iCode=pData->getU16();
      if (iCode==eIrrOdeEventJoint) {
        m_iJointID=pData->getS32();
        iCode=pData->getU16();
        if (iCode==eIrrOdeEventJointSlider) {
          m_pJoint=(CIrrOdeJoint *)pSmgr->getSceneNodeFromId(m_iJointID);
          m_fSliderPosition=pData->getF32();
          m_fSliderPositionRate=pData->getF32();
        }
      }
    }

    CIrrOdeEventJointSlider::CIrrOdeEventJointSlider(IIrrOdeEvent *pIn) : CIrrOdeEventJoint(pIn) {
      if (m_pToCopy!=NULL) {
        if (m_pToCopy->getJointEventType()==eIrrOdeEventJointSlider) {
          CIrrOdeEventJointSlider *pCopy=(CIrrOdeEventJointSlider *)m_pToCopy;
          m_fSliderPosition=pCopy->getSliderPosition();
          m_fSliderPositionRate=pCopy->getSliderPositionRate();
        }
      }
    }

    CSerializer *CIrrOdeEventJointSlider::serialize() {
      if (m_pSerializer==NULL) {
        CIrrOdeEventJoint::serialize();
        if (m_pSerializer!=NULL) {
          m_pSerializer->addF32(m_fSliderPosition);
          m_pSerializer->addF32(m_fSliderPositionRate);
        }
      }
      return m_pSerializer;
    }

    const c8 *CIrrOdeEventJointSlider::toString() {
      sprintf(m_sString,"CIrrOdeEventJointSlider: slider position=%.2f, slider position rate=%.2f\n",m_fSliderPosition,m_fSliderPositionRate);
      return m_sString;
    }

  }
}


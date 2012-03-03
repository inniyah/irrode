#ifndef _C_IRR_ODE_EVENT_JOINT_SLIDER
  #define _C_IRR_ODE_EVENT_JOINT_SLIDER

  #include <event/CIrrOdeEventJoint.h>

namespace irr {
  namespace ode {

    class CIrrOdeEventJointSlider : public CIrrOdeEventJoint {
      protected:
        f32 m_fSliderPosition,
            m_fSliderPositionRate;

      public:
        CIrrOdeEventJointSlider(CIrrOdeJoint *pJoint, f32 fSliderPosition, f32 fSliderPositionRate);
        CIrrOdeEventJointSlider(CSerializer *pData, irr::scene::ISceneManager *pSmgr);
        CIrrOdeEventJointSlider(IIrrOdeEvent *pIn);

        f32 getSliderPosition() { return m_fSliderPosition; }

        f32 getSliderPositionRate() { return m_fSliderPositionRate; }

        virtual CSerializer *serialize();

        virtual const c8 *toString();

        virtual IIrrOdeEvent *clone() { return new CIrrOdeEventJointSlider(this); }
    };

  }
}

#endif

#ifndef _C_IRR_ODE_EVENT_JOINT_HINGE
  #define _C_IRR_ODE_EVENT_JOINT_HINGE

  #include <event/CIrrOdeEventJoint.h>

namespace irr {
  namespace ode {

    class CIrrOdeEventJointHinge : public CIrrOdeEventJoint {
      protected:
        f32 m_fAngle,
            m_fAngleRate;

      public:
        CIrrOdeEventJointHinge(CIrrOdeJoint *pJoint, f32 fAngle, f32 fAngleRate);
        CIrrOdeEventJointHinge(CSerializer *pData, irr::scene::ISceneManager *pSmgr);
        CIrrOdeEventJointHinge(IIrrOdeEvent *pIn);

        f32 getAngle() { return m_fAngle; }

        f32 getAngleRate() { return m_fAngleRate; }

        virtual CSerializer *serialize();

        virtual const c8 *toString();

        virtual IIrrOdeEvent *clone() { return new CIrrOdeEventJointHinge(this); }
    };
  }
}

#endif

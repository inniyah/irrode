#ifndef _C_IRR_ODE_EVENT_JOINT_HINGE2
  #define _C_IRR_ODE_EVENT_JOINT_HINGE2

  #include <event/CIrrOdeEventJoint.h>

namespace irr {
  namespace ode {

    class CIrrOdeEventJointHinge2 : public CIrrOdeEventJoint {
      protected:
        f32 m_fAngle1,
            m_fAngleRate1,
            m_fAngleRate2;

      public:
        CIrrOdeEventJointHinge2(CIrrOdeJoint *pJoint, f32 fAngle1, f32 fAngleRate1, f32 fAngleRate2);
        CIrrOdeEventJointHinge2(CSerializer *pData, irr::scene::ISceneManager *pSmgr);
        CIrrOdeEventJointHinge2(IIrrOdeEvent *pIn);

        f32 getAngle1() { return m_fAngle1; }

        f32 getAngleRate1() { return m_fAngleRate1; }
        f32 getAngleRate2() { return m_fAngleRate2; }

        virtual CSerializer *serialize();

        virtual const c8 *toString();

        virtual IIrrOdeEvent *clone() { return new CIrrOdeEventJointHinge2(this); }
    };
  }
}

#endif


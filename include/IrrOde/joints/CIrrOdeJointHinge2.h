#ifndef _IRR_ODE_JOINT_HINGE2
  #define _IRR_ODE_JOINT_HINGE2

  #include <joints/CIrrOdeJoint.h>

namespace irr {
namespace ode {

const int IRR_ODE_JOINT_HINGE2_ID=MAKE_IRR_ID('i','o','j','2');
const wchar_t IRR_ODE_JOINT_HINGE2_NAME[0xFF]=L"CIrrOdeJointHinge2";

/**
 * @class CIrrOdeJointHinge2
 * This class represents ODE's hinge2 joint.
 */
class CIrrOdeJointHinge2 : public CIrrOdeJoint {
  protected:
    irr::core::vector3df m_pAxis1,   /**< the joint's axis 1 */
              m_pAxis2,   /**< the joint's axis 2 */
              m_pAnchor1, /**< the joint anchor on body 1 */
              m_pAnchor2; /**< the joint anchor on body 2 */

    f32 m_fAngle1,
        m_fAngleRate1,
        m_fAngleRate2;

  public:
    /** standard constructor */
    CIrrOdeJointHinge2(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                      const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		                  const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		                  const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeJointHinge2();

    virtual void render();       /**< render the scene node */
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();  /**< init physics          */

    virtual s32 getID() const;                 /**< get the node's ID */
    virtual irr::scene::ESCENE_NODE_TYPE getType() const;  /**< get the node's type */
    virtual const wchar_t *getTypeName();      /**< get the node's type name */

    void setHingeAxis1(irr::core::vector3df pAxis); /**< set the node's hinge axis 1 */
    void setHingeAxis2(irr::core::vector3df pAxis); /**< set the node's hinge axis 2 */
    irr::core::vector3df getHingeAxis1();           /**< get the node's hinge axis 1 */
    irr::core::vector3df getHingeAxis2();           /**< get the node's hinge axis 2 */

    irr::core::vector3df getAnchor1();  /**< get the joint's anchor on body 1 */
    irr::core::vector3df getAnchor2();  /**< get the joint's anchor on body 2 */

    f32 getHingeAngle1();     /**< get the joint's hinge angle 1 */
    f32 getHingeAngle1Rate(); /**< get the joint's hinge angle 1 rate */
    f32 getHingeAngle2Rate(); /**< get the joint's hinge angel 2 rate */

    virtual u16 numParamGroups() const;

    virtual void setParameter(u16 iGroup, eJointParameter iParam, f32 fValue);
    virtual f32 getParameter(u16 iGroup, eJointParameter iParam);

    virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    virtual eEventWriterType getEventWriterType() {
      return eIrrOdeEventWriterJointHinge2;
    }
};

} //namespace ode
} //namespace irr

#endif


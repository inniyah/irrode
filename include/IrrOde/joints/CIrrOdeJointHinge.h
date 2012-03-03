#ifndef _IRR_ODE_JOINT_HINGE
  #define _IRR_ODE_JOINT_HINGE

  #include <joints/CIrrOdeJoint.h>

namespace irr {
namespace ode {

const int IRR_ODE_JOINT_HINGE_ID=MAKE_IRR_ID('i','o','j','h');
const wchar_t IRR_ODE_JOINT_HINGE_NAME[0xFF]=L"CIrrOdeJointHinge";

/**
 * @class CIrrOdeJointHinge
 * This class represents ODE's hinge joint.
 */
class CIrrOdeJointHinge : public CIrrOdeJoint {
  protected:
    irr::core::vector3df m_pAxis,    /**< the joint's axis */
              m_pAnchor1, /**< the joint anchor on body 1 */
              m_pAnchor2; /**< the joint anchor on body 2 */

    f32 m_fAngle,
        m_fAngleRate;

  public:
    /** standard constructor */
    CIrrOdeJointHinge(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                      const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		                  const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		                  const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeJointHinge();

    virtual void render();       /**< render the scene node */
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();  /**< init physics          */

    virtual s32 getID() const;                 /**< get the node's ID */
    virtual irr::scene::ESCENE_NODE_TYPE getType() const;  /**< get the node's type */
    virtual const wchar_t *getTypeName();      /**< get the node's type name */

    void setHingeAxis(irr::core::vector3df pAxis); /**< set the node's hinge axis */
    irr::core::vector3df getHingeAxis();           /**< get the node's hinge axis */

    irr::core::vector3df getAnchor1();  /**< get the joint's anchor on body 1 */
    irr::core::vector3df getAnchor2();  /**< get the joint's anchor on body 2 */

    f32 getHingeAngle();     /**< get the joint's hinge angle */
    f32 getHingeAngleRate(); /**< get the joint's hinge angle rate */

    virtual u16 numParamGroups() const;

    virtual void setParameter(u16 iGroup, eJointParameter iParam, f32 fValue);
    virtual f32 getParameter(u16 iGroup, eJointParameter iParam);

    virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0);
    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options);

    virtual eEventWriterType getEventWriterType() {
      return eIrrOdeEventWriterJointHinge;
    }

    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(IIrrOdeEvent *pEvent);

    /**
     * This method is called to see whether or not an event is handled by this irr::core::listener
     * @param pEvent the event in question
     * @return "true" if the irr::core::listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    bool serializeEvents() { return m_bSerializeEvents; }
};

} //namespace ode
} //namespace irr

#endif

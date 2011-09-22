#ifndef _IRR_ODE_JOINT_SLIDER
  #define _IRR_ODE_JOINT_SLIDER

  #include <joints/CIrrOdeJoint.h>

namespace irr {
namespace ode {

const int IRR_ODE_JOINT_SLIDER_ID=MAKE_IRR_ID('i','o','j','s');
const wchar_t IRR_ODE_JOINT_SLIDER_NAME[0xFF]=L"CIrrOdeJointSlider";

class CIrrOdeJointSlider : public CIrrOdeJoint {
  protected:
    vector3df m_pAxis;
    f32 m_fSliderPosition,
        m_fSliderPositionRate;

  public:
    CIrrOdeJointSlider(ISceneNode *parent,ISceneManager *mgr,s32 id = -1,
                       const vector3df &position=core::vector3df(0,0,0),
		                   const vector3df &rotation = core::vector3df(0,0,0),
		                   const vector3df &scale = core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeJointSlider();

    virtual void render();
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();

    virtual s32 getID() const;
    virtual ESCENE_NODE_TYPE getType() const;
    virtual const wchar_t *getTypeName();

    void setAxis(vector3df pAxis);
    vector3df getAxis();

    f32 getSliderPosition();
    f32 getSliderPositionRate();

    virtual u16 numParamGroups() const;

    virtual void setParameter(u16 iGroup, eJointParameter iParam, f32 fValue);
    virtual f32 getParameter(u16 iGroup, eJointParameter iParam);

    virtual ISceneNode *clone(ISceneNode* newParent=0, ISceneManager* newManager=0);
    virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(IAttributes* out, SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);

    virtual eEventWriterType getEventWriterType() {
      return eIrrOdeEventWriterJointSlider;
    }

    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(IIrrOdeEvent *pEvent);

    /**
     * This method is called to see whether or not an event is handled by this listener
     * @param pEvent the event in question
     * @return "true" if the listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);
};

} //namespace ode
} //namespace irr

#endif

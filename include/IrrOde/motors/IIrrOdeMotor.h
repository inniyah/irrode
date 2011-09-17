#ifndef _I_IRR_ODE_MOTOR
  #define _I_IRR_ODE_MOTOR

  //#include <CIrrOdeSceneNode.h>
  #include <CIrrOdeBody.h>

namespace irr {
namespace ode {

class IIrrOdeMotor : public CIrrOdeSceneNode {
  public:
    IIrrOdeMotor(ISceneNode *parent,ISceneManager *mgr,s32 id = -1,
                 const vector3df &position=core::vector3df(0,0,0),
		             const vector3df &rotation = core::vector3df(0,0,0),
		             const vector3df &scale = core::vector3df(1.0f, 1.0f, 1.0f)) :
		             CIrrOdeSceneNode(parent,mgr,id,position,rotation,scale) {
      CIrrOdeBody *pBody=(CIrrOdeBody *)getAncestorOfType((ESCENE_NODE_TYPE)IRR_ODE_BODY_ID);
      if (pBody!=NULL) pBody->addMotor(this);
    }

    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(IIrrOdeEvent *pEvent) { return false; }

    /**
     * This method is called to see whether or not an event is handled by this listener
     * @param pEvent the event in question
     * @return "true" if the listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(IIrrOdeEvent *pEvent) { return false; }
};

} }

#endif

#ifndef _I_IRR_ODE_MOTOR
  #define _I_IRR_ODE_MOTOR

  //#include <CIrrOdeSceneNode.h>
  #include <CIrrOdeBody.h>

namespace irr {
namespace ode {

class IIrrOdeMotor : public CIrrOdeSceneNode {
  public:
    IIrrOdeMotor(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id = -1,
                 const irr::core::vector3df &position=irr::core::vector3df(0,0,0),
		             const irr::core::vector3df &rotation = irr::core::vector3df(0,0,0),
		             const irr::core::vector3df &scale = irr::core::vector3df(1.0f, 1.0f, 1.0f)) :
		             CIrrOdeSceneNode(parent,mgr,id,position,rotation,scale) {
      CIrrOdeBody *pBody=(CIrrOdeBody *)getAncestorOfType((irr::scene::ESCENE_NODE_TYPE)IRR_ODE_BODY_ID);
      if (pBody!=NULL) pBody->addMotor(this);
    }

    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(IIrrOdeEvent *pEvent) { return false; }

    /**
     * This method is called to see whether or not an event is handled by this irr::core::listener
     * @param pEvent the event in question
     * @return "true" if the irr::core::listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(IIrrOdeEvent *pEvent) { return false; }
};

} }

#endif

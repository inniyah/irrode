#ifndef _IRR_ODE_EVENT_LISTENER
  #define _IRR_ODE_EVENT_LISTENER

  #include <event/IIrrOdeEvent.h>

namespace irr {
namespace ode {

/**
 * @interface IIrrOdeEventListener
 * This interface must be implemented for each IrrODE event irr::core::listener.
 */
class IIrrOdeEventListener {
  public:
    /**
     * This method is called when an event is posted
     * @param pEvent the posted event
     */
    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent)=0;

    /**
     * This method is called to see whether or not an event is handled by this irr::core::listener
     * @param pEvent the event in question
     * @return "true" if the irr::core::listener handles the event, "false" otherwise
     */
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent)=0;
};

} //namespace ode
} //namespace irr

#endif

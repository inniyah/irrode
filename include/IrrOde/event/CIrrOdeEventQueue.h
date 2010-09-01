#ifndef _C_IRR_ODE_EVENT_QUEUE
  #define _C_IRR_ODE_EVENT_QUEUE

  #include <irrlicht.h>

namespace irr {
namespace ode {

class IIrrOdeEvent;
class IIrrOdeEventListener;
class CIrrOdeManager;

class CIrrOdeEventQueue {
  protected:
    core::list<irr::ode::IIrrOdeEventListener *> m_pListeners;  /**< the event listeners */
    core::list<irr::ode::IIrrOdeEvent *> m_lEventQueue;

  public:
    CIrrOdeEventQueue(CIrrOdeManager *pOdeManager);
    virtual ~CIrrOdeEventQueue();

    virtual void postEvent(IIrrOdeEvent *pEvent);
    virtual void postEventWithoutCache(IIrrOdeEvent *pEvent);
    virtual void flushEvents();
    virtual void clearEventQueue();
};

}
}
#endif

#ifndef _C_IRR_ODE_EVENT_QUEUE
  #define _C_IRR_ODE_EVENT_QUEUE

  #include <irrlicht.h>
  #include <event/IIrrOdeEventQueue.h>

namespace irr {
namespace ode {

class IIrrOdeEvent;
class IIrrOdeEventListener;
class CIrrOdeManager;

class CIrrOdeEventQueue : public IIrrOdeEventQueue {
  protected:
    irr::core::list<irr::ode::IIrrOdeEventListener *> m_pListeners;  /**< the event irr::core::listeners */
    irr::core::list<irr::ode::IIrrOdeEvent *> m_lEventQueue;

  public:
    CIrrOdeEventQueue();
    virtual ~CIrrOdeEventQueue();

    virtual void postEvent(IIrrOdeEvent *pEvent);

    virtual void addEventListener(IIrrOdeEventListener *pListener);
    virtual void removeEventListener(IIrrOdeEventListener *pListener);
};

}
}
#endif

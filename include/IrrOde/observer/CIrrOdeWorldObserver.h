#ifndef _C_IRR_ODE_WORLD_OBSERVER
  #define _C_IRR_ODE_WORLD_OBSERVER

  #include <event/CIrrOdeEventQueue.h>
  #include <event/IIrrOdeEventListener.h>

namespace irr {
namespace ode {

class IIrrOdeEvent;

class CIrrOdeWorldObserver : public CIrrOdeEventQueue, public IIrrOdeEventListener {
  protected:
    list<IIrrOdeEvent *> m_lEvents;

    CIrrOdeWorldObserver();
    virtual ~CIrrOdeWorldObserver();

  public:
    static CIrrOdeWorldObserver *getSharedInstance();

    //implementation of CIrrOdeEventQueue methods
    virtual void postEvent(IIrrOdeEvent *pEvent, bool bDelete=true);

    virtual void addEventListener(IIrrOdeEventListener *pListener);
    virtual void removeEventListener(IIrrOdeEventListener *pListener);

    //implementation of IIrrOdeEventListener methods
    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);
};

}
}
#endif

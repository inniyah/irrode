/*
CIrrOdeManager *CIrrOdeManager::getSharedInstance() {
  static CIrrOdeManager theManager;
  return &theManager;
}
*/
  #include <observer/CIrrOdeWorldObserver.h>
  #include <event/CIrrOdeEventBodyMoved.h>

namespace irr {
namespace ode {

CIrrOdeWorldObserver::CIrrOdeWorldObserver() {
}

CIrrOdeWorldObserver::~CIrrOdeWorldObserver() {
}

CIrrOdeWorldObserver *CIrrOdeWorldObserver::getSharedInstance() {
  static CIrrOdeWorldObserver theObserver;
  return &theObserver;
}

void CIrrOdeWorldObserver::postEvent(IIrrOdeEvent *pEvent, bool bDelete) {
  if (pEvent->getType()==eIrrOdeEventBodyMoved) {
    CIrrOdeEventBodyMoved *p=reinterpret_cast<CIrrOdeEventBodyMoved *>(pEvent);
    if (p) {
      list<IIrrOdeEvent *>::Iterator it;
      for (it=m_lEvents.begin(); it!=m_lEvents.end(); it++) {
        IIrrOdeEvent *pEvt=*it;
        if (pEvt->getType()==eIrrOdeEventBodyMoved) {
        }
      }
    }
  }

  CIrrOdeEventQueue::postEvent(pEvent,bDelete);
}

void CIrrOdeWorldObserver::addEventListener(IIrrOdeEventListener *pListener) {
  list<IIrrOdeEvent *>::Iterator it;

  //when a new listener is added we need to send the current state of the simulation
  for (it=m_lEvents.begin(); it!=m_lEvents.end(); it++)
    if (pListener->handlesEvent(*it)) pListener->onEvent(*it);

  CIrrOdeEventQueue::addEventListener(pListener);
}

void CIrrOdeWorldObserver::removeEventListener(IIrrOdeEventListener *pListener) {
  CIrrOdeEventQueue::removeEventListener(pListener);
}

bool CIrrOdeWorldObserver::onEvent(IIrrOdeEvent *pEvent) {
  return true;
}

bool CIrrOdeWorldObserver::handlesEvent(IIrrOdeEvent *pEvent) {
  return true;
}

}
}

/*
CIrrOdeManager *CIrrOdeManager::getSharedInstance() {
  static CIrrOdeManager theManager;
  return &theManager;
}
*/
  #include <observer/CIrrOdeWorldObserver.h>
  #include <event/CIrrOdeEventBodyMoved.h>
  #include <event/CIrrOdeEventBodyRemoved.h>
  #include <event/CIrrOdeEventBodyCreated.h>
  #include <event/CIrrOdeEventNodeCloned.h>
  #include <thread/IThread.h>
  #include <CIrrOdeManager.h>

namespace irr {
namespace ode {

CIrrOdeWorldObserver::CIrrOdeWorldObserver() {
}

CIrrOdeWorldObserver::~CIrrOdeWorldObserver() {
  while (m_lEvents.getSize()>0) {
    irr::core::list<IIrrOdeEvent *>::Iterator it=m_lEvents.begin();
    IIrrOdeEvent *p=*it;
    m_lEvents.erase(it);
    delete p;
  }
}

CIrrOdeWorldObserver *CIrrOdeWorldObserver::getSharedInstance() {
  static CIrrOdeWorldObserver theObserver;
  return &theObserver;
}

void CIrrOdeWorldObserver::install() {
  CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->addEventListener(this);
}

void CIrrOdeWorldObserver::destall() {
  CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->removeEventListener(this);
}

void CIrrOdeWorldObserver::postEvent(IIrrOdeEvent *pEvent) {
  CIrrOdeEventQueue::postEvent(pEvent);
}

void CIrrOdeWorldObserver::addEventListener(IIrrOdeEventListener *pListener) {
  irr::core::list<IIrrOdeEvent *>::Iterator it;

  //when a new irr::core::listener is added we need to send the current state of the simulation
  for (it=m_lEvents.begin(); it!=m_lEvents.end(); it++)
    if (pListener->handlesEvent(*it)) pListener->onEvent(*it);

  CIrrOdeEventQueue::addEventListener(pListener);
}

void CIrrOdeWorldObserver::removeEventListener(IIrrOdeEventListener *pListener) {
  CIrrOdeEventQueue::removeEventListener(pListener);
}

bool CIrrOdeWorldObserver::onEvent(IIrrOdeEvent *pEvent) {
  bool bAddToBuffer=true;

  if (pEvent->getType()==eIrrOdeEventBodyMoved) {
    //if we receieve a "body moved" event we will search the irr::core::list and update
    //the data of the "body moved" event of the same body (if there is one)
    CIrrOdeEventBodyMoved *p=reinterpret_cast<CIrrOdeEventBodyMoved *>(pEvent);
    if (p) {
      irr::core::list<IIrrOdeEvent *>::Iterator it;
      for (it=m_lEvents.begin(); it!=m_lEvents.end(); it++) {
        IIrrOdeEvent *pEvt=*it;
        if (pEvt->getType()==eIrrOdeEventBodyMoved) {
          CIrrOdeEventBodyMoved *pEvtMoved=(CIrrOdeEventBodyMoved *)pEvt;
          if (p->getBodyId()==pEvtMoved->getBodyId()) {
            pEvtMoved->merge(p);
            bAddToBuffer=false;
            break;
          }
        }
      }
    }
  }
  else
    //if we receive a "body removed" message we need to find out whether or not
    //the body was added during simulation time. If this is the case we need
    //to remove all hints of the body, otherwise we need to store the "body removed"
    //event
    if (pEvent->getType()==eIrrOdeEventBodyRemoved) {
      CIrrOdeEventBodyRemoved *pRemoved=(CIrrOdeEventBodyRemoved *)pEvent;
      if (pRemoved->getBodyId()!=-1) {
        bool bBodyCreated=false;
        irr::core::list<IIrrOdeEvent *>::Iterator it;
        for (it=m_lEvents.begin(); it!=m_lEvents.end() && !bBodyCreated; it++) {
          if ((*it)->getType()==eIrrOdeEventNodeCloned) {
            CIrrOdeEventNodeCloned *p=(CIrrOdeEventNodeCloned *)(*it);
            bBodyCreated=p->getNewId()==pRemoved->getBodyId();
            if (bBodyCreated) {
              m_lEvents.erase(it);
              bAddToBuffer=false;
              break;
            }
          }
        }

        for (it=m_lEvents.begin(); it!=m_lEvents.end(); it++) {
          if ((*it)->getType()==eIrrOdeEventBodyMoved) {
            CIrrOdeEventBodyMoved *p=(CIrrOdeEventBodyMoved *)(*it);
            if (p->getBodyId()==pRemoved->getBodyId()) {
              m_lEvents.erase(it);
              break;
            }
          }
        }
      }
    }

  if (pEvent->isObservable() && bAddToBuffer) {
    IIrrOdeEvent *pNewEvent=pEvent->clone();
    if (pNewEvent!=NULL) {
      m_lEvents.push_back(pNewEvent);
    }
  }

  irr::ode::IIrrOdeEvent *p = pEvent->clone();
  if (p != NULL) postEvent(p);
  return true;
}

bool CIrrOdeWorldObserver::handlesEvent(IIrrOdeEvent *pEvent) {
  return true;
}

}
}

  #include <CIrrOdeEventFactory.h>

  #include <event/CIrrOdeEventInit.h>
  #include <event/CIrrOdeEventClose.h>
  #include <event/CIrrOdeEventBeforeStep.h>
  #include <event/CIrrOdeEventBodyCreated.h>
  #include <event/CIrrOdeEventBodyRemoved.h>
  #include <event/CIrrOdeEventBodyMoved.h>
  #include <event/CIrrOdeEventLoadScene.h>
  #include <event/CIrrOdeEventActivationChanged.h>
  #include <event/CIrrOdeEventNodeCloned.h>
  #include <event/CIrrOdeEventRayHit.h>
  #include <event/CIrrOdeEventStep.h>
  #include <event/CIrrOdeEventIdentify.h>

CIrrOdeDefaultEventFactory::CIrrOdeDefaultEventFactory() {
}

IIrrOdeEvent *CIrrOdeDefaultEventFactory::createMessage(CSerializer *pData, ISceneManager *pSmgr) {
  pData->resetBufferPos();
  u16 iCode=pData->getU16();
  switch (iCode) {
    case irr::ode::eIrrOdeEventInit             : return new irr::ode::CIrrOdeEventInit             (pData);
    case irr::ode::eIrrOdeEventClose            : return new irr::ode::CIrrOdeEventClose            (pData);
    case irr::ode::eIrrOdeEventBeforeStep       : return new irr::ode::CIrrOdeEventBeforeStep       (pData);
    case irr::ode::eIrrOdeEventStep             : return new irr::ode::CIrrOdeEventStep             (pData);
    case irr::ode::eIrrOdeEventBodyCreated      : return new irr::ode::CIrrOdeEventBodyCreated      (pData,pSmgr);
    case irr::ode::eIrrOdeEventBodyRemoved      : return new irr::ode::CIrrOdeEventBodyRemoved      (pData,pSmgr);
    case irr::ode::eIrrOdeEventBodyMoved        : return new irr::ode::CIrrOdeEventBodyMoved        (pData,pSmgr);
    case irr::ode::eIrrOdeEventActivationChanged: return new irr::ode::CIrrOdeEventActivationChanged(pData,pSmgr);
    case irr::ode::eIrrOdeEventRayHit           : return new irr::ode::CIrrOdeEventRayHit           (pData,pSmgr);
    case irr::ode::eIrrOdeEventNodeCloned       : printf("clone!\n"); return new irr::ode::CIrrOdeEventNodeCloned       (pData);
    case irr::ode::eIrrOdeEventLoadScene        : return new irr::ode::CIrrOdeEventLoadScene        (pData);
    case irr::ode::eIrrOdeEventIdentify         : return new irr::ode::CIrrOdeEventIdentify         (pData);
    default: return NULL;
  }
}

CIrrOdeEventFactory::CIrrOdeEventFactory() {
  m_pDefaultFact=new CIrrOdeDefaultEventFactory();
  m_lFactories.push_back(m_pDefaultFact);
}

CIrrOdeEventFactory::~CIrrOdeEventFactory() {
  while (m_lFactories.getSize()>0) {
    list<IEventFactory *>::Iterator it=m_lFactories.begin();
    IEventFactory *p=*it;
    m_lFactories.erase(it);
    delete p;
  }
}

CIrrOdeEventFactory *CIrrOdeEventFactory::getSharedEventFactory() {
  static CIrrOdeEventFactory cFactory;
  return &cFactory;
}

IIrrOdeEvent *CIrrOdeEventFactory::createMessage(CSerializer *pData, ISceneManager *pSmgr) {
  list<IEventFactory *>::Iterator it;
  for (it=m_lFactories.begin(); it!=m_lFactories.end(); it++) {
    IIrrOdeEvent *evt=(*it)->createMessage(pData,pSmgr);
    if (evt!=NULL) return evt;
  }
  return NULL;
}

void CIrrOdeEventFactory::registerEventFactory(IEventFactory *p) {
  list<IEventFactory *>::Iterator it;
  for (it=m_lFactories.begin(); it!=m_lFactories.end(); it++) if (*it==p) return;
  m_lFactories.push_back(p);
}

void CIrrOdeEventFactory::setSceneManager(ISceneManager *pSmgr) {
  m_pSmgr=pSmgr;
}

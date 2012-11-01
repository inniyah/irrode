  #include <CIrrOdeManager.h>
  #include <irrlicht.h>
  #include <CIrrOdeSurfaceParameters.h>
  #include <CIrrOdeWorld.h>
  #include <event/IIrrOdeEventListener.h>
  #include <event/CIrrOdeEventBodyMoved.h>
  #include <event/CIrrOdeEventBodyRemoved.h>
  #include <event/CIrrOdeEventInit.h>
  #include <event/CIrrOdeEventClose.h>
  #include <event/CIrrOdeEventProgress.h>
  #include <event/CIrrOdeEventNodeCloned.h>
  #include <event/CIrrOdeEventLoadScene.h>
  #include <event/CIrrOdeEventQueue.h>
  #include <event/CIrrOdeEventNodeRemoved.h>
  #include <event/IIrrOdeEventWriter.h>
  #include <event/CIrrOdeEventJoint.h>
  #include <IIrrOdeDevice.h>
  #include <thread/CFakeThread.h>

  #ifndef _USE_ODE_NULL_DEVICE
    #include <CIrrOdeDevice.h>
  #else
    #include <CIrrOdeNullDevice.h>
  #endif

namespace irr {
namespace ode {

CIrrOdeManager::CIrrOdeManager() {
  m_iLastStep=0;
  m_pTimer=NULL;
  m_iFrameNo=0;
  #ifndef _USE_ODE_NULL_DEVICE
    m_pOdeDevice=new CIrrOdeDevice();
  #else
    m_pOdeDevice=new CIrrOdeNullDevice();
  #endif
  m_bPhysicsInitialized=false;

  m_pDevice=NULL;
  m_pSmgr=NULL;
  m_pTimer=NULL;

  m_pOdeThread = new irr::ode::CFakeThread();
  m_pIrrThread = new irr::ode::CFakeThread();

  m_pOdeThread->getOutputQueue()->addEventListener(m_pIrrThread);
  m_pIrrThread->getOutputQueue()->addEventListener(m_pOdeThread);
}

CIrrOdeManager::~CIrrOdeManager() {
  if(this->physicsInitialized()==true) this->closeODE();
  delete m_pOdeDevice;
}

/**
 * Get the shared singleton instance of the manager
 * @return the shared singleton instance of the manager
 */
CIrrOdeManager *CIrrOdeManager::getSharedInstance() {
  static CIrrOdeManager theManager;
  return &theManager;
}

/**
 * Initialize ODE. This call needs to be done once
 */
void CIrrOdeManager::initODE() {
  CIrrOdeEventInit *pEvent=new CIrrOdeEventInit();
  m_pOdeThread->getOutputQueue()->postEvent(pEvent);

  getOdeDevice()->initODE();
}

/**
 * Shutdown ODE
 */
void CIrrOdeManager::closeODE() {
  getOdeDevice()->closeODE();

  CIrrOdeEventClose *pEvent=new CIrrOdeEventClose();
  m_pOdeThread->getOutputQueue()->postEvent(pEvent);

  irr::core::list<CIrrOdeWorld *>::Iterator wit;
  for (wit=m_lWorlds.begin(); wit!=m_lWorlds.end(); wit++) {
    (*wit)->setPhysicsInitialized(false);
    (*wit)->stopPhysics();
  }

  CIrrOdeWorld *p = *(m_lWorlds.begin());
  if (p) p->clearParameterList();

  m_lWorlds.clear();

  m_bPhysicsInitialized=false;
}

void CIrrOdeManager::install(IrrlichtDevice *pDevice) {
  m_pDevice=pDevice;
  m_pSmgr=m_pDevice->getSceneManager();
  m_pTimer=m_pDevice->getTimer();
  while (m_pTimer->isStopped()) m_pTimer->start();

  m_pIrrThread->getInputQueue()->addEventListener(this);
}
/**
 * Step the worlds
 */
void CIrrOdeManager::step() {
  if (!m_pTimer) printf("###### CIrrOdeManager::step: timer==NULL!\n");
  u32 thisStep=m_pTimer->getTime();
  f32 fStep=((f32)(thisStep-m_iLastStep))/1000;
  m_iFrameNo++;

  irr::core::list<CIrrOdeWorld *>::Iterator it;
  for (it=m_lWorlds.begin(); it!=m_lWorlds.end(); it++) (*it)->step(fStep);

  m_iLastStep=thisStep;
}

/**
 * Retrieve the used ODE device
 */
IIrrOdeDevice *CIrrOdeManager::getOdeDevice() {
  return m_pOdeDevice;
}

/**
 * Set a new ODE device. Remember: you have to destroy the old device yourself, IrrODE will not do this!
 */
void CIrrOdeManager::setOdeDevice(IIrrOdeDevice *pDevice) {
  #ifndef _USE_ODE_NULL_DEVICE
    m_pOdeDevice=pDevice;
  #endif
}

void CIrrOdeManager::findWorlds(irr::scene::ISceneNode *pNode) {
  if (pNode->getType() == irr::ode::IRR_ODE_WORLD_ID) m_lWorlds.push_back(reinterpret_cast<irr::ode::CIrrOdeWorld *>(pNode));

  irr::core::list<irr::scene::ISceneNode *> children = pNode->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it = children.begin(); it != children.end(); it++)
    findWorlds(*it);
}

/**
 * Initialize physics
 */
void CIrrOdeManager::initPhysics() {
  m_bPhysicsInitialized=false;

  findWorlds(m_pSmgr->getRootSceneNode());

  irr::core::list<CIrrOdeWorld *>::Iterator itw;
  for (itw=m_lWorlds.begin(); itw!=m_lWorlds.end(); itw++)
    (*itw)->initPhysics();

  m_bPhysicsInitialized=true;
}

bool CIrrOdeManager::loadScene(const c8 *sScene, irr::scene::ISceneManager *pSmgr) {
  pSmgr->clear();
  pSmgr->getMeshCache()->clear();
  bool bRet=pSmgr->loadScene(sScene);
  if (bRet) {
    CIrrOdeEventLoadScene *pEvt=new CIrrOdeEventLoadScene(sScene);
    m_pOdeThread->getOutputQueue()->postEvent(pEvt);
  }
  return bRet;
}

bool CIrrOdeManager::onEvent(IIrrOdeEvent *pEvt) {
  if (pEvt==NULL) return false;

  if (pEvt->getType()==eIrrOdeEventBodyMoved) {
    CIrrOdeEventBodyMoved *pEvent=(CIrrOdeEventBodyMoved *)pEvt;
    CIrrOdeBody *pBody=pEvent->getBody();
    if (pBody==NULL) pBody=reinterpret_cast<CIrrOdeBody *>(m_pSmgr->getSceneNodeFromId(pEvent->getBodyId()));
    if (pBody!=NULL) pBody->onEvent(pEvt);

    return true;
  }

  if (pEvt->getType()==eIrrOdeEventBodyRemoved) {
    CIrrOdeEventBodyRemoved *pBr=(CIrrOdeEventBodyRemoved *)pEvt;
    CIrrOdeBody *pBody=pBr->getBody();

    if (pBody==NULL) pBody=reinterpret_cast<CIrrOdeBody *>(m_pSmgr->getSceneNodeFromId(pBr->getBodyId()));
    if (pBody!=NULL) pBody->onEvent(pEvt);

    return true;
  }

  if (pEvt->getType()==eIrrOdeEventNodeRemoved) {
    CIrrOdeEventNodeRemoved *pNr=(CIrrOdeEventNodeRemoved *)pEvt;
    irr::scene::ISceneNode *pNode=pNr->getRemovedNode();
    if (pNode==NULL && pNr->getRemovedNodeId()!=-1) {
      pNode=m_pSmgr->getSceneNodeFromId(pNr->getRemovedNodeId());
    }

    if (pNode!=NULL) m_pSmgr->addToDeletionQueue(pNode);
  }

  if (pEvt->getType()==eIrrOdeEventJoint) {
    CIrrOdeEventJoint *p=(CIrrOdeEventJoint *)pEvt;
    CIrrOdeJoint *pJoint=p->getJoint();
    pJoint->onEvent(p);
  }

  return false;
}

bool CIrrOdeManager::handlesEvent(IIrrOdeEvent *pEvent) {
  return pEvent->getType()==eIrrOdeEventBodyMoved   ||
         pEvent->getType()==eIrrOdeEventBodyRemoved ||
         pEvent->getType()==eIrrOdeEventNodeRemoved ||
         pEvent->getType()==eIrrOdeEventJoint;
}

} //namespace ode
} //namespace irr

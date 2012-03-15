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
  m_iNodesInitialized=0;
  m_bPhysicsInitialized=false;
  m_iNextOdeNodeId=20000;

  m_pQueue=new CIrrOdeEventQueue();
  m_pDevice=NULL;
  m_pSmgr=NULL;
  m_pTimer=NULL;
}

CIrrOdeManager::~CIrrOdeManager() {
  if(this->physicsInitialized()==true) this->closeODE();
  this->getIrrOdeNodes().clear();
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
  m_pQueue->postEvent(pEvent);

  getOdeDevice()->initODE();
}

/**
 * Shutdown ODE
 */
void CIrrOdeManager::closeODE() {
  getOdeDevice()->closeODE();

  CIrrOdeEventClose *pEvent=new CIrrOdeEventClose();
  m_pQueue->postEvent(pEvent);

  irr::core::list<CIrrOdeWorld *>::Iterator wit;
  for (wit=m_lWorlds.begin(); wit!=m_lWorlds.end(); wit++) (*wit)->setPhysicsInitialized(false);
  irr::core::list<CIrrOdeSceneNode *>::Iterator nit;
  for (nit=m_pSceneNodes.begin(); nit!=m_pSceneNodes.end(); nit++) (*nit)->setPhysicsInitialized(false);

  m_pSceneNodes.clear();
  m_lWorlds.clear();

  m_iNodesInitialized=0;
  m_bPhysicsInitialized=false;
}

void CIrrOdeManager::clearODE() {
  getOdeDevice()->clearODE();
  m_pSceneNodes.clear();

  m_iNodesInitialized=0;
  m_bPhysicsInitialized=false;

  CIrrOdeEventClose *pEvent=new CIrrOdeEventClose();
  m_pQueue->postEvent(pEvent);

  irr::core::list<irr::ode::CIrrOdeWorld *>::Iterator it;
  for (it = m_lWorlds.begin(); it != m_lWorlds.end(); it++) (*it)->stopPhysics();

  CIrrOdeWorld *p = *(m_lWorlds.begin());
  if (p) p->clearParameterList();
}

void CIrrOdeManager::install(IrrlichtDevice *pDevice) {
  m_pDevice=pDevice;
  m_pSmgr=m_pDevice->getSceneManager();
  m_pTimer=m_pDevice->getTimer();
  while (m_pTimer->isStopped()) m_pTimer->start();

  m_pQueue->addEventListener(this);
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
 * Add a new ODE scene node
 */
void CIrrOdeManager::addOdeSceneNode(CIrrOdeSceneNode *pNode) {
  if (pNode->getID()>m_iNextOdeNodeId) m_iNextOdeNodeId=pNode->getID();
  m_pSceneNodes.push_back(pNode);
}

/**
 * Remove an ODE scene node
 */
void CIrrOdeManager::removeOdeSceneNode(CIrrOdeSceneNode *pNode) {
  irr::core::list<CIrrOdeSceneNode *>::Iterator it;
  for (it=m_pSceneNodes.begin(); it!=m_pSceneNodes.end(); it++)
    if ((*it)==pNode) {
      m_pSceneNodes.erase(it);
      return;
    }
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
  m_iNodesInitialized=0;
  m_bPhysicsInitialized=false;

  findWorlds(m_pSmgr->getRootSceneNode());

  irr::core::list<CIrrOdeWorld *>::Iterator itw;
  for (itw=m_lWorlds.begin(); itw!=m_lWorlds.end(); itw++)
    (*itw)->initPhysics();

  m_bPhysicsInitialized=true;
}

irr::core::list<CIrrOdeSceneNode *> &CIrrOdeManager::getIrrOdeNodes() {
  return m_pSceneNodes;
}

void CIrrOdeManager::sceneNodeInitialized(CIrrOdeSceneNode *pNode) {
  if (!m_bPhysicsInitialized) {
    irr::core::list<CIrrOdeSceneNode *>::Iterator it;
    bool b=false;
    for (it=m_pSceneNodes.begin(); it!=m_pSceneNodes.end() && !b; it++)
      if (*it==pNode) b=true;

    if (b) {
      m_iNodesInitialized++;
      CIrrOdeEventProgress *pPrg=new CIrrOdeEventProgress (m_iNodesInitialized,m_pSceneNodes.getSize());
      m_pQueue->postEvent(pPrg);
    }
  }
}

CIrrOdeDampable *CIrrOdeManager::getDampableWithParamName(const wchar_t *sName) {
  irr::core::list<CIrrOdeWorld *>::Iterator wit;
  for (wit=m_lWorlds.begin(); wit!=m_lWorlds.end(); wit++) {
		CIrrOdeWorld *p=*wit;
		if (!wcscmp(sName,p->getParamName())) return p;
	}

	irr::core::list<CIrrOdeSceneNode *>::Iterator it;
	for (it=m_pSceneNodes.begin(); it!=m_pSceneNodes.end(); it++) {
		if ((*it)->getType()==irr::ode::IRR_ODE_BODY_ID) {
			CIrrOdeBody *p=reinterpret_cast<CIrrOdeBody *>(*it);
      if (!wcscmp(sName,p->getParamName())) return p;
		}
	}

	return NULL;
}

irr::scene::ISceneNode *CIrrOdeManager::cloneTree(irr::scene::ISceneNode *pSource, irr::scene::ISceneNode *newParent, irr::scene::ISceneManager *newSmgr) {
  irr::scene::ISceneNode *pRet=cloneOdeNode(pSource,newParent,newSmgr);

  CIrrOdeEventNodeCloned *pEvent=new CIrrOdeEventNodeCloned(pSource->getID(),pRet->getID());
  m_pQueue->postEvent(pEvent);

  return pRet;
}

irr::scene::ISceneNode *CIrrOdeManager::cloneOdeNode(irr::scene::ISceneNode *pSource, irr::scene::ISceneNode *newParent, irr::scene::ISceneManager *newSmgr, s32 iNewId) {
  irr::scene::ISceneNode *pRet=pSource->clone(newParent,newSmgr);
  pRet->setID(iNewId==-1?getNextId():iNewId);
  pRet->setParent(newParent);

  return pRet;
}


void CIrrOdeManager::removeTreeFromPhysics(irr::scene::ISceneNode *pNode) {
  irr::core::list<irr::scene::ISceneNode *> ch=pNode->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=ch.begin(); it!=ch.end(); it++) {
    if (isRegisteredOdeSceneNode(*it)) {
      CIrrOdeSceneNode *p=(CIrrOdeSceneNode *)(*it);
      p->removeFromPhysics();
    }
    removeTreeFromPhysics(*it);
  }
}

void CIrrOdeManager::removeSceneNode(irr::scene::ISceneNode *pNode) {
  CIrrOdeEventNodeRemoved *p=new CIrrOdeEventNodeRemoved(pNode);
  m_pQueue->postEvent(p);
}

bool CIrrOdeManager::loadScene(const c8 *sScene, irr::scene::ISceneManager *pSmgr) {
  pSmgr->clear();
  pSmgr->getMeshCache()->clear();
  bool bRet=pSmgr->loadScene(sScene);
  if (bRet) {
    CIrrOdeEventLoadScene *pEvt=new CIrrOdeEventLoadScene(sScene);
    m_pQueue->postEvent(pEvt);
  }
  return bRet;
}

bool CIrrOdeManager::isRegisteredOdeSceneNode(irr::scene::ISceneNode *pNode) {
  irr::core::list<irr::ode::CIrrOdeSceneNode *>::Iterator it;

  for (it=m_pSceneNodes.begin(); it!=m_pSceneNodes.end(); it++)
    if ((*it)==pNode)
      return true;

  return false;
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

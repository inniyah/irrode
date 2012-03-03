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
  #include <motors/IIrrOdeStepMotor.h>

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

void CIrrOdeManager::addWorld(CIrrOdeWorld *pWorld) {
  irr::core::list<CIrrOdeWorld *>::Iterator it;
  for (it=m_lWorlds.begin(); it!=m_lWorlds.end(); it++)
    if ((*it)==pWorld)
      return;

  m_lWorlds.push_back(pWorld);
}

void CIrrOdeManager::removeWorld(CIrrOdeWorld *pWorld) {
  irr::core::list<CIrrOdeWorld *>::Iterator it;
  for (it=m_lWorlds.begin(); it!=m_lWorlds.end(); it++)
    if ((*it)==pWorld) {
      m_lWorlds.erase(it);
      return;
    }
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
  irr::core::list<IIrrOdeStepMotor *>::Iterator mit;
  for (mit=m_lStepMotors.begin(); mit!=m_lStepMotors.end(); mit++) (*mit)->setPhysicsInitialized(false);

  m_pSceneNodes.clear();
  m_lParamList.clear();
  m_lStepMotors.clear();
  m_lWorlds.clear();

  m_iNodesInitialized=0;
  m_bPhysicsInitialized=false;
}

void CIrrOdeManager::clearODE() {
  getOdeDevice()->clearODE();
  m_pSceneNodes.clear();
  m_lParamList.clear();

  m_iNodesInitialized=0;
  m_bPhysicsInitialized=false;

  CIrrOdeEventClose *pEvent=new CIrrOdeEventClose();
  m_pQueue->postEvent(pEvent);
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

  irr::core::list<IIrrOdeEventWriter *>::Iterator cit;
  for (cit=m_lChanged.begin(); cit!=m_lChanged.end(); cit++) {
    IIrrOdeEvent *p=m_pOdeDevice->writeEventFor(*cit);
    if (p==NULL) p=(*cit)->writeEvent();
    if (p) getQueue()->postEvent(p);
  }
  m_lChanged.clear();

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

void CIrrOdeManager::removeEventWriter(IIrrOdeEventWriter *p) {
  irr::core::list<IIrrOdeEventWriter *>::Iterator it;
  for (it=m_lChanged.begin(); it!=m_lChanged.end(); it++)
    if (*it==p) {
      m_lChanged.erase(it);
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

/**
 * Initialize physics
 */
void CIrrOdeManager::initPhysics() {
  m_iNodesInitialized=0;
  m_bPhysicsInitialized=false;

  irr::core::list<CIrrOdeWorld *>::Iterator itw;
  for (itw=m_lWorlds.begin(); itw!=m_lWorlds.end(); itw++)
    (*itw)->initPhysics();

  irr::core::list<irr::ode::IIrrOdeStepMotor *>::Iterator it;
  for (it=m_lStepMotors.begin(); it!=m_lStepMotors.end(); it++) (*it)->initPhysics();

  m_bPhysicsInitialized=true;
}

irr::core::list<CIrrOdeSceneNode *> &CIrrOdeManager::getIrrOdeNodes() {
  return m_pSceneNodes;
}

irr::core::list<CIrrOdeWorld *> &CIrrOdeManager::getWorlds() {
  return m_lWorlds;
}

static irr::core::array<irr::core::stringc> g_aParamNames;
static irr::core::array<const c8 *> g_aC8ParamNames;

void CIrrOdeManager::updateSurfaceParameterList() {
  g_aParamNames.clear();
  g_aC8ParamNames.clear();

  irr::core::list<CIrrOdeSurfaceParameters *>::Iterator it;
  for (it=m_lParamList.begin(); it!=m_lParamList.end(); it++) {
    irr::core::stringc s=((*it)->getName());
    if (s!="") {
      bool bAdd=true;
      for (u32 i=0; i<g_aParamNames.size() && bAdd; i++) if (g_aParamNames[i]==s) bAdd=false;
      if (bAdd) g_aParamNames.push_back(s);
    }
  }

  for (u32 i=0; i<g_aParamNames.size(); i++) g_aC8ParamNames.push_back(g_aParamNames[i].c_str());
  g_aC8ParamNames.push_back(NULL);
}

void CIrrOdeManager::addSurfaceParameter(CIrrOdeSurfaceParameters *pParam) {
  m_lParamList.push_back(pParam);
  updateSurfaceParameterList();
}

void CIrrOdeManager::removeSurfaceParameter(CIrrOdeSurfaceParameters *pParam) {
  irr::core::list<CIrrOdeSurfaceParameters *>::Iterator it;
  for (it=m_lParamList.begin(); it!=m_lParamList.end(); it++)
    if ((*it)==pParam) {
      m_lParamList.erase(it);
      updateSurfaceParameterList();
      return;
    }
}

CIrrOdeSurfaceParameters *CIrrOdeManager::getSurfaceParameter(irr::core::stringw sName) {
	irr::core::list<CIrrOdeSurfaceParameters *>::Iterator it;

	#ifdef _TRACE_INIT_PHYSICS
	  printf("\tsearching for surface parameters \"%s\" ... ",irr::core::stringc(sName).c_str());
  #endif

	if (sName=="") {
	  #ifdef _TRACE_INIT_PHYSICS
	    printf("no search name defined!\n");
    #endif
	  return &m_cNullSurface;
	}

	for (it=m_lParamList.begin(); it!=m_lParamList.end(); it++)
		if (sName==irr::core::stringw((*it)->getName())) {
		  #ifdef _TRACE_INIT_PHYSICS
		    printf("OK\n");
      #endif
		  return *it;
		}

  #ifdef _TRACE_INIT_PHYSICS
    printf("not found!\n");
  #endif
	return &m_cNullSurface;
}

const c8 *const *CIrrOdeManager::getSurfaceParameterList() {
  return g_aC8ParamNames.const_pointer();
}

void CIrrOdeManager::addStepMotor(IIrrOdeStepMotor *pMotor) {
  irr::core::list<IIrrOdeStepMotor *>::Iterator it;
  for (it=m_lStepMotors.begin(); it!=m_lStepMotors.end(); it++) if (*it==pMotor) return;
  m_lStepMotors.push_back(pMotor);
}

void CIrrOdeManager::removeStepMotor(IIrrOdeStepMotor *pMotor) {
  irr::core::list<IIrrOdeStepMotor *>::Iterator it;
  for (it=m_lStepMotors.begin(); it!=m_lStepMotors.end(); it++) if (*it==pMotor) { m_lStepMotors.erase(it); return; }
}

void CIrrOdeManager::stepStepMotors() {
  irr::core::list<IIrrOdeStepMotor *>::Iterator it;
  for (it=m_lStepMotors.begin(); it!=m_lStepMotors.end(); it++) (*it)->step();
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

void CIrrOdeManager::objectChanged(IIrrOdeEventWriter *p) {
  irr::core::list<IIrrOdeEventWriter *>::Iterator it;
  for (it=m_lChanged.begin(); it!=m_lChanged.end(); it++) if (*it==p) return;
  m_lChanged.push_back(p);
}

} //namespace ode
} //namespace irr

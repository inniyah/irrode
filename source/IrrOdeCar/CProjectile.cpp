  #include <CProjectile.h>
  #include <geom/CIrrOdeGeom.h>
  #include <CIrrOdeBody.h>
  #include <CIrrOdeManager.h>
  #include <CAdvancedParticleSystemNode.h>

void CProjectile::findParticleSystems(irr::scene::ISceneNode *pNode) {
  if (pNode->getType()==(irr::scene::ESCENE_NODE_TYPE)ADVANCED_PARTICLE_NODE_ID) {
    m_aParticleSystems.push_back((CAdvancedParticleSystemNode *)pNode);
  }
  list<ISceneNode *> children=pNode->getChildren();
  list<ISceneNode *>::Iterator i;

  for (i=children.begin(); i!=children.end(); i++)
    findParticleSystems(*i);
}

void CProjectile::findMesh(irr::scene::ISceneNode *pNode) {
  if (pNode->getType()==irr::scene::ESNT_ANIMATED_MESH) {
    m_pNode=(irr::scene::IAnimatedMeshSceneNode *)pNode;
    return;
  }

  list<ISceneNode *> children=pNode->getChildren();
  list<ISceneNode *>::Iterator i;

  for (i=children.begin(); i!=children.end(); i++) findMesh(*i);
}

CProjectile::CProjectile(irr::scene::ISceneManager *pSmgr, irr::core::vector3df vPos, irr::core::vector3df vRot, irr::core::vector3df vVel, const irr::c8 *sSource, irr::s32 iTtl, irr::scene::ISceneNode *pWorld, bool bFastCollision) {
  m_iTtl=iTtl;
  m_pSmgr=pSmgr;
  m_bActive=true;
  m_pNode=NULL;

  irr::ode::CIrrOdeBody *pSource=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName(sSource));

  if (pSource!=NULL) {
    irr::scene::ISceneNode *pNewNode=irr::ode::CIrrOdeManager::getSharedInstance()->cloneTree(pSource,pWorld,m_pSmgr);
    m_pBody=reinterpret_cast<irr::ode::CIrrOdeBody *>(pNewNode);
    if (m_pBody!=NULL) {
      findParticleSystems(m_pBody);
      findMesh(m_pBody);

      m_pBody->setPosition(vPos);
      m_pBody->setRotation(vRot);
      m_pBody->setLinearVelocity(vVel);
      m_pBody->setIsFastMoving(bFastCollision);
      m_pBody->initPhysics();

      //last but not least we add the shot to our list
      CProjectileManager::getSharedInstance()->addShot(this);
    }
  }
}

//get the body of the projectile for the follow cam
irr::ode::CIrrOdeBody *CProjectile::getBody() {
  return m_pBody;
}

//Get the remaining lifetime of the projectile
//This one is used by the projectilemanager to
//remove a projectile.
irr::s32 CProjectile::getTtl() {
  return m_iTtl;
}

//this method is called on each CIrrOdeEventStep event
void CProjectile::step() {
  //reduce the remaining lifetime
  m_iTtl--;

  //if the lifetime has reached 0 ...
  if (m_iTtl<=0 && m_bActive) {
    //... we remove the body
    m_pBody->removeFromPhysics();
    m_bActive=false;
    //Toggling the particle systems and removing the scene node has been moved
    //to the CCustomEventReceiver which is reused in the plugin for the RePlayer
  }
}

bool CProjectile::particlesActive() {
  u32 iParticles=0;
  for (u32 i=0; i<m_aParticleSystems.size(); i++)
    iParticles+=m_aParticleSystems[i]->getParticleCount();

  bool bRet=iParticles!=0;

  if (!bRet) irr::ode::CIrrOdeManager::getSharedInstance()->removeSceneNode(m_pBody);

  return bRet;
}
//if a collision occured we set the remaining
//lifetime to zero
void CProjectile::collide() {
  m_iTtl=0;
}

CProjectileManager::CProjectileManager() {
  m_iShots=0;
  m_iHits=0;
  m_pLast=NULL;

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
}

CProjectileManager::~CProjectileManager() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

CProjectileManager *CProjectileManager::getSharedInstance() {
  static CProjectileManager theManager;
  return &theManager;
}

void CProjectileManager::addShot(CProjectile *p) {
  m_lShots.push_back(p);
  m_pLast=p;
  m_iShots++;
}

bool CProjectileManager::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    irr::core::list<CProjectile *>::Iterator it;
    for (it=m_lShots.begin(); it!=m_lShots.end(); it++) {
      CProjectile *p=*it;
      p->step();
      if (p->getTtl()<=0 && !p->particlesActive()) {
        if (m_pLast==p) m_pLast=NULL;
        m_lShots.erase(it);
        delete p;
        return false;
      }
    }
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventBodyMoved) {
    irr::ode::CIrrOdeEventBodyMoved *pMove=(irr::ode::CIrrOdeEventBodyMoved *)pEvent;
    irr::core::list<CProjectile *>::Iterator it;
    for (it=m_lShots.begin(); it!=m_lShots.end(); it++) {
      CProjectile *p=*it;
      if (p->getBody()==pMove->getBody() && pMove->getTouched()!=NULL) {
        irr::ode::CIrrOdeGeom *g=pMove->getTouched();
        if (g->getBody()!=NULL) m_iHits++;
        p->collide();
      }
    }
  }

  return false;
}

bool CProjectileManager::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventBodyMoved || pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

CProjectile *CProjectileManager::getLast() {
  return m_pLast;
}

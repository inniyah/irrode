  #include <CProjectile.h>
  #include <geom/CIrrOdeGeom.h>
  #include <CIrrOdeBody.h>
  #include <CIrrOdeManager.h>
  #include <CIrrOdeCarState.h>
  #include <CAdvancedParticleSystemNode.h>
  #include <CAutoPilot.h>
  #include <irrlicht.h>
#ifndef NO_IRRKLANG
  #include <irrKlang.h>
#endif
  #include <thread/IThread.h>
  #include <CEventVehicleState.h>

void CProjectile::findParticleSystems(irr::scene::ISceneNode *pNode) {
  if (pNode->getType()==(irr::scene::ESCENE_NODE_TYPE)ADVANCED_PARTICLE_NODE_ID) {
    m_aParticleSystems.push_back((CAdvancedParticleSystemNode *)pNode);
  }
  list<ISceneNode *> children=pNode->getChildren();
  list<ISceneNode *>::Iterator i;

  for (i=children.begin(); i!=children.end(); i++) {
    findParticleSystems(*i);
    if ((*i)->getType()==irr::ode::IRR_ODE_AERO_DRAG_ID) {
      m_pAero=(irr::ode::CIrrOdeAeroDrag *)(*i);
      printf("\t\t**** aero drag!\n");
    }
    if ((*i)->getType()==irr::ode::IRR_ODE_TORQUE_MOTOR_ID) {
      m_pTorque=(irr::ode::CIrrOdeTorqueMotor *)(*i);
      printf("\t\t**** torque motor: %p!\n",(void *)m_pTorque);
    }
  }
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

void CProjectile::setTarget(irr::ode::CIrrOdeBody *pTarget) {
  m_pTarget=pTarget;
  if (m_pAutoPilot==NULL) m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor);
  m_pAutoPilot->setState(CAutoPilot::eApMissile);
  m_pAutoPilot->setEnabled(true);
  m_pAutoPilot->setTarget(m_pTarget);
}

CProjectile::CProjectile(irr::scene::ISceneManager *pSmgr, irr::core::vector3df vPos, irr::core::vector3df vRot, irr::core::vector3df vVel, const irr::c8 *sSource, irr::s32 iTtl, irr::ode::CIrrOdeWorld *pWorld, bool bFastCollision, CIrrOdeCarState *pShooter) {
  m_iTtl=iTtl;
  m_pSmgr=pSmgr;
  m_bActive=true;
  m_pNode=NULL;
  m_pTarget=NULL;
  m_pAero=NULL;
  m_pTorque=NULL;
  m_pAutoPilot=NULL;
  m_pShooter=pShooter;

  m_pWorld = pWorld;

  irr::ode::CIrrOdeBody *pSource=reinterpret_cast<irr::ode::CIrrOdeBody *>(m_pSmgr->getSceneNodeFromName(sSource));

  if (pSource!=NULL) {
    irr::scene::ISceneNode *pNewNode=pWorld->cloneTree(pSource,pWorld,m_pSmgr);
    pNewNode->setName("projectile_clone");
    m_pBody=reinterpret_cast<irr::ode::CIrrOdeBody *>(pNewNode);
    if (m_pBody!=NULL) {
      findParticleSystems(m_pBody);
      findMesh(m_pBody);

      m_pBody->setPosition(vPos);
      m_pBody->setRotation(vRot);
      m_pBody->setLinearVelocity(vVel);
      m_pBody->setIsFastMoving(bFastCollision);
      m_pBody->initPhysics();

      if (strcmp(sSource,"missile")) {
        CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndFireShell,1.0f,vPos);
        ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
      }

      if (!strcmp(sSource,"bullet")) m_fVolume=0.3f; else m_fVolume=1.0f;

      //last but not least we add the shot to our list
      CProjectileManager::getSharedInstance()->addShot(this);
    }
  }
}

CProjectile::~CProjectile() {
  if (m_pAutoPilot!=NULL) delete m_pAutoPilot;
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
    CEventFireSound *p=new CEventFireSound(CEventFireSound::eSndExplode,m_fVolume,m_pBody->getPosition());
    ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);

    //... we remove the body
    m_pBody->removeFromPhysics();
    m_bActive=false;
    //Toggling the particle systems and removing the scene node has been moved
    //to the CCustomEventReceiver which is reused in the plugin for the RePlayer
  }
  else
    if (m_bActive && m_pTarget!=NULL && m_pAero!=NULL && m_pTorque!=NULL) {
      f32 fYaw,fPitch,fRoll,fThrust;
      m_pAutoPilot->step(fYaw,fPitch,fRoll,fThrust);

      if (fYaw  ==fYaw  ) m_pTorque->setYaw  (fYaw  );
      if (fPitch==fPitch) m_pTorque->setPitch(fPitch);
      if (fRoll ==fRoll ) m_pTorque->setRoll (fRoll );
    }
}

bool CProjectile::particlesActive() {
  u32 iParticles=0;
  for (u32 i=0; i<m_aParticleSystems.size(); i++)
    iParticles+=m_aParticleSystems[i]->getParticleCount();

  bool bRet=iParticles!=0;

  if (!bRet && m_pWorld != NULL) m_pWorld->removeSceneNode(m_pBody);

  return bRet;
}
//if a collision occured we set the remaining
//lifetime to zero
void CProjectile::collide(CIrrOdeCarState *pStateHit) {
  m_iTtl=0;
  if (m_pShooter!=NULL && pStateHit!=NULL) {
    if (m_pShooter!=pStateHit) m_pShooter->incHitsScored();
    pStateHit->incHitsTaken();
    m_pShooter=NULL;
  }
}

CProjectileManager::CProjectileManager() {
  m_iShots=0;
  m_iHits=0;
  m_pLast=NULL;

  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->addEventListener(this);
}

CProjectileManager::~CProjectileManager() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->removeEventListener(this);
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

    m_lGasStationCheck.clear();
    while (m_lGasStationIn.size()>0) {
      core::list<s32>::Iterator sit=m_lGasStationIn.begin();
      m_lGasStationCheck.push_back(*sit);
      m_lGasStationIn.erase(sit);
    }
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventBodyMoved) {
    irr::ode::CIrrOdeEventBodyMoved *pMove=(irr::ode::CIrrOdeEventBodyMoved *)pEvent;
    irr::core::list<CProjectile *>::Iterator it;
    for (it=m_lShots.begin(); it!=m_lShots.end(); it++) {
      CProjectile *p=*it;
      if (p->getBody()==pMove->getBody() && pMove->getTouched()!=NULL) {
        irr::ode::CIrrOdeGeom *g=pMove->getTouched();
        CIrrOdeCarState *pStateHit=NULL;
        if (g->getBody()!=NULL) {
          m_iHits++;

          irr::ode::CIrrOdeBody *pBodyHit=g->getBody();
          while (pBodyHit->getParentBody()!=NULL) pBodyHit=pBodyHit->getParentBody();
          void *pUserData=pBodyHit->getUserData();
          if (pUserData!=NULL) {
            pStateHit=reinterpret_cast<CIrrOdeCarState *>(pUserData);
          }
        }
        p->collide(pStateHit);
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

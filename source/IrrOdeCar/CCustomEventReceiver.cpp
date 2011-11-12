  #include <CCustomEventReceiver.h>
  #include <CEventVehicleState.h>
  #include <CAdvancedParticleSystemNode.h>

using namespace irr;

CCustomEventReceiver::CCustomEventReceiver() {
  m_pDevice=NULL;
  m_pOdeManager=NULL;
  m_bInstalled=false;
}

CCustomEventReceiver::~CCustomEventReceiver() {
  if (m_bInstalled) m_pOdeManager->getQueue()->removeEventListener(this);
}

void CCustomEventReceiver::install() {
  if (m_bInstalled) return;
  m_bInstalled=true;

  m_pOdeManager->getQueue()->addEventListener(this);
}

void CCustomEventReceiver::destall() {
  m_pOdeManager->getQueue()->removeEventListener(this);

  while (m_lPlanes.getSize()>0) {
    list<SPlaneNodes *>::Iterator it=m_lPlanes.begin();
    SPlaneNodes *p=*it;
    m_lPlanes.erase(it);
    delete p;
  }

  while (m_lTanks.getSize()>0) {
    list<STankNodes *>::Iterator it=m_lTanks.begin();
    STankNodes *p=*it;
    m_lTanks.erase(it);
    delete p;
  }

  m_bInstalled=false;
}

void CCustomEventReceiver::searchPlaneNodes(irr::scene::ISceneNode *pNode, SPlaneNodes *pPlane) {
  core::list<scene::ISceneNode *> children=pNode->getChildren();
  core::list<scene::ISceneNode *>::Iterator it;

  if (!strcmp(pNode->getName(),"yaw"   ) || !strcmp(pNode->getName(),"yaw2"  )) pPlane->aYaw  .push_back(pNode);
  if (!strcmp(pNode->getName(),"pitch1") || !strcmp(pNode->getName(),"pitch2")) pPlane->aPitch.push_back(pNode);
  if (!strcmp(pNode->getName(),"roll1" ) || !strcmp(pNode->getName(),"roll2" )) pPlane->aRoll .push_back(pNode);

  for (it=children.begin(); it!=children.end(); it++) {
    searchPlaneNodes(*it,pPlane);
  }
}

void CCustomEventReceiver::searchTankNodes(irr::scene::ISceneNode *pNode, STankNodes *pTank) {
  core::list<scene::ISceneNode *> children=pNode->getChildren();
  core::list<scene::ISceneNode *>::Iterator it;

  if (!strcmp(pNode->getName(),"cannonAxis")) { pTank->pCannon=pNode; }
  if (!strcmp(pNode->getName(),"turretAxis")) { pTank->pTurret=pNode; }

  irr::core::stringc aNames[]={"tankAxisRR","tankAxisFR","tankAxisRL","tankAxisFL"};

  for (irr::u32 i=0; i<4; i++)
    if (!strcmp(pNode->getName(),aNames[i].c_str())) {
      pTank->aAxes.push_back(pNode);
    }

  for (it=children.begin(); it!=children.end(); it++) {
    searchTankNodes(*it,pTank);
  }
}

void CCustomEventReceiver::searchCarNodes(irr::scene::ISceneNode *pNode, SCarNodes *pCar) {
  core::list<scene::ISceneNode *> children=pNode->getChildren();
  core::list<scene::ISceneNode *>::Iterator it;

  if (!strcmp(pNode->getName(),"sc_wheel_rl"       )) pCar->pRearWheels[0]=pNode;
  if (!strcmp(pNode->getName(),"sc_wheel_rr"       )) pCar->pRearWheels[1]=pNode;
  if (!strcmp(pNode->getName(),"sc_suspension_rear")) pCar->pSuspension   =pNode;

  if (!strcmp(pNode->getName(),"smoke_1")) pCar->pSmoke[0]=reinterpret_cast<CAdvancedParticleSystemNode *>(pNode);
  if (!strcmp(pNode->getName(),"smoke_2")) pCar->pSmoke[1]=reinterpret_cast<CAdvancedParticleSystemNode *>(pNode);

  for (it=children.begin(); it!=children.end(); it++) searchCarNodes(*it,pCar);
}

void CCustomEventReceiver::setMembers(irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMgr) {
  CCustomEventReceiver::getSharedInstance()->m_pDevice=pDevice;
  CCustomEventReceiver::getSharedInstance()->m_pOdeManager=pOdeMgr;
}

CCustomEventReceiver *CCustomEventReceiver::getSharedInstance() {
  static CCustomEventReceiver theReceiver;
  return &theReceiver;
}

void CCustomEventReceiver::addPlane(irr::scene::ISceneNode *pPlane) {
  SPlaneNodes *pNodes=new SPlaneNodes();
  pNodes->iNodeId=pPlane->getID();
  searchPlaneNodes(pPlane,pNodes);
  m_lPlanes.push_back(pNodes);
}

void CCustomEventReceiver::addTank(irr::scene::ISceneNode *pTank) {
  STankNodes *pNodes=new STankNodes();
  pNodes->pCannon=NULL;
  pNodes->iNodeId=pTank->getID();
  searchTankNodes(pTank,pNodes);
  m_lTanks.push_back(pNodes);
}

void CCustomEventReceiver::addCar(irr::scene::ISceneNode *pCar) {
  SCarNodes *pNodes=new SCarNodes();
  pNodes->iNodeId=pCar->getID();
  searchCarNodes(pCar,pNodes);
  m_lCars.push_back(pNodes);
}

/**
 * This method deactivates all particles systems that are children of a node
 * @param pNode the node
 */
void CCustomEventReceiver::deactivateParticleSystems(irr::scene::ISceneNode *pNode) {
  if (pNode->getType()==ADVANCED_PARTICLE_NODE_ID) {
    CAdvancedParticleSystemNode *p=(CAdvancedParticleSystemNode *)pNode;
    //We deactivate the normal particle systems and activate those for explosions
    //which will automatically be deactivated once finished.
    p->setIsActive(!p->isActive());
  }
  list<ISceneNode *> children=pNode->getChildren();
  list<ISceneNode *>::Iterator i;
  for (i=children.begin(); i!=children.end(); i++) deactivateParticleSystems(*i);
}

/**
 * This method hides the projectile which is an animated mesh
 * @param pNode the node to check
 */
void CCustomEventReceiver::hideAnimatedMesh(irr::scene::ISceneNode *pNode) {
  if (pNode->getType()==irr::scene::ESNT_ANIMATED_MESH) {
    irr::scene::IAnimatedMeshSceneNode *p=(irr::scene::IAnimatedMeshSceneNode *)pNode;
    p->setVisible(false);
  }
  list<ISceneNode *> children=pNode->getChildren();
  list<ISceneNode *>::Iterator i;
  for (i=children.begin(); i!=children.end(); i++) hideAnimatedMesh(*i);
}

bool CCustomEventReceiver::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==EVENT_PLANE_STATE_ID) {
    CEventPlaneState *p=(CEventPlaneState *)pEvent;
    list<SPlaneNodes *>::Iterator it;
    for (it=m_lPlanes.begin(); it!=m_lPlanes.end(); it++) {
      if ((*it)->iNodeId==p->getNodeId()) {
        SPlaneNodes *nodes=*it;
        u32 i;
        f32 fYaw=p->getYaw(),fPitch=p->getPitch(),fRoll=p->getRoll();
        bool tw=p->isThreeWheeler();

        for(i=0; i<nodes->aRoll.size(); i++)
          nodes->aRoll[i]->setRotation(vector3df(i==0?15.0f*fRoll:-15.0f*fRoll,i==0?4:-4,0));

        for(i=0; i<nodes->aPitch.size(); i++)
          nodes->aPitch[i]->setRotation(vector3df(fPitch*-15.0f,i==0?-8:8,0));

        if (nodes->aYaw.size()>0) nodes->aYaw[0]->setRotation(vector3df(0,tw?90+10.0f*fYaw:90-10.0f*fYaw, 0));
        if (nodes->aYaw.size()>1) nodes->aYaw[1]->setRotation(vector3df(-15.0f*fYaw,13,90));

        return true;
      }
    }
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventBodyRemoved) {
    irr::ode::CIrrOdeEventBodyRemoved *p=(irr::ode::CIrrOdeEventBodyRemoved *)pEvent;
    ISceneNode *pNode=m_pDevice->getSceneManager()->getSceneNodeFromId(p->getBodyId());
    if (pNode!=NULL) {
      deactivateParticleSystems(pNode);
      hideAnimatedMesh(pNode);
    }
    return true;
  }

  if (pEvent->getType()==EVENT_TANK_STATE_ID) {
    CEventTankState *p=(CEventTankState *)pEvent;
    list<STankNodes *>::Iterator it;
    for (it=m_lTanks.begin(); it!=m_lTanks.end(); it++) {
      STankNodes *pNodes=*it;
      if (pNodes->iNodeId==p->getNodeId()) {
        for (irr::u32 i=0; i<4; i++) {
          f32 f=((f32)p->getAngles()[i])*M_PI/60.0f;
          pNodes->aAxes[i]->setRotation(vector3df(0.0f,0.0f,-f*180/M_PI));
        }
      }

      if (pNodes->pCannon!=NULL) {
        f32 f=p->getCannonAngle()*180.0f/M_PI;
        pNodes->pCannon->setRotation(vector3df(0.0f,0.0f,-f));
      }

      if (pNodes->pTurret!=NULL) {
        f32 f=p->getTurretAngle()*180.0f/M_PI;
        pNodes->pTurret->setRotation(vector3df(0.0f,-f,0.0f));
      }
    }
  }

  if (pEvent->getType()==EVENT_CAR_STATE_ID) {
    CEventCarState *p=(CEventCarState *)pEvent;
    list<SCarNodes *>::Iterator it;
    for (it=m_lCars.begin(); it!=m_lCars.end(); it++) {
      SCarNodes *pCar=*it;
      if (pCar->iNodeId==p->getNodeId()) {
        pCar->pSuspension->setPosition(irr::core::vector3df(0.0f,-1.0f,0.0f)*p->getSuspension());

        core::vector3df v=(p->getLeftWheel()*core::vector3df(0.0f,0.0f,-1.0f));
        pCar->pRearWheels[0]->setRotation(v);

        v=(p->getRightWheel()*core::vector3df(0.0f,0.0f,-1.0f));
        pCar->pRearWheels[1]->setRotation(v);

        if (pCar->pSmoke[0]!=NULL && pCar->pSmoke[1]!=NULL) {
          pCar->pSmoke[0]->setIsActive(p->getFlags()&CEventCarState::eCarFlagBoost);
          pCar->pSmoke[1]->setIsActive(p->getFlags()&CEventCarState::eCarFlagBoost);

          u32 iMin=(u32)(-p->getRpm()*3.0f),
              iMax=(u32)(-p->getRpm()*5.0f);

          if (iMin<25) iMin=25;
          if (iMax<50) iMax=50;

          if (iMin>450) iMin=750;
          if (iMax>500) iMax=750;

          for (u32 i=0; i<2; i++) {
            pCar->pSmoke[i]->getEmitter()->setMinParticlesPerSecond(iMin);
            pCar->pSmoke[i]->getEmitter()->setMaxParticlesPerSecond(iMax);
          }
        }
      }
    }
  }

  return false;
}

bool CCustomEventReceiver::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==EVENT_PLANE_STATE_ID || pEvent->getType()==irr::ode::eIrrOdeEventBodyRemoved ||
         pEvent->getType()==EVENT_TANK_STATE_ID  || pEvent->getType()==EVENT_CAR_STATE_ID;
}

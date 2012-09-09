  #include <CCustomEventReceiver.h>
  #include <CEventVehicleState.h>
  #include <CAdvancedParticleSystemNode.h>
  #include <CRandomForestNode.h>

  #include <irrklang.h>

CCustomEventReceiver::CCustomEventReceiver() {
  m_pDevice=NULL;
  m_pOdeManager=NULL;
  m_bInstalled=false;
  m_pRearLights[0]=NULL;
  m_pRearLights[1]=NULL;
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
    list<CPlaneNodes *>::Iterator it=m_lPlanes.begin();
    CPlaneNodes *p=*it;
    m_lPlanes.erase(it);
    delete p;
  }

  while (m_lTanks.getSize()>0) {
    list<CTankNodes *>::Iterator it=m_lTanks.begin();
    CTankNodes *p=*it;
    m_lTanks.erase(it);
    delete p;
  }

  while (m_lCars.getSize()>0) {
    list<CCarNodes *>::Iterator it=m_lCars.begin();
    CCarNodes *p=*it;
    m_lCars.erase(it);
    delete p;
  }

  while (m_lHelis.getSize()>0) {
    list<CHeliNodes *>::Iterator it=m_lHelis.begin();
    CHeliNodes *p=*it;
    m_lHelis.erase(it);
    delete p;
  }

  m_bInstalled=false;
}

void CCustomEventReceiver::setMembers(irr::IrrlichtDevice *pDevice, irr::ode::CIrrOdeManager *pOdeMgr, irrklang::ISoundEngine *pSndEngine) {
  CCustomEventReceiver::getSharedInstance()->m_pDevice=pDevice;
  CCustomEventReceiver::getSharedInstance()->m_pOdeManager=pOdeMgr;
  CCustomEventReceiver::getSharedInstance()->m_pSndEngine=pSndEngine;

  CCustomEventReceiver::getSharedInstance()->m_pRearLights[0]=pDevice->getVideoDriver()->getTexture("../../data/textures/bl_off.png");
  CCustomEventReceiver::getSharedInstance()->m_pRearLights[1]=pDevice->getVideoDriver()->getTexture("../../data/textures/bl_on.png");
}

CCustomEventReceiver *CCustomEventReceiver::getSharedInstance() {
  static CCustomEventReceiver theReceiver;
  return &theReceiver;
}

void CCustomEventReceiver::addPlane(irr::scene::ISceneNode *pPlane) {
  CCustomEventReceiver::CPlaneNodes *p = new CPlaneNodes(pPlane, m_pSndEngine);
  m_lPlanes.push_back(p);
}

void CCustomEventReceiver::addCar(irr::scene::ISceneNode *pCar) {
  CCustomEventReceiver::CCarNodes *p = new CCarNodes(pCar, m_pSndEngine, m_pRearLights);
  m_lCars.push_back(p);
}

void CCustomEventReceiver::addTank(irr::scene::ISceneNode *pTank) {
  CCustomEventReceiver::CTankNodes *p = new CTankNodes(pTank, m_pSndEngine);
  m_lTanks.push_back(p);
}

void CCustomEventReceiver::addHeli(irr::scene::ISceneNode *pHeli) {
  CCustomEventReceiver::CHeliNodes::CHeliNodes *p = new CHeliNodes(pHeli, m_pSndEngine);
  m_lHelis.push_back(p);
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
    list<CPlaneNodes *>::Iterator it;
    for (it=m_lPlanes.begin(); it!=m_lPlanes.end(); it++) {
      if ((*it)->getNodeId()==p->getNodeId()) {
        (*it)->handlePlaneEvent(p);
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

      core::list<CMissileNodes *>::Iterator it;
      for (it=m_lMissiles.begin(); it!=m_lMissiles.end(); it++) {
        CMissileNodes *pMsl=*it;
        if (pMsl->getNodeId()==p->getBodyId()) {
          pMsl->triggerUpdateSound();
        }
      }
    }
    return true;
  }

  if (pEvent->getType()==EVENT_TANK_STATE_ID) {
    CEventTankState *p=(CEventTankState *)pEvent;
    list<CTankNodes *>::Iterator it;
    for (it=m_lTanks.begin(); it!=m_lTanks.end(); it++) {
      CTankNodes *pNodes=*it;
      if (pNodes->getNodeId()==p->getNodeId()) {
        pNodes->handleTankEvent(p);
      }
    }
  }

  if (pEvent->getType()==EVENT_CAR_STATE_ID) {
    CEventCarState *p=(CEventCarState *)pEvent;
    list<CCarNodes *>::Iterator it;
    for (it=m_lCars.begin(); it!=m_lCars.end(); it++) {
      CCarNodes *pCar=*it;
      if (pCar->getNodeId()==p->getNodeId()) {
        pCar->handleCarEvent(p);
      }
    }
  }

  if (pEvent->getType()==EVENT_FIRE_SND_ID) {
    CEventFireSound *p=(CEventFireSound *)pEvent;
    char s[0xFF]="";

    switch (p->getSound()) {
      case CEventFireSound::eSndCrash    : strcpy(s,"../../data/sound/crash.ogg"  ); break;
      case CEventFireSound::eSndExplode  : strcpy(s,"../../data/sound/explode.ogg"); break;
      case CEventFireSound::eSndFireShell: strcpy(s,"../../data/sound/shot.ogg"   ); break;
      case CEventFireSound::eSndSkid     : strcpy(s,"../../data/sound/skid.ogg"   ); break;
      case CEventFireSound::eSndBell     : strcpy(s,"../../data/sound/bell.ogg"   ); break;
      case CEventFireSound::eSndShift    : strcpy(s,"../../data/sound/shift.ogg"  ); break;
      case CEventFireSound::eSndCreaky   : strcpy(s,"../../data/sound/creaky.ogg" ); break;
    }

    if (s[0]!='\0') {
      irrklang::ISound *pSnd=m_pSndEngine->play3D(s,p->getPosition(),false,true);
      pSnd->setVolume(p->getSound());
      if (p->getSound()==CEventFireSound::eSndExplode) pSnd->setMinDistance(100.0f);
      pSnd->setIsPaused(false);
      pSnd->drop();
    }
  }

  if (pEvent->getType()==EVENT_HELI_STATE_ID) {
    list<CHeliNodes *>::Iterator it;

    CEventHeliState *p=(CEventHeliState *)pEvent;
    for (it=m_lHelis.begin(); it!=m_lHelis.end(); it++) {
      CHeliNodes *pNodes=*it;
      if (p->getNodeId()==pNodes->getNodeId()) {
        pNodes->handleHeliEvent(p);
      }
    }
  }

  if (pEvent->getType()==EVENT_INST_FOREST_ID) {
    printf("**** install forest!\n");
    irr::scene::ISceneManager *smgr=m_pDevice->getSceneManager();
    CRandomForestFactory *pFactory=new CRandomForestFactory(smgr);
    smgr->registerSceneNodeFactory(pFactory);
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventBodyMoved) {
    bool bDone=false;

    irr::ode::CIrrOdeEventBodyMoved *p=(irr::ode::CIrrOdeEventBodyMoved *)pEvent;
    list<CHeliNodes *>::Iterator hit;
    for (hit=m_lHelis.begin(); hit!=m_lHelis.end(); hit++) {
      CHeliNodes *pHeli=*hit;
      if (p->getBodyId()==pHeli->getNodeId()) {
        pHeli->triggerUpdateSound();
        bDone=true;
      }
    }

    if (!bDone) {
      list<CPlaneNodes *>::Iterator pit;
      for (pit=m_lPlanes.begin(); pit!=m_lPlanes.end(); pit++) {
        CPlaneNodes *pPlane=*pit;
        if (p->getBodyId()==pPlane->getNodeId()) {
          pPlane->triggerUpdateSound();
          bDone=true;
        }
      }

      if (!bDone) {
        list<CCarNodes *>::Iterator cit;
        for (cit=m_lCars.begin(); cit!=m_lCars.end(); cit++) {
          CCarNodes *pCar=*cit;
          if (p->getBodyId()==pCar->getNodeId()) {
            pCar->triggerUpdateSound();
            bDone=true;
          }
        }

        if (!bDone) {
          list<CTankNodes *>::Iterator tit;
          for (tit=m_lTanks.begin(); tit!=m_lTanks.end(); tit++) {
            CTankNodes *pTank=*tit;
            if (p->getBodyId()==pTank->getNodeId()) {
              pTank->triggerUpdateSound();
              bDone=true;
            }
          }

          if (!bDone) {
            list<CMissileNodes *>::Iterator mit;
            for (mit=m_lMissiles.begin(); mit!=m_lMissiles.end(); mit++) {
              CMissileNodes *pMsl=*mit;
              if (p->getBodyId()==pMsl->getNodeId()) {
                pMsl->triggerUpdateSound();
                bDone=true;
              }
            }
          }
        }
      }
    }
  }

  if (pEvent->getType()==irr::ode::eIrrOdeEventNodeCloned) {
    ode::CIrrOdeEventNodeCloned *p=(ode::CIrrOdeEventNodeCloned *)pEvent;
    scene::ISceneNode *pNode=m_pDevice->getSceneManager()->getSceneNodeFromId(p->getNewId());
    if (p && !strcmp(pNode->getName(),"missile")) {
      bool b=true;
      core::list<CMissileNodes *>::Iterator it;
      for (it=m_lMissiles.begin(); it!=m_lMissiles.end() && b; it++) if ((*it)->getNodeId()==p->getNewId()) b=false;
      if (b) {
        CMissileNodes *pData=new CMissileNodes(pNode, m_pSndEngine, p);
        m_lMissiles.push_back(pData);
      }
    }
  }

  return false;
}

void CCustomEventReceiver::updateSound(irrklang::ISound *pSound, irr::ode::CIrrOdeBody *pBody) {
  if (pSound==NULL) return;

  core::vector3df irrPos=pBody->getPosition(),
                  irrVel=pBody->getLinearVelocity();

  irrklang::vec3df vPos=irrklang::vec3df(irrPos.X,irrPos.Y,irrPos.Z),
                   vVel=irrklang::vec3df(irrVel.X,irrVel.Y,irrVel.Z);

  pSound->setVelocity(vVel);
  pSound->setPosition(vPos);
}

bool CCustomEventReceiver::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==EVENT_PLANE_STATE_ID || pEvent->getType()==irr::ode::eIrrOdeEventBodyRemoved ||
         pEvent->getType()==EVENT_TANK_STATE_ID  || pEvent->getType()==EVENT_CAR_STATE_ID ||
         pEvent->getType()==EVENT_FIRE_SND_ID || pEvent->getType()==EVENT_HELI_STATE_ID ||
         pEvent->getType()==irr::ode::eIrrOdeEventBodyMoved ||
         pEvent->getType()==irr::ode::eIrrOdeEventNodeCloned ||
         pEvent->getType()==EVENT_INST_FOREST_ID;
}

//Implementation of CCarNodes
void CCustomEventReceiver::CCarNodes::searchCarNodes(irr::scene::ISceneNode *pNode) {
  core::list<scene::ISceneNode *> children=pNode->getChildren();
  core::list<scene::ISceneNode *>::Iterator it;

  if (!strcmp(pNode->getName(),"sc_wheel_rl"       )) m_pRearWheels[0] =pNode;
  if (!strcmp(pNode->getName(),"sc_wheel_rr"       )) m_pRearWheels[1] =pNode;
  if (!strcmp(pNode->getName(),"sc_suspension_rear")) m_pSuspension    =pNode;
  if (!strcmp(pNode->getName(),"steering_wheel"    )) m_pSteering      =pNode;
  if (!strcmp(pNode->getName(),"CarBody"           )) m_pBody          =pNode;
  if (!strcmp(pNode->getName(),"axis_fl"           )) m_pFrontAxes[0]  =pNode;
  if (!strcmp(pNode->getName(),"axis_fr"           )) m_pFrontAxes[1]  =pNode;
  if (!strcmp(pNode->getName(),"steer_l"           )) m_pSteer[0]      =pNode;
  if (!strcmp(pNode->getName(),"steer_r"           )) m_pSteer[1]      =pNode;
  if (!strcmp(pNode->getName(),"wheel_fl"          )) m_pFrontWheels[0]=pNode;
  if (!strcmp(pNode->getName(),"wheel_fr"          )) m_pFrontWheels[1]=pNode;

  if (!strcmp(pNode->getName(),"smoke_1")) m_pSmoke[0]=reinterpret_cast<CAdvancedParticleSystemNode *>(pNode);
  if (!strcmp(pNode->getName(),"smoke_2")) m_pSmoke[1]=reinterpret_cast<CAdvancedParticleSystemNode *>(pNode);

  for (it=children.begin(); it!=children.end(); it++) searchCarNodes(*it);
}

CCustomEventReceiver::CCarNodes::CCarNodes(irr::scene::ISceneNode *pCar, irrklang::ISoundEngine *pSndEngine, irr::video::ITexture *pRearLights[2]) {
  m_pRearLights[0] = pRearLights[0];
  m_pRearLights[1] = pRearLights[1];

  m_pSndEngine = pSndEngine;

  m_vOldSpeed=core::vector3df(0.0f,0.0f,0.0f);
  m_fSteerAngle=0.0f;

  m_iNodeId=pCar->getID();

  m_pEngine = m_pSndEngine->play3D("../../data/sound/car.ogg"    ,irrklang::vec3df(0.0f,0.0f,0.0f),true,true);
  m_pWind   = m_pSndEngine->play3D("../../data/sound/wind.ogg"   ,irrklang::vec3df(0.0f,0.0f,0.0f),true,true);
  m_pWheels = m_pSndEngine->play3D("../../data/sound/rolling.ogg",irrklang::vec3df(0.0f,0.0f,0.0f),true,true);

  if (m_pEngine) m_pEngine->setMinDistance(25.0f); else printf("\n\t\t**** oops 1\n\n");
  if (m_pWind  ) m_pWind  ->setMinDistance( 0.0f); else printf("\n\t\t**** oops 2\n\n");
  if (m_pWheels) m_pWheels->setMinDistance( 0.0f); else printf("\n\t\t**** oops 3\n\n");

  m_pCar=reinterpret_cast<ode::CIrrOdeBody *>(pCar);
  searchCarNodes(m_pCar);
}

CCustomEventReceiver::CCarNodes::~CCarNodes() {
  if (m_pEngine) m_pEngine->drop();
  if (m_pWind  ) m_pWind  ->drop();
  if (m_pWheels) m_pWheels->drop();
}

void CCustomEventReceiver::CCarNodes::triggerUpdateSound() {
  updateSound(m_pEngine, m_pCar);

  irr::f32 fVol = m_pCar->getLinearVelocity().getLength();
  if (fVol > 100.0f) fVol = 100.0f;
  fVol /= 100.0f;
  if (fVol < 0.0f) fVol = -fVol;

  m_pWind->setVolume(fVol / 4.0f);
  updateSound(m_pWind, m_pCar);
}

void CCustomEventReceiver::CCarNodes::handleCarEvent(CEventCarState *p) {
  if (m_pEngine!=NULL) {
    core::vector3df irrPos=m_pCar->getPosition(),
                    irrVel=m_pCar->getLinearVelocity();

    irrklang::vec3df vPos=irrklang::vec3df(irrPos.X,irrPos.Y,irrPos.Z),
                     vVel=irrklang::vec3df(irrVel.X,irrVel.Y,irrVel.Z);

    m_pEngine->setVelocity(vVel);
    m_pEngine->setPosition(vPos);
    m_pEngine->setPlaybackSpeed(p->getEngineSound());

    m_pEngine->setIsPaused(false);
    m_pWind  ->setIsPaused(false);
    m_pWheels->setIsPaused(false);
  }
  m_pSuspension->setPosition(irr::core::vector3df(0.0f,-1.0f,0.0f)*p->getSuspension());

  irr::f32 fSteer=3.0f * p->getSteer();
  if (m_fSteerAngle<fSteer) { m_fSteerAngle+=10.0f; if (m_fSteerAngle>fSteer) m_fSteerAngle=fSteer; }
  if (m_fSteerAngle>fSteer) { m_fSteerAngle-=10.0f; if (m_fSteerAngle<fSteer) m_fSteerAngle=fSteer; }

  m_pSteering->setRotation(irr::core::vector3df(m_fSteerAngle/2.0f,0.0f,0.0f));

  core::vector3df v=(p->getLeftWheel()*core::vector3df(0.0f,0.0f,-1.0f));
  m_pRearWheels[0]->setRotation(v);

  irr::f32 fAngle = -m_fSteerAngle / 3.0f;
  for (irr::u32 i = 0; i < 2; i++) {
    irr::core::vector3df v = m_pFrontAxes[i]->getPosition();

    m_pFrontWheels[i]->setRotation(irr::core::vector3df(0.0f, 0.0f, p->getWheelRot(i)));
    m_pFrontAxes  [i]->setPosition(irr::core::vector3df(v.X,p->getWheelPos(i),v.Z));
    m_pSteer[i]->setRotation(irr::core::vector3df(0.0f, fAngle, 0.0f));
  }

  v=(p->getRightWheel()*core::vector3df(0.0f,0.0f,-1.0f));
  m_pRearWheels[1]->setRotation(v);

  if (m_pSmoke[0]!=NULL && m_pSmoke[1]!=NULL) {
    m_pSmoke[0]->setIsActive(p->getFlags()&(CEventCarState::eCarFlagSmoke | CEventCarState::eCarFlagBoost));
    m_pSmoke[1]->setIsActive(p->getFlags()&(CEventCarState::eCarFlagSmoke | CEventCarState::eCarFlagBoost));

    u32 iMin=(u32)(-p->getRpm()*3.0f),
        iMax=(u32)(-p->getRpm()*5.0f);

    if (iMin<250) iMin=250;
    if (iMax<350) iMax=350;

    if (iMin>750) iMin=750;
    if (iMax>750) iMax=750;

    for (u32 i=0; i<2; i++) {
      m_pSmoke[i]->getEmitter()->setMinParticlesPerSecond(iMin);
      m_pSmoke[i]->getEmitter()->setMaxParticlesPerSecond(iMax);
    }
  }

  if (m_pRearLights[0] && m_pRearLights[1] && m_pBody) {
    bool bBrk=p->getFlags()&CEventCarState::eCarFlagBrake;
    m_pBody->getMaterial(4).setTexture(0,bBrk?m_pRearLights[1]:m_pRearLights[0]);
  }

  irr::f32 fVol = 0.0f;
  if (p->getFlags() & CEventCarState::eCarFlagTouch) {
    fVol = m_pCar->getLinearVelocity().getLength() / 100.0f;
    if (fVol < 0.0f) fVol = -fVol;
    if (fVol > 1.0f) fVol =  1.0f;
  }

  m_pWheels->setVolume(0.6f * fVol);
  m_pWheels->setPlaybackSpeed(1.0f+(fVol-0.5f));
  updateSound(m_pWheels, m_pCar);
}

//Implementation of CPlaneNodes
void CCustomEventReceiver::CPlaneNodes::searchPlaneNodes(irr::scene::ISceneNode *pNode) {
  core::list<scene::ISceneNode *> children=pNode->getChildren();
  core::list<scene::ISceneNode *>::Iterator it;

  if (!strcmp(pNode->getName(),"yaw"   ) || !strcmp(pNode->getName(),"yaw2"  )) m_aYaw  .push_back(pNode);
  if (!strcmp(pNode->getName(),"pitch1") || !strcmp(pNode->getName(),"pitch2")) m_aPitch.push_back(pNode);
  if (!strcmp(pNode->getName(),"roll1" ) || !strcmp(pNode->getName(),"roll2" )) m_aRoll .push_back(pNode);

  for (it=children.begin(); it!=children.end(); it++) {
    searchPlaneNodes(*it);
  }
}

CCustomEventReceiver::CPlaneNodes::CPlaneNodes(irr::scene::ISceneNode *pPlane, irrklang::ISoundEngine *pSndEngine) {
  m_pSndEngine = pSndEngine;

  m_iNodeId=pPlane->getID();
  m_pEngine=m_pSndEngine->play3D("../../data/sound/plane.ogg",irrklang::vec3df(0.0f,0.0f,0.0f),true,true);
  if (m_pEngine) m_pEngine->setMinDistance(100.0f);
  m_pWind = m_pSndEngine->play3D("../../data/sound/wind.ogg",irrklang::vec3df(0.0f,0.0f,0.0f),true,true);
  if (m_pWind) m_pWind->setMinDistance(0.0f); else printf("\n\t\t**** oops\n\n");
  m_pPlane=reinterpret_cast<ode::CIrrOdeBody *>(pPlane);
  searchPlaneNodes(pPlane);
}

CCustomEventReceiver::CPlaneNodes::~CPlaneNodes() {
  if (m_pEngine) m_pEngine->drop();
  if (m_pWind  ) m_pWind  ->drop();
}

void CCustomEventReceiver::CPlaneNodes::handlePlaneEvent(CEventPlaneState *p) {
  u32 i;
  f32 fYaw=p->getYaw(),fPitch=p->getPitch(),fRoll=p->getRoll();

  for(i=0; i<m_aRoll.size(); i++)
    m_aRoll[i]->setRotation(vector3df(i==0?-15.0f*fRoll:15.0f*fRoll,i==0?4:-4,0));

  for(i=0; i<m_aPitch.size(); i++)
    m_aPitch[i]->setRotation(vector3df(fPitch*-15.0f,i==0?-8:8,0));

  if (m_aYaw.size()>0) m_aYaw[0]->setRotation(vector3df(0,90+10.0f*fYaw, 0));
  if (m_aYaw.size()>1) m_aYaw[1]->setRotation(vector3df(-15.0f*fYaw,13,90));

  if (m_pEngine!=NULL) {
    core::vector3df irrPos=m_pPlane->getPosition(),
                    irrVel=m_pPlane->getLinearVelocity();

    irrklang::vec3df vPos=irrklang::vec3df(irrPos.X,irrPos.Y,irrPos.Z),
                     vVel=irrklang::vec3df(irrVel.X,irrVel.Y,irrVel.Z);

    f32 fPitch=p->getSound();
    if (fPitch<0.0f) fPitch=-fPitch;

    m_pEngine->setVelocity(vVel);
    m_pEngine->setPosition(vPos);
    m_pEngine->setPlaybackSpeed(0.75f+0.5*fPitch);
    m_pEngine->setIsPaused(false);
    m_pWind  ->setIsPaused(false);
  }
}

void CCustomEventReceiver::CPlaneNodes::triggerUpdateSound() {
  updateSound(m_pEngine,m_pPlane);
  irr::f32 fVol = m_pPlane->getLinearVelocity().getLength();
  if (fVol > 100.0f) fVol = 100.0f;
  fVol /= 100.0f;
  if (fVol < 0.0f) fVol = -fVol;

  m_pWind->setVolume(fVol / 8.0f);
  updateSound(m_pWind, m_pPlane);
}

//Implementation of CHeliNodes
CCustomEventReceiver::CHeliNodes::CHeliNodes(irr::scene::ISceneNode *pHeli, irrklang::ISoundEngine *pSndEngine) {
  m_pSndEngine = pSndEngine;

  m_iNodeId=pHeli->getID();
  m_pHeli=reinterpret_cast<ode::CIrrOdeBody *>(pHeli);
  m_pEngine=m_pSndEngine->play3D("../../data/sound/heli.ogg",irrklang::vec3df(0.0f,0.0f,0.0f),true,true);

  if (m_pEngine) {
    m_pEngine->setMaxDistance(200.0f);
    m_pEngine->setVolume(0.5f);
  }
}

CCustomEventReceiver::CHeliNodes::~CHeliNodes() {
  if (m_pEngine) m_pEngine->drop();
}

void CCustomEventReceiver::CHeliNodes::handleHeliEvent(CEventHeliState *p) {
  core::vector3df irrPos=m_pHeli->getPosition(),
                  irrVel=m_pHeli->getLinearVelocity();

  irrklang::vec3df vPos=irrklang::vec3df(irrPos.X,irrPos.Y,irrPos.Z),
                   vVel=irrklang::vec3df(irrVel.X,irrVel.Y,irrVel.Z);

  m_pEngine->setVelocity(vVel);
  m_pEngine->setPosition(vPos);
  m_pEngine->setPlaybackSpeed(p->getSound());
  m_pEngine->setIsPaused(false);
}

void CCustomEventReceiver::CHeliNodes::triggerUpdateSound() {
  updateSound(m_pEngine,m_pHeli);
}

//Implementation of CTankNodes
void CCustomEventReceiver::CTankNodes::searchTankNodes(irr::scene::ISceneNode *pNode) {
  core::list<scene::ISceneNode *> children=pNode->getChildren();
  core::list<scene::ISceneNode *>::Iterator it;

  if (!strcmp(pNode->getName(),"cannonAxis")) { m_pCannon=pNode; }
  if (!strcmp(pNode->getName(),"turretAxis")) { m_pTurret=pNode; }

  irr::core::stringc aNames[]={"tankAxisRR","tankAxisFR","tankAxisRL","tankAxisFL"};

  for (irr::u32 i=0; i<4; i++)
    if (!strcmp(pNode->getName(),aNames[i].c_str())) {
      m_aAxes.push_back(pNode);
    }

  for (it=children.begin(); it!=children.end(); it++) {
    searchTankNodes(*it);
  }
}

CCustomEventReceiver::CTankNodes::CTankNodes(irr::scene::ISceneNode *pTank, irrklang::ISoundEngine *pSndEngine) {
  m_pSndEngine = pSndEngine;
  m_pCannon=NULL;
  m_iNodeId=pTank->getID();
  m_pTank=reinterpret_cast<ode::CIrrOdeBody *>(pTank);
  m_pEngine=m_pSndEngine->play3D("../../data/sound/tank.ogg",irrklang::vec3df(0.0f,0.0f,0.0f),true,true);
  if (m_pEngine) {
    m_pEngine->setVolume(0.5f);
    m_pEngine->setMinDistance(100.0f);
  }

  searchTankNodes(pTank);
}

CCustomEventReceiver::CTankNodes::~CTankNodes() {
  if (m_pEngine) m_pEngine->drop();
}

void CCustomEventReceiver::CTankNodes::handleTankEvent(CEventTankState *p) {
  for (irr::u32 i=0; i<4; i++) {
    f32 f=((f32)p->getAngles()[i])*M_PI/60.0f;
    m_aAxes[i]->setRotation(vector3df(0.0f,0.0f,-f*180/M_PI));
  }

  if (m_pCannon!=NULL) {
    f32 f=p->getCannonAngle()*180.0f/M_PI;
    m_pCannon->setRotation(vector3df(0.0f,0.0f,-f));
  }

  if (m_pTurret!=NULL) {
    f32 f=p->getTurretAngle()*180.0f/M_PI;
    m_pTurret->setRotation(vector3df(0.0f,-f,0.0f));
  }

  if (m_pEngine) {
    m_pEngine->setIsPaused(false);
    m_pEngine->setVelocity(m_pTank->getLinearVelocity());
    m_pEngine->setPosition(m_pTank->getAbsolutePosition());
    m_pEngine->setPlaybackSpeed(p->getSound());
  }
}

void CCustomEventReceiver::CTankNodes::triggerUpdateSound() {
  updateSound(m_pEngine,m_pTank);
}

//Implementation of MissileNodes
CCustomEventReceiver::CMissileNodes::CMissileNodes(irr::scene::ISceneNode *pMissile, irrklang::ISoundEngine *pSndEngine, irr::ode::CIrrOdeEventNodeCloned *p) {
  m_pSndEngine = pSndEngine;
  core::vector3df vPos=pMissile->getPosition();
  m_iNodeId=p->getNewId();
  m_pNode=reinterpret_cast<ode::CIrrOdeBody *>(pMissile);
  m_pEngine=m_pSndEngine->play3D("../../data/sound/missile.ogg",irrklang::vec3df(vPos.X,vPos.Y,vPos.Z),true,true);
  m_pEngine->setIsPaused(false);
}

CCustomEventReceiver::CMissileNodes::~CMissileNodes() {
  if (m_pEngine!=NULL) {
    m_pEngine->stop();
    m_pEngine->drop();
  }
}

void CCustomEventReceiver::CMissileNodes::handleMissileEvent(irr::ode::IIrrOdeEvent *p) {
}

void CCustomEventReceiver::CMissileNodes::triggerUpdateSound() {
  updateSound(m_pEngine,m_pNode);
}

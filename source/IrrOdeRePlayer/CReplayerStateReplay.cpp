  #include <CReplayerStateReplay.h>

void CReplayerStateReplay::updateBodyList() {
  stringw s;
  for (u32 i=0; i<m_aBodies.size(); i++) {
    ode::CIrrOdeBody *p=m_aBodies[i];
    if (p!=NULL) {
      s+=p->getID();
      s+=" -- (";
      s+=p->getName();
      s+=")";
      s+="\n";
    }
  }
  m_pLblBodies->setText(s.c_str());
}

void CReplayerStateReplay::removeNode(ISceneNode *pNode) {
  if (pNode==NULL) return;

  list<ISceneNode *> children=pNode->getChildren();
  list<ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) removeNode(*it);

  if (pNode->getType()==ode::IRR_ODE_BODY_ID) {
    ode::CIrrOdeBody *pBody=(ode::CIrrOdeBody *)pNode;
    for (u32 i=0; i<m_aBodies.size(); i++) if (m_aBodies[i]==pBody) {
      m_aBodies.erase(i); break;
      updateBodyList();
    }
  }
}

CReplayerStateReplay::CReplayerStateReplay(irr::IrrlichtDevice *pDevice, const c8 *sReplay) {
  m_pOdeManager=ode::CIrrOdeManager::getSharedInstance();
  m_pDevice=pDevice;
  m_pSmgr=pDevice->getSceneManager();
  m_pLblBodies=NULL;
  m_bSceneLoaded=false;
  strcpy(m_sReplay,sReplay);
  m_iRet=0;
}

void CReplayerStateReplay::activate() {
  m_pPlayer=new irr::ode::CIrrOdeRePlayer(m_pDevice,m_sReplay);
  IGUIEnvironment *guienv=m_pDevice->getGUIEnvironment();
  m_pLblBodies=guienv->addStaticText(L"Hello World",rect<s32>(5,5,200,300),true,true,0,-1,true);
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
  ICameraSceneNode *pCam=m_pSmgr->addCameraSceneNodeFPS();
  vector3df v=pCam->getAbsolutePosition();
  m_pSmgr->setActiveCamera(pCam);
}

void CReplayerStateReplay::deactivate() {
  for (u32 i=0; i<m_aBodies.size(); i++) m_aBodies[i]->drop();
  m_aBodies.clear();
  m_pSmgr->clear();
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

u32 CReplayerStateReplay::update() {
  m_pPlayer->update();
  return m_iRet;
}

bool CReplayerStateReplay::onEvent(ode::IIrrOdeEvent *pEvent) {
  if (!m_bSceneLoaded) {
    if (pEvent->getType()==ode::eIrrOdeEventLoadScene) {
      core::list<ode::CIrrOdeSceneNode *> lBodies=m_pOdeManager->getIrrOdeNodes();
      core::list<ode::CIrrOdeSceneNode *>::Iterator it;

      for (it=lBodies.begin(); it!=lBodies.end(); it++) {
        ode::CIrrOdeSceneNode *pNode=*it;
        if (pNode->getType()==ode::IRR_ODE_BODY_ID) {
          ode::CIrrOdeBody *pBody=(ode::CIrrOdeBody *)pNode;
          if (pBody->getParentBody()==NULL && pBody->getID()!=-1) {
            pBody->grab();
            m_aBodies.push_back(pBody);
          }
        }
      }
      updateBodyList();
      m_bSceneLoaded=true;
      return true;
    }
  }
  else {
    if (pEvent->getType()==ode::eIrrOdeEventNodeCloned) {
      ode::CIrrOdeEventNodeCloned *p=(ode::CIrrOdeEventNodeCloned *)pEvent;
      ISceneNode *pNode=m_pSmgr->getSceneNodeFromId(p->getNewId());
      if (pNode->getType()==ode::IRR_ODE_BODY_ID) {
        ode::CIrrOdeBody *pBody=(ode::CIrrOdeBody *)pNode;
        if (pBody->getParentBody()==NULL) {
          pBody->grab();
          m_aBodies.push_back(pBody);
        }
      }
      updateBodyList();
      return true;
    }

    if (pEvent->getType()==ode::eIrrOdeEventNodeRemoved) {
      ode::CIrrOdeEventNodeRemoved *p=(ode::CIrrOdeEventNodeRemoved *)pEvent;
      ISceneNode *pNode=m_pSmgr->getSceneNodeFromId(p->getRemovedNodeId());
      if (pNode!=NULL) removeNode(pNode);
      updateBodyList();
      return true;
    }
  }

  return false;
}

bool CReplayerStateReplay::handlesEvent(ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==ode::eIrrOdeEventNodeCloned  ||
         pEvent->getType()==ode::eIrrOdeEventNodeRemoved ||
         pEvent->getType()==ode::eIrrOdeEventLoadScene;
}

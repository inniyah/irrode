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
  m_pFont=m_pDevice->getGUIEnvironment()->getFont("../../data/font2.xml");
}

void CReplayerStateReplay::activate() {
  m_pPlayer=new irr::ode::CIrrOdeRePlayer(m_pDevice,m_sReplay);
  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_pLblBodies=m_pGuiEnv->addStaticText(L"Hello World",rect<s32>(5,5,200,300),true,true,0,-1,true);
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
  m_pCam=m_pSmgr->addCameraSceneNode();
  m_pFreeCam=m_pSmgr->addCameraSceneNodeFPS();
  m_pSmgr->setActiveCamera(m_pFreeCam);

  m_pLblPaused=m_pGuiEnv->addStaticText(L"Paused",rect<s32>(422,5,602,35),true,true,0,-1,true);
  m_pLblPaused->setOverrideFont(m_pFont);
  m_pLblPaused->setOverrideColor(SColor(255,255,118,70));
  m_pLblPaused->setTextAlignment(gui::EGUIA_CENTER,gui::EGUIA_CENTER);
  m_pLblPaused->setVisible(false);

  m_pLblFinished=m_pGuiEnv->addStaticText(L"Finished",rect<s32>(422,369,602,399),true,true,0,-1,true);
  m_pLblFinished->setOverrideFont(m_pFont);
  m_pLblFinished->setOverrideColor(SColor(255,255,118,70));
  m_pLblFinished->setTextAlignment(gui::EGUIA_CENTER,gui::EGUIA_CENTER);
  m_pLblFinished->setVisible(false);

  m_pDevice->setEventReceiver(this);
  m_eCamMode=eCamFree;
  m_pCrsCtrl=m_pDevice->getCursorControl();
  m_pCrsCtrl->setVisible(false);
  m_vCamTarget=core::vector3df(1.0f,0.0f,0.0f);
  m_vCamRotation=core::vector3df(0.0f,0.0f,0.0f);
  m_fCamDist=15.0f;
  m_fCamHeight=0.0f;
  m_pFocusedNode=NULL;
}

void CReplayerStateReplay::deactivate() {
  for (u32 i=0; i<m_aBodies.size(); i++) m_aBodies[i]->drop();
  m_aBodies.clear();
  m_pSmgr->clear();
  m_pGuiEnv->clear();
  ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
  m_pCrsCtrl->setVisible(true);
}

u32 CReplayerStateReplay::update() {
  if (!m_pLblFinished->isVisible() && m_pPlayer->isFinished())
    m_pLblFinished->setVisible(true);

  m_pPlayer->update();
  return m_iRet;
}

bool CReplayerStateReplay::onEvent(ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==ode::eIrrOdeEventStep) {
    position2df pos=m_pCrsCtrl->getRelativePosition();
    m_pCrsCtrl->setPosition(position2df(0.5f,0.5f));
    switch (m_eCamMode) {
      case eCamFree:
        break;

      case eCamFollow:
        if (m_pFocusedNode!=NULL) {
          m_pCam->setTarget(m_pFocusedNode->getAbsolutePosition());

          m_vCamRotation.Y+=15.0f*(pos.X-0.5f);
          m_fCamHeight+=0.5f*(pos.Y-0.5f);

          if (m_fCamHeight> 1.0f) m_fCamHeight= 1.0f;
          if (m_fCamHeight<-1.0f) m_fCamHeight=-1.0f;

          vector3df v=vector3df(1.0f,0.0f,0.0f);
          v.rotateXZBy(m_vCamRotation.Y);
          v.Y=m_fCamHeight;
          m_pCam->setPosition(m_pFocusedNode->getAbsolutePosition()-m_fCamDist*v);
        }
        break;
    }
  }

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
         pEvent->getType()==ode::eIrrOdeEventLoadScene   ||
         pEvent->getType()==ode::eIrrOdeEventStep;
}

bool CReplayerStateReplay::OnEvent(const SEvent &event) {
  bool bRet=false;

  if (event.EventType==EET_KEY_INPUT_EVENT) {
    if (!event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case KEY_TAB:
          if (m_eCamMode==eCamFree) {
            if (m_pFocusedNode==NULL && m_aBodies.size()>0) {
              if (m_iFocusedNode>=m_aBodies.size()) m_iFocusedNode=0;
              m_pFocusedNode=m_aBodies[m_iFocusedNode];
              m_pFocusedNode->grab();
            }

            if (m_pFocusedNode!=NULL) {
              m_eCamMode=eCamFollow;
              m_pSmgr->setActiveCamera(m_pCam);
            }
          }
          else {
            m_pFreeCam->setPosition(m_pCam->getPosition());
            m_pFreeCam->setTarget(m_pCam->getTarget());
            m_pSmgr->setActiveCamera(m_pFreeCam);

            if (m_pFocusedNode!=NULL) {
              m_pFocusedNode->drop();
              m_pFocusedNode=NULL;
            }
            m_eCamMode=eCamFree;
          }
          bRet=true;
          break;

        case KEY_RIGHT:
          if (m_pFocusedNode!=NULL) {
            m_pFocusedNode->drop();
            m_pFocusedNode=NULL;
          }
          m_iFocusedNode++;
          if (m_iFocusedNode>=m_aBodies.size()) m_iFocusedNode=0;
          if (m_aBodies.size()>0) {
            m_pFocusedNode=m_aBodies[m_iFocusedNode];
            m_pFocusedNode->grab();
          }
          break;

        case KEY_LEFT:
          if (m_pFocusedNode!=NULL) {
            m_pFocusedNode->drop();
            m_pFocusedNode=NULL;
          }
          if (m_iFocusedNode>0) m_iFocusedNode--; else m_iFocusedNode=m_aBodies.size()-1;
          if (m_aBodies.size()>0) {
            m_pFocusedNode=m_aBodies[m_iFocusedNode];
            m_pFocusedNode->grab();
          }
          break;

        case KEY_SPACE:
          if (m_pPlayer->isPaused()) {
            m_pPlayer->setIsPaused(false);
            m_pLblPaused->setVisible(false);
          }
          else {
            m_pPlayer->setIsPaused(true);
            m_pLblPaused->setVisible(true);
          }
          break;

        default: break;
      }
    }
  }

  if (event.EventType==EET_MOUSE_INPUT_EVENT)
    if (event.MouseInput.Event==EMIE_MOUSE_WHEEL)
      m_fCamDist+=event.MouseInput.Wheel;

  return bRet;
}

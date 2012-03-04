  #include <CReplayerStateReplay.h>

void CReplayerStateReplay::updateBodyList() {
  irr::core::stringw s;
  for (irr::u32 i=0; i<m_aBodies.size(); i++) {
    irr::ode::CIrrOdeBody *p=m_aBodies[i];
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

void CReplayerStateReplay::removeNode(irr::scene::ISceneNode *pNode) {
  if (pNode==NULL) return;

  irr::core::list<irr::scene::ISceneNode *> children=pNode->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) removeNode(*it);

  if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
    irr::ode::CIrrOdeBody *pBody=(irr::ode::CIrrOdeBody *)pNode;
    for (irr::u32 i=0; i<m_aBodies.size(); i++) if (m_aBodies[i]==pBody) {
      m_aBodies.erase(i); break;
      updateBodyList();
    }
  }
}

CReplayerStateReplay::CReplayerStateReplay(irr::IrrlichtDevice *pDevice, const irr::c8 *sReplay) {
  m_pOdeManager=irr::ode::CIrrOdeManager::getSharedInstance();
  m_pDevice=pDevice;
  m_pSmgr=pDevice->getSceneManager();
  m_pLblBodies=NULL;
  m_bSceneLoaded=false;
  strcpy(m_sReplay,sReplay);
  m_iRet=0;
  m_pFont=m_pDevice->getGUIEnvironment()->getFont("../../data/font2.xml");
  m_iDirection = 0;
  m_iPos = 0;
}

void CReplayerStateReplay::activate() {
  m_pPlayer=new irr::ode::CIrrOdeRePlayer(m_pDevice,m_sReplay);
  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_pLblBodies=m_pGuiEnv->addStaticText(L"Hello World",irr::core::rect<irr::s32>(5,5,200,300),true,true,0,-1,true);
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);
  m_pCam=m_pSmgr->addCameraSceneNode();
  m_pFreeCam=m_pSmgr->addCameraSceneNodeFPS();
  m_pSmgr->setActiveCamera(m_pFreeCam);

  m_pLblPaused=m_pGuiEnv->addStaticText(L"Paused",irr::core::rect<irr::s32>(422,5,602,35),true,true,0,-1,true);
  m_pLblPaused->setOverrideFont(m_pFont);
  m_pLblPaused->setOverrideColor(irr::video::SColor(255,255,118,70));
  m_pLblPaused->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);
  m_pLblPaused->setVisible(false);

  m_pLblFinished=m_pGuiEnv->addStaticText(L"Finished",irr::core::rect<irr::s32>(422,369,602,399),true,true,0,-1,true);
  m_pLblFinished->setOverrideFont(m_pFont);
  m_pLblFinished->setOverrideColor(irr::video::SColor(255,255,118,70));
  m_pLblFinished->setTextAlignment(irr::gui::EGUIA_CENTER,irr::gui::EGUIA_CENTER);
  m_pLblFinished->setVisible(false);

  m_pDevice->setEventReceiver(this);
  m_eCamMode=eCamFree;
  m_pCrsCtrl=m_pDevice->getCursorControl();
  m_pCrsCtrl->setVisible(false);
  m_vCamTarget=irr::core::vector3df(1.0f,0.0f,0.0f);
  m_vCamRotation=irr::core::vector3df(0.0f,0.0f,0.0f);
  m_fCamDist=15.0f;
  m_fCamHeight=0.0f;
  m_pFocusedNode=NULL;
}

void CReplayerStateReplay::deactivate() {
  for (irr::u32 i=0; i<m_aBodies.size(); i++) m_aBodies[i]->drop();
  m_aBodies.clear();
  m_pSmgr->clear();
  m_pGuiEnv->clear();
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
  m_pCrsCtrl->setVisible(true);
}

irr::u32 CReplayerStateReplay::update() {
  if (!m_pLblFinished->isVisible() && m_pPlayer->isFinished())
    m_pLblFinished->setVisible(true);

  m_pPlayer->update();
  return m_iRet;
}

bool CReplayerStateReplay::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==irr::ode::eIrrOdeEventStep) {
    irr::core::position2df pos=m_pCrsCtrl->getRelativePosition();
    m_pCrsCtrl->setPosition(irr::core::position2df(0.5f,0.5f));
    switch (m_eCamMode) {
      case eCamFree:
        break;

      case eCamFollow:
        if (m_pFocusedNode!=NULL) {
          irr::core::vector3df vDir, vCpo = irr::core::vector3df(0.0f, m_iPos==0?1.1f:1.35f, m_iPos==0?-0.6f:0.0f);

          if (m_iPos==0) {
            switch (m_iDirection) {
              case 0: vDir = irr::core::vector3df( 0.0f,0.0f, 1.0f); break;
              case 1: vDir = irr::core::vector3df( 1.0f,0.0f, 0.0f); break;
              case 2: vDir = irr::core::vector3df( 0.0f,0.0f,-1.0f); break;
              case 3: vDir = irr::core::vector3df(-1.0f,0.0f, 0.0f); break;
            }
          }
          else {
            switch (m_iDirection) {
              case 0: vDir = irr::core::vector3df(-5.0f, 1.35f, 0.0f); break;
              case 1: vDir = irr::core::vector3df( 0.0f, 1.35f,-5.0f); break;
              case 2: vDir = irr::core::vector3df( 5.0f, 1.35f, 0.0f); break;
              case 3: vDir = irr::core::vector3df( 0.0f, 1.35f, 5.0f); break;
            }
          }

          irr::core::vector3df vPos = m_pFocusedNode->getPosition(),
                               vRot = m_pFocusedNode->getRotation(),
                               vTgt = vRot.rotationToDirection(vDir);

          irr::core::vector3df pos=vRot.rotationToDirection(vCpo),
                               up =vRot.rotationToDirection(irr::core::vector3df(0,0.1,0)),
                               tgt=vRot.rotationToDirection(irr::core::vector3df(vTgt.X,1.0f,vTgt.Y));

          m_pCam->setPosition(vPos+pos);
          m_pCam->setTarget(vPos+pos-vTgt);
          m_pCam->setUpVector(up);
        }
        break;
    }
  }

  if (!m_bSceneLoaded) {
    if (pEvent->getType()==irr::ode::eIrrOdeEventLoadScene) {
      irr::core::list<irr::ode::CIrrOdeSceneNode *> lBodies=m_pOdeManager->getIrrOdeNodes();
      irr::core::list<irr::ode::CIrrOdeSceneNode *>::Iterator it;

      for (it=lBodies.begin(); it!=lBodies.end(); it++) {
        irr::ode::CIrrOdeSceneNode *pNode=*it;
        if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
          irr::ode::CIrrOdeBody *pBody=(irr::ode::CIrrOdeBody *)pNode;
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
    if (pEvent->getType()==irr::ode::eIrrOdeEventNodeCloned) {
      irr::ode::CIrrOdeEventNodeCloned *p=(irr::ode::CIrrOdeEventNodeCloned *)pEvent;
      irr::scene::ISceneNode *pNode=m_pSmgr->getSceneNodeFromId(p->getNewId());
      if (pNode->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *pBody=(irr::ode::CIrrOdeBody *)pNode;
        if (pBody->getParentBody()==NULL) {
          pBody->grab();
          m_aBodies.push_back(pBody);
        }
      }
      updateBodyList();
      return true;
    }

    if (pEvent->getType()==irr::ode::eIrrOdeEventNodeRemoved) {
      irr::ode::CIrrOdeEventNodeRemoved *p=(irr::ode::CIrrOdeEventNodeRemoved *)pEvent;
      irr::scene::ISceneNode *pNode=m_pSmgr->getSceneNodeFromId(p->getRemovedNodeId());
      if (pNode!=NULL) removeNode(pNode);
      updateBodyList();
      return true;
    }
  }

  return false;
}

bool CReplayerStateReplay::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType()==irr::ode::eIrrOdeEventNodeCloned  ||
         pEvent->getType()==irr::ode::eIrrOdeEventNodeRemoved ||
         pEvent->getType()==irr::ode::eIrrOdeEventLoadScene   ||
         pEvent->getType()==irr::ode::eIrrOdeEventStep;
}

bool CReplayerStateReplay::OnEvent(const irr::SEvent &event) {
  bool bRet=false;

  if (event.EventType==irr::EET_KEY_INPUT_EVENT) {
    if (!event.KeyInput.PressedDown) {
      switch (event.KeyInput.Key) {
        case irr::KEY_RETURN:
          m_iDirection++;
          if (m_iDirection > 3) m_iDirection = 0;
          break;

        case irr::KEY_BACK:
          m_iPos = m_iPos==0?1:0;
          break;

        case irr::KEY_TAB:
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

        case irr::KEY_RIGHT:
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

        case irr::KEY_LEFT:
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

        case irr::KEY_SPACE:
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

  if (event.EventType==irr::EET_MOUSE_INPUT_EVENT)
    if (event.MouseInput.Event==irr::EMIE_MOUSE_WHEEL) {
      printf("==> %i\n",(int)irr::ode::CIrrOdeManager::getSharedInstance());
      m_fCamDist+=0.25f*event.MouseInput.Wheel;
    }

  return bRet;
}

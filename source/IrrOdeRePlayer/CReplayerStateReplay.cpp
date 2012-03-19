  #include <CReplayerStateReplay.h>

void CReplayerStateReplay::removeNode(irr::scene::ISceneNode *pNode) {
  if (pNode==NULL) return;

  irr::core::list<irr::scene::ISceneNode *> children=pNode->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) removeNode(*it);
}

CReplayerStateReplay::CReplayerStateReplay(irr::IrrlichtDevice *pDevice, const irr::c8 *sReplay, IPlugin *pPlugin) {
  m_pOdeManager=irr::ode::CIrrOdeManager::getSharedInstance();
  m_pDevice=pDevice;
  m_pSmgr=pDevice->getSceneManager();
  m_bSceneLoaded=false;
  strcpy(m_sReplay,sReplay);
  m_iRet=0;
  m_pFont=m_pDevice->getGUIEnvironment()->getFont("../../data/font2.xml");
  m_iDirection = 0;
  m_iPos = 0;

  m_pPlugin = pPlugin;
  m_bPluginHandlesCamera = pPlugin->pluginHandleCamera();

  m_iThisStep = 0;

  m_bStepTaken = false;
}

void CReplayerStateReplay::activate() {
  m_pPlayer=new irr::ode::CIrrOdeRePlayer(m_pDevice,m_sReplay);
  m_pGuiEnv=m_pDevice->getGUIEnvironment();
  m_pTab = m_pGuiEnv->addTab(irr::core::rect<irr::s32>(0,0,300,500));
  m_pLblStep = m_pGuiEnv->addStaticText(L"Step", irr::core::rect<irr::s32>(5, 5, 200, 20), true, true, 0, -1, true);

  m_pLblStep->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);

  m_iSteps = m_pPlayer->getNumberOfSteps();

  if (m_bPluginHandlesCamera) {
    m_pCam = NULL;
    m_pFreeCam = NULL;
  }
  else {
    m_pCam=m_pSmgr->addCameraSceneNode();
    m_pFreeCam=m_pSmgr->addCameraSceneNodeFPS();
  }

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
    m_iThisStep++;
    wchar_t s[0xFF];
    swprintf(s, 0xFF, L"Step %i of %i", m_iThisStep, m_iSteps);
    m_pLblStep->setText(s);
    if (!m_bStepTaken) {
      m_bStepTaken = true;
      m_pPlugin->physicsInitialized();
    }

    irr::core::position2df pos=m_pCrsCtrl->getRelativePosition();
    m_pCrsCtrl->setPosition(irr::core::position2df(0.5f,0.5f));
    switch (m_eCamMode) {
      case eCamFree:
        break;

      case eCamFollow:
        if (m_pCam && m_pFocusedNode!=NULL) {
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
      m_bSceneLoaded=true;
      irr::u32 iSec = (m_iSteps /   60) % 60,
               iMin = (m_iSteps / 3600) % 60;

      printf("Replay information: %i steps (%i:%i minutes)\n",m_iSteps, iMin, iSec);

      wchar_t s[0xFF];
      swprintf(s, 0xFF, L"%i Steps, (%i:%i minutes)", m_iSteps, iMin, iSec);
      m_pLblInfo = m_pGuiEnv->addStaticText(s, irr::core::rect<irr::s32>(5, 25, 200, 40), true, true, m_pTab, -1, true);
      m_pLblInfo->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
    return true;
    }
  }
  else {
    if (pEvent->getType()==irr::ode::eIrrOdeEventNodeRemoved) {
      irr::ode::CIrrOdeEventNodeRemoved *p=(irr::ode::CIrrOdeEventNodeRemoved *)pEvent;
      irr::scene::ISceneNode *pNode=m_pSmgr->getSceneNodeFromId(p->getRemovedNodeId());
      if (pNode!=NULL) {
        pNode->setVisible(false);
        removeNode(pNode);
      }
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
  if (m_pPlugin->HandleEvent(event)) return true;

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
      m_fCamDist+=0.25f*event.MouseInput.Wheel;
    }

  return bRet;
}

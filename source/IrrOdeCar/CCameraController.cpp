  #include <CCameraController.h>
  #include <CRearView.h>
  #include <irrCC.h>

CCameraController::CCameraController(irr::IrrlichtDevice *pDevice, irrklang::ISoundEngine *pSndEngine, CIrrCC *pCtrl, irr::ode::CIrrOdeManager *pOdeMngr, irr::f32 fVrAr) {
  m_pDevice = pDevice;
  m_pSndEngine = pSndEngine;
  m_pOdeMngr = pOdeMngr;

  m_pSmgr = m_pDevice->getSceneManager();
  m_pDriver = m_pDevice->getVideoDriver();
  m_pCam = m_pSmgr->addCameraSceneNode();
  m_pCam->setNearValue(0.1f);
  m_pController = pCtrl;
  m_fInitialFOV = m_pCam->getFOV();
  m_fInitialAR  = m_pCam->getAspectRatio();
  m_fVrAr       = fVrAr;

  m_pOdeMngr->getIrrThread()->getInputQueue()->addEventListener(this);

  m_pCursor = m_pDevice->getCursorControl();

  m_cScreen = m_pDevice->getVideoDriver()->getScreenSize() / 2;

  m_fCamAngleH = 0.0f;
  m_fCamAngleV = 0.0f;

  m_fTgtAngleH = 0.0f;
  m_fTgtAngleV = 0.0f;

  m_fExtFact = 1.0f;

  m_cMousePos = irr::core::position2di(-1, -1);

  m_bLeftMouse = false;
  m_bRghtMouse = false;

  setTarget(NULL);

  m_vPosition  = irr::core::vector3df(3505.0f, 1000.0f, 2490.0f);
  m_vDirection = irr::core::vector3df(   0.0f,    0.0f,    1.0f);

  m_pRearView = new CRearView(m_pDevice,"rearview.jpg",m_pSmgr->addCameraSceneNode(), m_pSmgr->getRootSceneNode());
  m_pRearView->setActive(true);

  m_fCamAngleH = 25.0f;

  m_bInternal     = false;
  m_bButton       = true;
  m_b3d           = false;
  m_bLeft         = false;
  m_bFocusNear    = false;
  m_bVr           = false;
  m_bShowFps      = false;
  m_bFrontMonitor = false;

  irr::core::dimension2du cScreenSize=m_pDevice->getVideoDriver()->getScreenSize();

  m_cFull = irr::core::rect<irr::s32>(0, 0, cScreenSize.Width, cScreenSize.Height);
  m_cLeft = irr::core::rect<irr::s32>(0, 0, cScreenSize.Width, (cScreenSize.Height / 2));
  m_cRght = irr::core::rect<irr::s32>(0, cScreenSize.Height / 2, cScreenSize.Width, cScreenSize.Height);
  m_cVrLf = irr::core::rect<irr::s32>(0, 0, (cScreenSize.Width / 2), cScreenSize.Height);
  m_cVrRg = irr::core::rect<irr::s32>(cScreenSize.Width / 2, 0, cScreenSize.Width, cScreenSize.Height);

  m_pFps = m_pDevice->getGUIEnvironment()->addStaticText(L"FPS", irr::core::rect<irr::s32>(irr::core::position2di(cScreenSize.Width - 305, 5), irr::core::dimension2du(300, 15)), true, false, NULL, -1, true);
  m_pFps->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
  m_pFps->setVisible(false);
}

CCameraController::~CCameraController() {
  m_pOdeMngr->getIrrThread()->getInputQueue()->removeEventListener(this);
}

void CCameraController::setTarget(irr::ode::CIrrOdeBody *pTarget) {
  m_pTarget = pTarget;

  if (m_pTarget == NULL) {
    irr::core::vector3df v = m_pCam->getTarget() - m_pCam->getPosition(),
                         a = v.getHorizontalAngle();

    m_fTgtAngleH = m_fCamAngleH;
    m_fTgtAngleV = m_fCamAngleV;

    m_fCamAngleH = -a.Y;
    m_fCamAngleV = -a.X;

    while (m_fCamAngleH >  180.0f) m_fCamAngleH -= 360.0f;
    while (m_fCamAngleH < -180.0f) m_fCamAngleH += 360.0f;

    while (m_fCamAngleV >  180.0f) m_fCamAngleV -= 360.0f;
    while (m_fCamAngleV < -180.0f) m_fCamAngleV += 360.0f;

    if (m_fCamAngleV >  80.0f) m_fCamAngleV =  80.0f;
    if (m_fCamAngleV < -80.0f) m_fCamAngleV = -80.0f;

    m_bRotateXY = false;

    m_pCursor->setVisible(true);

    if (m_cMousePos.X != -1) {
      m_pCursor->setPosition(m_cMousePos);
      m_cMousePos = irr::core::position2di(-1,-1);
    }

    m_vDirection = irr::core::vector3df(0.0f, 0.0f, 1.0f);
  }
  else {
    m_fCamAngleH = m_fTgtAngleH;
    m_fCamAngleV = m_fTgtAngleV;

    m_pCursor->setVisible(false);
    if (m_cMousePos.X == -1) m_cMousePos = m_pCursor->getPosition();
    m_pCursor->setPosition(irr::core::position2df(0.5f,0.5f));

    if (m_pTarget->getOdeClassname() == "car") {
      m_vInternalOffset = irr::core::vector3df( 0.05f, 0.34f, 0.0f);

      m_vDirection = irr::core::vector3df(-1.0f,0.0f,0.0f);
      m_bRotateXY = true;

      m_fExtOffset = 1.0f;
      m_fExtDist   = 3.0f;

      m_vRViewOffset = m_vInternalOffset + irr::core::vector3df(0.0f, 0.15f, 0.0f);
      m_vFViewOffset = m_vRViewOffset;
      m_vFViewDir = -m_vDirection;
    }
    else
      if (m_pTarget->getOdeClassname() == "tank") {
        m_vInternalOffset = irr::core::vector3df(10.0f, 5.00f, 0.0f);

        m_vDirection = irr::core::vector3df(-1.0f,0.0f,0.0f);
        m_bRotateXY = true;

        m_fExtDist   = 0.4f;
        m_fExtOffset = 0.0f;

        m_vRViewOffset = irr::core::vector3df(0.0f, 0.0f, 0.0f);
        m_vFViewOffset = m_vRViewOffset;
        m_vFViewDir = m_vDirection;
      }
      else
        if (m_pTarget->getOdeClassname() == "heli") {
          m_vInternalOffset = irr::core::vector3df(0.0f, 0.016f, -0.5f);

          m_vDirection = irr::core::vector3df(0.0f,0.0f,-1.0f);
          m_bRotateXY = false;

          m_fExtDist   = 7.0f;
          m_fExtOffset = 2.0f;

          m_vRViewOffset = m_vInternalOffset + irr::core::vector3df( 1.75f,  0.1f,  0.0f);
          m_vFViewOffset = m_vInternalOffset + irr::core::vector3df( 0.00f, -1.5f, -1.6f);
          m_vFViewDir = irr::core::vector3df(0.0f, -1.0f, -1.0f);
        }
        else
          if (m_pTarget->getOdeClassname() == "plane") {
            m_vInternalOffset = irr::core::vector3df(0.0f, 0.55f, -0.3f);

            m_vDirection = irr::core::vector3df(0.0f,0.0f,-1.0f);
            m_bRotateXY = false;

            m_fExtDist   = 7.5f;
            m_fExtOffset = 1.5f;

            m_vRViewOffset = m_vInternalOffset + irr::core::vector3df( 1.75f,  0.1f,  0.0f);
            m_vFViewOffset = m_vInternalOffset + irr::core::vector3df( 0.00f, -1.5f, -1.6f);
            m_vFViewDir = m_vDirection;
          }
          else setTarget(NULL);
  }
}

void CCameraController::updateRearView() {
  if (m_pRearView != NULL) {
    if (m_bFrontMonitor)
      m_pRearView->setCameraParameters(m_vFViewPos, m_vFViewTgt, m_vUp);
    else
      m_pRearView->setCameraParameters(m_vRViewPos, m_vRViewTgt, m_vUp);
    m_pRearView->update();
  }
}

void CCameraController::update() {
  m_pSmgr->setActiveCamera(m_pCam);

  irr::core::vector3df v = m_vDirection;
  if (m_bRotateXY)
    v.rotateXYBy(m_fCamAngleV);
  else
    v.rotateYZBy(-m_fCamAngleV);

  v.rotateXZBy(m_fCamAngleH);

#ifndef NO_IRRKLANG
  if (m_pTarget != NULL) {
    irr::core::vector3df vRot = m_pTarget->getRotation(),
                         vPos = m_pTarget->getPosition();

    if (m_bInternal) {
      m_vPosition = m_pTarget->getPosition() + vRot.rotationToDirection(m_vInternalOffset);
      m_vTarget   = m_pTarget->getPosition() + vRot.rotationToDirection(m_vInternalOffset + v);
      m_vUp       = vRot.rotationToDirection(irr::core::vector3df(0.0f, 1.0f, 0.0f));
    }
    else {
      m_vPosition = m_pTarget->getPosition() + vRot.rotationToDirection(m_vInternalOffset - (m_fExtFact * m_fExtDist * v - m_fExtFact * m_fExtOffset * irr::core::vector3df(0.0f, 1.0f, 0.0f)));
      m_vTarget   = m_pTarget->getPosition() + vRot.rotationToDirection(m_vInternalOffset);
      m_vUp = vRot.rotationToDirection(irr::core::vector3df(0.0f, 1.0f, 0.0f));
    }
  }
  else {
    m_vTarget = m_vPosition + v;
    m_vUp = irr::core::vector3df(0.0f, 1.0f, 0.0f);
  }
#else
  m_vTarget = m_vPosition + v;
  m_vUp = irr::core::vector3df(0.0f, 1.0f, 0.0f);
#endif

  if (m_b3d || m_bVr) {
    irr::core::vector3df v1 = m_vTarget - m_vPosition,
                         vSide = v1.crossProduct(m_vUp);

    if (m_bFocusNear) {
      m_vTarget -= 0.5 * v1;
    }

    if (m_bLeft) {
      m_vPosition += 0.01f * vSide;
      m_bLeft = false;
    }
    else {
      m_bLeft = true;
      m_vPosition -= 0.01f * vSide;
    }
  }

  m_pCam->setPosition(m_vPosition);
  m_pCam->setTarget  (m_vTarget  );
  m_pCam->setUpVector(m_vUp      );

#ifndef NO_IRRKLANG
  if (m_pSndEngine) {
    irr::core::vector3df irrPos=m_pCam->getPosition(),
                         irrTgt=m_pCam->getTarget(),
                         irrUp =m_pCam->getUpVector(),
                         irrVel=m_pTarget!=NULL?m_pTarget->getLinearVelocity():irr::core::vector3df(0.0f,0.0f,0.0f);

    irrklang::vec3df pos=irrklang::vec3df(irrPos.X,irrPos.Y,irrPos.Z),
                     tgt=irrklang::vec3df(irrTgt.X,irrTgt.Y,irrTgt.Z),
                     up =irrklang::vec3df(irrUp .X,irrUp .Y,irrUp .Z),
                     vel=irrklang::vec3df(irrVel.X,irrVel.Y,irrVel.Z);

    m_pSndEngine->setListenerPosition(pos,tgt,vel,up);
    m_pSndEngine->setRolloffFactor(0.125f);
  }
#endif
}

bool CCameraController::OnEvent(const irr::SEvent &event) {
  if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
    m_bLeftMouse = event.MouseInput.isLeftPressed ();
    m_bRghtMouse = event.MouseInput.isRightPressed();

    m_fExtFact += 0.01f * event.MouseInput.Wheel;

    if (m_fExtFact > 3.0f) m_fExtFact = 3.0f;
    if (m_fExtFact < 0.1f) m_fExtFact = 0.1f;
  }

  return false;
}

bool CCameraController::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == irr::ode::eIrrOdeEventStep) {
    irr::core::position2di pos = m_pCursor->getPosition();
    irr::s32 offsetx = 0,
             offsety = 0;

    if (m_pTarget != NULL) {
      m_pCursor->setPosition(0.5f, 0.5f);
      offsetx = m_cScreen.Width  - pos.X,
      offsety = m_cScreen.Height - pos.Y;
    }
    else {
      offsetx = m_cMousePos.X - pos.X;
      offsety = m_cMousePos.Y - pos.Y;

      m_cMousePos = pos;
    }

    if (m_bLeftMouse || (!m_bButton && m_pTarget != NULL)) {
      m_fCamAngleH += (irr::f32)(offsetx) / 8.0f;

      if (m_pTarget != NULL)
        m_fCamAngleV -= (irr::f32)(offsety) / 8.0f;
      else
        m_fCamAngleV += (irr::f32)(offsety) / 8.0f;
    }
    else
      if (m_bRghtMouse && m_pTarget == NULL) {
        irr::core::vector3df v = m_vDirection;
        if (m_bRotateXY)
          v.rotateXYBy(m_fCamAngleV);
        else
          v.rotateYZBy(-m_fCamAngleV);

        v.rotateXZBy(m_fCamAngleH);
        m_vPosition += offsety * v;

        v = v.crossProduct(m_vUp);
        m_vPosition += offsetx * v;
      }

    if (m_pController->get(m_pCtrls[eCameraButtonMove])) {
      m_pController->set(m_pCtrls[eCameraButtonMove],0.0f);
      m_bButton = !m_bButton;
    }

    m_bFocusNear = m_pController->get(m_pCtrls[eCameraNearFocus]) != 0.0f;

    if (m_pController->get(m_pCtrls[eCameraFrontMonitor])) {
      m_pController->set(m_pCtrls[eCameraFrontMonitor], 0.0f);
      m_bFrontMonitor = !m_bFrontMonitor;
    }

    if (m_pController->get(m_pCtrls[eCameraInternal])) {
      m_pController->set(m_pCtrls[eCameraInternal], 0.0f);
      m_bInternal = !m_bInternal;
    }

    if (m_pController->get(m_pCtrls[eCameraLeft])!=0.0f) {
      m_fCamAngleH+=m_pController->get(m_pCtrls[eCameraLeft]);
    }

    while (m_fCamAngleH >=  180.0f) m_fCamAngleH -= 360.0f;
    while (m_fCamAngleH <  -180.0f) m_fCamAngleH += 360.0f;

    if (m_pController->get(m_pCtrls[eCameraDown])!=0.0f) {
      m_fCamAngleV+=m_pController->get(m_pCtrls[eCameraDown]);
    }

    if (m_fCamAngleV> 80.0f) m_fCamAngleV= 80.0f;
    if (m_fCamAngleV<-80.0f) m_fCamAngleV=-80.0f;

    if (m_pController->get(m_pCtrls[eCameraCenter]) || (m_bRghtMouse && m_pTarget != NULL)) {
      if (m_fCamAngleH!=0.0f) {
        if (m_fCamAngleH>0.0f) {
          m_fCamAngleH-=5.0f;
          if (m_fCamAngleH<0.0f) m_fCamAngleH=0.0f;
        }
        else {
          m_fCamAngleH+=5.0f;
          if (m_fCamAngleH>0.0f) m_fCamAngleH=0.0f;
        }
      }

      if (m_fCamAngleV!=0.0f) {
        if (m_fCamAngleV>0.0f) {
          m_fCamAngleV-=5.0f;
          if (m_fCamAngleV<0.0f) m_fCamAngleV=0.0f;
        }
        else {
          m_fCamAngleV+=5.0f;
          if (m_fCamAngleV>0.0f) m_fCamAngleV=0.0f;
        }
      }

      if (m_fExtFact > 1.0f) {
        m_fExtFact -= 0.05f;
        if (m_fExtFact < 1.0f) m_fExtFact = 1.0f;
      }

      if (m_fExtFact < 1.0f) {
        m_fExtFact += 0.05f;
        if (m_fExtFact > 1.0f) m_fExtFact = 1.0f;
      }
    }
  }

  if (pEvent->getType() == irr::ode::eIrrOdeEventBodyMoved) {
    irr::ode::CIrrOdeEventBodyMoved *p = reinterpret_cast<irr::ode::CIrrOdeEventBodyMoved *>(pEvent);

    if (m_pTarget != NULL && m_pRearView != NULL && p->getBodyId() == m_pTarget->getID()) {
      m_vRViewPos = p->getNewPosition() + p->getNewRotation().rotationToDirection(m_vRViewOffset);
      m_vFViewPos = p->getNewPosition() + p->getNewRotation().rotationToDirection(m_vFViewOffset);

      m_vFViewTgt = m_vFViewPos + p->getNewRotation().rotationToDirection(m_vFViewDir );
      m_vRViewTgt = m_vRViewPos - p->getNewRotation().rotationToDirection(m_vDirection);
    }
  }

  return false;
}

bool CCameraController::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == irr::ode::eIrrOdeEventStep || pEvent->getType() == irr::ode::eIrrOdeEventBodyMoved;
}

void CCameraController::toggle3d() {
  m_b3d = !m_b3d;
  m_pFps->setVisible(m_bShowFps && !m_bVr);
  printf("3d mode %s\n", m_b3d ? "activated" : "disabled");
}

void CCameraController::toggleVr() {
  m_bVr = !m_bVr;
  printf("vr mode %s\n", m_bVr ? "activated" : "disabled");

  if (m_bVr) {
    m_pFps->setVisible(false);
    m_pCam->setFOV(110.0f * M_PI / 180.0f);
    m_pCam->setAspectRatio(m_fVrAr);
  }
  else {
    m_pFps->setVisible(m_bShowFps && !m_b3d);
    m_pCam->setFOV(m_fInitialFOV);
    m_pCam->setAspectRatio(m_fInitialAR);
  }
}

void CCameraController::render() {
  if (m_bVr) {
    update();
    m_pDriver->setViewPort(m_cVrLf);
    m_pSmgr->drawAll();

    update();
    m_pDriver->setViewPort(m_cVrRg);
    m_pSmgr->drawAll();
  }
  else
    if (m_b3d) {
      update();
      m_pDriver->setViewPort(m_cLeft);
      m_pSmgr->drawAll();

      update();
      m_pDriver->setViewPort(m_cRght);
      m_pSmgr->drawAll();
    }
    else {
      update();
      m_pDriver->setViewPort(m_cFull);
      m_pSmgr->drawAll();
    }
}

void CCameraController::setFps(const wchar_t *s) {
  m_pFps->setText(s);
}

void CCameraController::setShowFps(bool b) {
  m_bShowFps = b;
  printf("show FPS: %s\n", b ? "enabled" : "disabled");
  m_pFps->setVisible(m_bShowFps && !m_bVr && !m_b3d);
}

  #include <CCameraController.h>
  #include <irrCC.h>

CCameraController::CCameraController(irr::IrrlichtDevice *pDevice, irrklang::ISoundEngine *pSndEngine, CIrrCC *pCtrl) {
  m_pDevice = pDevice;
  m_pTarget = NULL;
  m_pSndEngine = pSndEngine;

  m_pSmgr = m_pDevice->getSceneManager();
  m_pCam = m_pSmgr->addCameraSceneNode();
  m_pCam->setNearValue(0.1f);
  m_pController = pCtrl;

  m_bInternal = false;

  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->addEventListener(this);

  m_fCamAngleH = 0.0f;
  m_fCamAngleV = 0.0f;
}

CCameraController::~CCameraController() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CCameraController::setTarget(irr::ode::CIrrOdeBody *pTarget) {
  m_pTarget = pTarget;

  if (m_pTarget==NULL) {
    m_vInternalOffset = irr::core::vector3df(0.0f,0.0f, 0.0f);
    m_bRotateXY = true;
  }
  else
    if (m_pTarget->getOdeClassname() == "car") {
      m_vInternalOffset = irr::core::vector3df( 0.0f, 1.35f, 0.0f);

      m_vDirection = irr::core::vector3df(-1.0f,0.0f,0.0f);
      m_bRotateXY = true;

      m_fExtOffset =  3.0f;
      m_fExtDist   = 15.0f;
    }
    else
      if (m_pTarget->getOdeClassname() == "tank") {
        m_vInternalOffset = irr::core::vector3df(10.0f, 5.00f, 0.0f);

        m_vDirection = irr::core::vector3df(-1.0f,0.0f,0.0f);
        m_bRotateXY = true;

        m_fExtDist   = 1.0f;
        m_fExtOffset = 0.0f;
      }
      else
        if (m_pTarget->getOdeClassname() == "heli") {
          m_vInternalOffset = irr::core::vector3df(0.0f, 0.1f, -1.0f);

          m_vDirection = irr::core::vector3df(0.0f,0.0f,-1.0f);
          m_bRotateXY = false;

          m_fExtDist   = 15.0f;
          m_fExtOffset =  5.0f;
        }
        else
          if (m_pTarget->getOdeClassname() == "plane") {
            m_vInternalOffset = irr::core::vector3df(0.0f, 1.1f, -0.6f);

            m_vDirection = irr::core::vector3df(0.0f,0.0f,-1.0f);
            m_bRotateXY = false;

            m_fExtDist   = 15.0f;
            m_fExtOffset =  3.0f;
          }
}

void CCameraController::update() {
  m_pSmgr->setActiveCamera(m_pCam);

  if (m_pTarget) {
    irr::core::vector3df vRot = m_pTarget->getRotation(),
                         vPos = m_pTarget->getPosition();

    irr::core::vector3df v = m_vDirection;
    if (m_bRotateXY)
      v.rotateXYBy(m_fCamAngleV);
    else
      v.rotateYZBy(-m_fCamAngleV);

    v.rotateXZBy(m_fCamAngleH);

    if (m_bInternal) {
      m_vPosition = m_pTarget->getPosition() + vRot.rotationToDirection(m_vInternalOffset);
      m_vTarget   = m_pTarget->getPosition() + vRot.rotationToDirection(m_vInternalOffset + v);
      m_vUp       = vRot.rotationToDirection(irr::core::vector3df(0.0f, 1.0f, 0.0f));
    }
    else {
      m_vPosition = m_pTarget->getPosition() + vRot.rotationToDirection(m_vInternalOffset - (m_fExtDist * v - m_fExtOffset * irr::core::vector3df(0.0f, 1.0f, 0.0f)));
      m_vTarget   = m_pTarget->getPosition() + vRot.rotationToDirection(m_vInternalOffset);
      m_vUp = vRot.rotationToDirection(irr::core::vector3df(0.0f, 1.0f, 0.0f));
    }

    m_pCam->setPosition(m_vPosition);
    m_pCam->setTarget  (m_vTarget  );
    m_pCam->setUpVector(m_vUp      );
  }
  else {
  }

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
}

bool CCameraController::OnEvent(const irr::SEvent &event) {
  return false;
}

bool CCameraController::onEvent(irr::ode::IIrrOdeEvent *pEvent) {
  if (pEvent->getType() == irr::ode::eIrrOdeEventStep) {
    if (m_pController->get(m_pCtrls[eCameraInternal])) {
      m_pController->set(m_pCtrls[eCameraInternal], 0.0f);
      m_bInternal = !m_bInternal;
    }

    if (m_pController->get(m_pCtrls[eCameraLeft])!=0.0f) {
      m_fCamAngleH+=m_pController->get(m_pCtrls[eCameraLeft]);

      if (m_fCamAngleH> 190.0f) m_fCamAngleH= 190.0f;
      if (m_fCamAngleH<-190.0f) m_fCamAngleH=-190.0f;
    }

    if (m_pController->get(m_pCtrls[eCameraDown])!=0.0f) {
      m_fCamAngleV+=m_pController->get(m_pCtrls[eCameraDown]);

      if (m_fCamAngleV> 60.0f) m_fCamAngleV= 60.0f;
      if (m_fCamAngleV<-60.0f) m_fCamAngleV=-60.0f;
    }

    if (m_pController->get(m_pCtrls[eCameraCenter])) {
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
    }
  }

  return false;
}

bool CCameraController::handlesEvent(irr::ode::IIrrOdeEvent *pEvent) {
  return pEvent->getType() == irr::ode::eIrrOdeEventStep;
}

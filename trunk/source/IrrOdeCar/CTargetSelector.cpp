  #include <irrlicht.h>
  #include <CTargetSelector.h>
  #include <irrode.h>

using namespace irr;

CTargetSelector::CTargetSelector(ode::CIrrOdeBody *pCarrier, IrrlichtDevice *pDevice, core::vector3df vForeward) {
  m_pDevice=pDevice;
  m_pCarrier=pCarrier;
  m_vForeward=vForeward;
  m_pManager=ode::CIrrOdeManager::getSharedInstance();
  m_pColMgr=pDevice->getSceneManager()->getSceneCollisionManager();
  m_pDrv=pDevice->getVideoDriver();
  m_pTarget=NULL;

  core::list<irr::ode::CIrrOdeSceneNode *> pNodes=m_pManager->getIrrOdeNodes();
  core::list<irr::ode::CIrrOdeSceneNode *>::Iterator it;

  for (it=pNodes.begin(); it!=pNodes.end(); it++) {
    if ((*it)->getType()==ode::IRR_ODE_BODY_ID) {
      ode::CIrrOdeBody *p=(ode::CIrrOdeBody *)(*it);
      if (p->getParentBody()==NULL) m_lOptions.push_back(p);
    }
  }
}

void CTargetSelector::highlightTargets() {
  if (m_pOption!=NULL && m_pOption!=m_pTarget) {
    core::position2di cPos=m_pColMgr->getScreenCoordinatesFrom3DPosition(m_pOption->getAbsolutePosition());
    core::recti cRect=core::recti(cPos-core::position2di(20,20),cPos+core::position2di(20,20));
    m_pDrv->draw2DRectangleOutline(cRect,video::SColor(0xFF,0,0xFF,0));
  }
  else
    if (m_pTarget!=NULL) {
      core::position2di cPos=m_pColMgr->getScreenCoordinatesFrom3DPosition(m_pTarget->getAbsolutePosition());
      core::recti cRect=core::recti(cPos-core::position2di(20,20),cPos+core::position2di(20,20));
      m_pDrv->draw2DRectangleOutline(cRect,video::SColor(0xFF,0xFF,0,0));
    }
}

void CTargetSelector::update() {
  core::vector3df vDir=m_pCarrier->getRotation().rotationToDirection(m_vForeward),
                  vStart=m_pCarrier->getAbsolutePosition()+5.0f*vDir,
                  vEnd=m_pCarrier->getAbsolutePosition()+2500.0f*vDir;

  core::line3df theLine(vStart,vEnd);

  core::recti cRect;
  m_pOption=NULL;

  core::list<ode::CIrrOdeBody *>::Iterator it;

  for (it=m_lOptions.begin(); it!=m_lOptions.end(); it++) {
    ode::CIrrOdeBody *p=*it;
    core::vector3df vPos=p->getAbsolutePosition(),
                    vPoint=theLine.getClosestPoint(vPos);

    f32 fDist=2500.0f;

    if (vPoint!=vStart && vPoint!=vEnd) {
      f32 f1=vPoint.getDistanceFrom(vPos),
          f2=vPoint.getDistanceFrom(m_pCarrier->getAbsolutePosition());

      if (f1*5.0f<f2 && f1<fDist) {
        core::position2di cPos=m_pColMgr->getScreenCoordinatesFrom3DPosition(vPos);
        cRect=core::recti(cPos-core::position2di(20,20),cPos+core::position2di(20,20));
        m_pOption=p;
        fDist=f1;
      }
    }
  }

  if (m_pTarget!=NULL) {
    core::vector3df vPos=m_pTarget->getAbsolutePosition(),
                    vPoint=theLine.getClosestPoint(vPos);

    f32 f1=vPoint.getDistanceFrom(vPos),
        f2=vPoint.getDistanceFrom(m_pCarrier->getAbsolutePosition());

    if (f1*5.0f>=f2) m_pTarget=NULL;
  }
}

bool CTargetSelector::selectOption() {
  if (m_pOption!=NULL) {
    m_pTarget=m_pOption;
    return true;
  }
  return false;
}

ode::CIrrOdeBody *CTargetSelector::getTarget() {
  return m_pTarget;
}

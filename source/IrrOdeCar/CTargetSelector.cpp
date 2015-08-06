  #include <irrlicht.h>
  #include <CTargetSelector.h>
  #include <CEventVehicleState.h>
  #include <IrrOde.h>

CTargetSelector::CTargetSelector(irr::ode::CIrrOdeBody *pCarrier, irr::IrrlichtDevice *pDevice, irr::core::vector3df vForeward) {
  m_pDevice=pDevice;
  m_pCarrier=pCarrier;
  m_vForeward=vForeward;
  m_pManager=irr::ode::CIrrOdeManager::getSharedInstance();
  m_pColMgr=pDevice->getSceneManager()->getSceneCollisionManager();
  m_pDrv=pDevice->getVideoDriver();
  m_pTarget=NULL;

  irr::ode::CIrrOdeWorld *pWorld = pCarrier->getWorld();

  if (pWorld) {
    irr::core::list<irr::ode::CIrrOdeSceneNode *> nodes = pWorld->getIrrOdeNodes();
    irr::core::list<irr::ode::CIrrOdeSceneNode *>::Iterator it;

    for (it=nodes.begin(); it!=nodes.end(); it++) {
      if ((*it)->getType()==irr::ode::IRR_ODE_BODY_ID) {
        irr::ode::CIrrOdeBody *p=(irr::ode::CIrrOdeBody *)(*it);
        if (p->getParentBody()==NULL) m_lOptions.push_back(p);
      }
    }
  }
}

void CTargetSelector::highlightTargets() {
  if (m_pOption!=NULL && m_pOption!=m_pTarget) {
    irr::core::position2di cPos=m_pColMgr->getScreenCoordinatesFrom3DPosition(m_pOption->getAbsolutePosition());
    irr::core::recti cRect=irr::core::recti(cPos-irr::core::position2di(20,20),cPos+irr::core::position2di(20,20));
    m_pDrv->draw2DRectangleOutline(cRect,irr::video::SColor(0xFF,0,0xFF,0));
  }
  else
    if (m_pTarget!=NULL) {
      irr::core::position2di cPos=m_pColMgr->getScreenCoordinatesFrom3DPosition(m_pTarget->getAbsolutePosition());
      irr::core::recti cRect=irr::core::recti(cPos-irr::core::position2di(20,20),cPos+irr::core::position2di(20,20));
      m_pDrv->draw2DRectangleOutline(cRect,irr::video::SColor(0xFF,0xFF,0,0));
    }
}

void CTargetSelector::update() {
  irr::core::vector3df vDir=m_pCarrier->getRotation().rotationToDirection(m_vForeward),
                       vStart=m_pCarrier->getAbsolutePosition()+5.0f*vDir,
                       vEnd=m_pCarrier->getAbsolutePosition()+2500.0f*vDir;

  irr::core::line3df theLine(vStart,vEnd);

  irr::core::recti cRect;
  m_pOption=NULL;

  irr::core::list<irr::ode::CIrrOdeBody *>::Iterator it;

  for (it=m_lOptions.begin(); it!=m_lOptions.end(); it++) {
    irr::ode::CIrrOdeBody *p=*it;
    irr::core::vector3df vPos=p->getAbsolutePosition(),
                         vPoint=theLine.getClosestPoint(vPos);

    irr::f32 fDist=2500.0f;

    if (vPoint!=vStart && vPoint!=vEnd) {
      irr::f32 f1=vPoint.getDistanceFrom(vPos),
               f2=vPoint.getDistanceFrom(m_pCarrier->getAbsolutePosition());

      if (f1*5.0f<f2 && f1<fDist) {
        irr::core::position2di cPos=m_pColMgr->getScreenCoordinatesFrom3DPosition(vPos);
        cRect=irr::core::recti(cPos-irr::core::position2di(20,20),cPos+irr::core::position2di(20,20));
        m_pOption=p;
        fDist=f1;
      }
    }
  }

  if (m_pTarget!=NULL) {
    irr::core::vector3df vPos=m_pTarget->getAbsolutePosition(),
                         vPoint=theLine.getClosestPoint(vPos);

    irr::f32 f1=vPoint.getDistanceFrom(vPos),
             f2=vPoint.getDistanceFrom(m_pCarrier->getAbsolutePosition());

    if (f1*5.0f>=f2) {
      m_pTarget=NULL;
      CEventSelectTarget *p = new CEventSelectTarget(m_pCarrier->getID(), -1);
      irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);
    }
  }
}

bool CTargetSelector::selectOption() {
  if (m_pOption!=NULL) {
    m_pTarget=m_pOption;

    CEventSelectTarget *p = new CEventSelectTarget(m_pCarrier->getID(), m_pOption->getID());
    irr::ode::CIrrOdeManager::getSharedInstance()->getOdeThread()->getOutputQueue()->postEvent(p);

    return true;
  }
  return false;
}

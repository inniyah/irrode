  #include "CHeli.h"
  #include <CProjectile.h>
  #include <CAutoPilot.h>
  #include <irrCC.h>
  #include <CTargetSelector.h>
  #include <irrKlang.h>
  #include <CEventVehicleState.h>
  #include <CCustomEventReceiver.h>

CHeli::CHeli(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, irr::ode::IIrrOdeEventQueue *pInputQueue) : CAeroVehicle(pDevice, pNode, pInputQueue) {
  m_pAutoPilot=new CAutoPilot(m_pBody,m_pAero,m_pTorque,m_pMotor,m_pRay);

  m_pTargetSelector=new CTargetSelector(m_pBody,m_pDevice,m_pAero->getForeward());

  m_bLeft=false;
  m_fSound=0.0f;
  m_iNodeId=pNode->getID();

  CCustomEventReceiver::getSharedInstance()->addHeli(pNode);

  dataChanged();
}

CHeli::~CHeli() {
  irr::ode::CIrrOdeManager::getSharedInstance()->getQueue()->removeEventListener(this);
}

void CHeli::odeStep(irr::u32 iStep) {
  irr::core::vector3df rot=m_pBody->getRotation();

  if (m_bFirePrimary) {
    m_bFirePrimary=false;
    m_iLastShot1=iStep;
    //We add a new missile...
    irr::core::vector3df pos=m_pBody->getAbsolutePosition()+m_pBody->getRotation().rotationToDirection(irr::core::vector3df(m_bLeft?-3.0f:3.0f,-0.6f,-2.0f)),
                         rot=m_pBody->getAbsoluteTransformation().getRotationDegrees(),
                         vel=m_pBody->getLinearVelocity();

    CProjectile *p=new CProjectile(m_pSmgr,pos,rot,vel,"missile",600,m_pWorld,true,this);
    p->setTarget(m_pTargetSelector->getTarget());
    m_bLeft=!m_bLeft;
    incShotsFired();
  }

  m_fSound=0.75f+0.5*m_pMotor->getPower();
  if (m_bDataChanged) {
    dataChanged();
    m_bDataChanged=false;
  }
}

void CHeli::drawSpecifics() {
  m_pTargetSelector->highlightTargets();
}

irr::ode::IIrrOdeEvent *CHeli::writeEvent() {
  return new CEventHeliState(m_iNodeId,m_fSound,m_pAutoPilot->isEnabled(),m_fThrust);
}

irr::ode::eEventWriterType CHeli::getEventWriterType() {
  return irr::ode::eIrrOdeEventWriterUnknown;
}

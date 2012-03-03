  #include <motors/CIrrOdeImpulseMotor.h>

namespace irr {
  namespace ode {

CIrrOdeImpulseMotor::CIrrOdeImpulseMotor(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                    const irr::core::vector3df &position, const irr::core::vector3df &rotation, const irr::core::vector3df &scale)
                    :IIrrOdeStepMotor(parent,mgr,id,position,rotation,scale) {
  m_fMaxPower=0;
  m_fPower=0;
  m_vForeward=irr::core::vector3df(1.0f,0.0f,0.0f);

  #ifdef _IRREDIT_PLUGIN
    if (m_pMesh) {
      c8 sFileName[1024];
      sprintf(sFileName,"%sIrrOdeImpulseMotor.png",m_sResources);
      m_cMat.setTexture(0,m_pSceneManager->getVideoDriver()->getTexture(sFileName));
    }
  #endif
}

void CIrrOdeImpulseMotor::step() {
  if (m_pBody!=NULL && m_pBody->physicsInitialized() && m_bIsActive) {
    m_pBody->addForce(m_pBody->getRotation().rotationToDirection(m_fMaxPower*m_fPower*m_vForeward));
  }
}

const wchar_t *CIrrOdeImpulseMotor::getTypeName() {
  return IRR_ODE_IMPULSE_MOTOR_NAME;
}

void CIrrOdeImpulseMotor::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
  IIrrOdeStepMotor::serializeAttributes(out,options);
  out->addVector3d("foreward",m_vForeward);
}

void CIrrOdeImpulseMotor::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
  IIrrOdeStepMotor::deserializeAttributes(in,options);
  m_vForeward=in->getAttributeAsVector3d("foreward");
}

irr::scene::ISceneNode *CIrrOdeImpulseMotor::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager) {
  CIrrOdeImpulseMotor *pRet=new CIrrOdeImpulseMotor(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  pRet->setName(getName());
  pRet->setBody(reinterpret_cast<irr::ode::CIrrOdeBody *>(newParent));
  pRet->setForeward(m_vForeward);
  pRet->setPower(m_fPower);
  pRet->setMaxPower(m_fMaxPower);
  pRet->setIsActive(m_bIsActive);

  CIrrOdeSceneNode::cloneChildren(pRet,newManager);

  return pRet;
}

  } //namespace ode
}   //namespace irr

  #include <joints/CIrrOdeJointFixed.h>
  #include <CIrrOdeWorld.h>
  #include <IIrrOdeDevice.h>

namespace irr {
namespace ode {

CIrrOdeJointFixed::CIrrOdeJointFixed(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                                     const irr::core::vector3df &position,const irr::core::vector3df &rotation,const irr::core::vector3df &scale) :
                                     CIrrOdeJoint(parent, mgr, id, position, rotation, scale) {
  #ifdef _IRREDIT_PLUGIN
    if (m_pMesh) {
	  c8 sFileName[1024];
	  sprintf(sFileName,"%sIrrOdeJointFixed.png",m_sResources);
      m_cMat.setTexture(0,m_pSceneManager->getVideoDriver()->getTexture(sFileName));
    }
  #endif
}


CIrrOdeJointFixed::~CIrrOdeJointFixed() {
}

void CIrrOdeJointFixed::OnRegisterSceneNode() {
  if (IsVisible) SceneManager->registerNodeForRendering(this);
  irr::scene::ISceneNode::OnRegisterSceneNode();
}

void CIrrOdeJointFixed::render() {
  CIrrOdeSceneNode::render();
  #ifdef _DRAW_JOINT_INFO
    m_pVideoDriver->setMaterial(m_cMat);
    m_pVideoDriver->setTransform(irr::video::ETS_WORLD, core::matrix4());
    irr::core::vector3df pos1=m_pBody1?m_pBody1->getAbsolutePosition():getAbsolutePosition();
    irr::core::vector3df pos2=m_pBody2?m_pBody2->getAbsolutePosition():getAbsolutePosition();
    m_pVideoDriver->draw3DLine(pos1,pos2,irr::video::SColor(128,0,0xFF,0xFF));
  #endif
}

void CIrrOdeJointFixed::initPhysics() {
  if (m_bPhysicsInitialized) return;
  CIrrOdeJoint::initPhysics();

  updateAbsolutePosition();
  m_iJointId=m_pOdeDevice->jointCreateFixed(m_pWorld->getWorldId());

  if (getParent()->getID()==IRR_ODE_BODY_ID)
    setBody1(reinterpret_cast<CIrrOdeBody *>(getParent()));

  m_pOdeDevice->jointAttach(m_iJointId,m_pBody1,m_pBody2);
  m_pOdeDevice->jointSetFixed(m_iJointId);
  m_pOdeDevice->jointSetData(m_iJointId,this);
}

s32 CIrrOdeJointFixed::getID() const {
  return ID;
}

irr::scene::ESCENE_NODE_TYPE CIrrOdeJointFixed::getType() const {
  return (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_JOINT_FIXED_ID;
}

const wchar_t *CIrrOdeJointFixed::getTypeName() {
  return IRR_ODE_JOINT_FIXED_NAME;
}

void CIrrOdeJointFixed::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
  CIrrOdeJoint::serializeAttributes(out,options);
}

void CIrrOdeJointFixed::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
  CIrrOdeJoint::deserializeAttributes(in,options);
}


irr::scene::ISceneNode *CIrrOdeJointFixed::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager) {
  CIrrOdeJointFixed *pRet=new CIrrOdeJointFixed(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  copyParams(pRet);
  CIrrOdeSceneNode::cloneChildren(pRet,newManager);
  return pRet;
}

void CIrrOdeJointFixed::copyParams(CIrrOdeSceneNode *pDest, bool bRecurse) {
  if (bRecurse) CIrrOdeJoint::copyParams(pDest);
}

u16 CIrrOdeJointFixed::numParamGroups() const {
  return 0;
}

} //namespace ode
} //namespace irr

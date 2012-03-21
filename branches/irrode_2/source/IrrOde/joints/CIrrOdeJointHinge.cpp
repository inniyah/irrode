  #include <joints/CIrrOdeJointHinge.h>
  #include <CIrrOdeWorld.h>
  #include <IIrrOdeDevice.h>
  #include <event/CIrrOdeEventJointHinge.h>

namespace irr {
namespace ode {

CIrrOdeJointHinge::CIrrOdeJointHinge(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                                     const irr::core::vector3df &position,const irr::core::vector3df &rotation,const irr::core::vector3df &scale) :
                                     CIrrOdeJoint(parent, mgr, id, position, rotation, scale) {
  m_pAxis=irr::core::vector3df(0,0,1);
  #ifdef _IRREDIT_PLUGIN
    if (m_pMesh) {
	  c8 sFileName[1024];
	  sprintf(sFileName,"%sIrrOdeJointHinge.png",m_sResources);
      m_cMat.setTexture(0,m_pSceneManager->getVideoDriver()->getTexture(sFileName));
    }
  #endif

  m_fAngle=0.0f;
  m_fAngleRate=0.0f;
}


CIrrOdeJointHinge::~CIrrOdeJointHinge() {
}

void CIrrOdeJointHinge::OnRegisterSceneNode() {
  if (IsVisible) SceneManager->registerNodeForRendering(this);
  irr::scene::ISceneNode::OnRegisterSceneNode();
}

void CIrrOdeJointHinge::render() {
  CIrrOdeSceneNode::render();
  #ifdef _DRAW_JOINT_INFO
    m_pVideoDriver->setMaterial(m_cMat);
    m_pVideoDriver->setTransform(irr::video::ETS_WORLD, core::matrix4());
    irr::core::vector3df axisStart=getAbsolutePosition()-1*getAbsoluteTransformation().getRotationDegrees().rotationToDirection(m_pAxis);
    irr::core::vector3df axisEnd=axisStart+10*getAbsoluteTransformation().getRotationDegrees().rotationToDirection(m_pAxis);
    m_pVideoDriver->draw3DLine(axisStart,axisEnd,irr::video::SColor(128,0xFF,0,0));
    irr::core::vector3df pos1=m_pBody1?m_pBody1->getAbsolutePosition():getAbsolutePosition();
    irr::core::vector3df pos2=m_pBody2?m_pBody2->getAbsolutePosition():getAbsolutePosition();
    m_pVideoDriver->draw3DLine(pos1,getAbsolutePosition(),irr::video::SColor(128,0,0xFF,0));
    m_pVideoDriver->setTransform(irr::video::ETS_WORLD, core::matrix4());
    m_pVideoDriver->draw3DLine(pos2,getAbsolutePosition(),irr::video::SColor(128,0,0,0xFF));
  #endif
}

void CIrrOdeJointHinge::initPhysics() {
  if (m_bPhysicsInitialized) return;
  CIrrOdeJoint::initPhysics();

  //create the joint
  m_iJointId=m_pOdeDevice->jointCreateHinge(m_pWorld->getWorldId());

  //set the parent as body 1
  if (getParent()->getID()==IRR_ODE_BODY_ID)
    setBody1(reinterpret_cast<CIrrOdeBody *>(getParent()));

  irr::core::vector3df rot=getAbsoluteTransformation().getRotationDegrees(),
            axis=rot.rotationToDirection(m_pAxis);

  m_pOdeDevice->jointAttach(m_iJointId,m_pBody1,m_pBody2);
  m_pOdeDevice->jointSetHingeAnchor(m_iJointId,getAbsolutePosition());
  m_pOdeDevice->jointSetHingeAxis(m_iJointId,axis);
  m_pOdeDevice->jointSetData(m_iJointId,this);

  #ifdef _TRACE_INIT_PHYSICS
    printf("CIrrOdeJointHinge: %i: %.2f, %.2f, %.2f\n",ID,getAbsolutePosition().X,getAbsolutePosition().Y,getAbsolutePosition().Z);
  #endif

  for (s32 i=0; i<eParamEnd; i++)
    if (m_bParamUsed[i][0]) {
      #ifdef _TRACE_INIT_PHYSICS
        printf("CIrrOdeJointHinge::initPhysics: parameter %i set: %.2f\n",i,m_fParam[i][0]);
      #endif
      setParameter(0,(eJointParameter)i,m_fParam[i][0]);
    }

}

s32 CIrrOdeJointHinge::getID() const {
  return ID;
}

irr::scene::ESCENE_NODE_TYPE CIrrOdeJointHinge::getType() const {
  return (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_JOINT_HINGE_ID;
}

const wchar_t *CIrrOdeJointHinge::getTypeName() {
  return IRR_ODE_JOINT_HINGE_NAME;
}

void CIrrOdeJointHinge::setHingeAxis(irr::core::vector3df pAxis) {
  m_pAxis=pAxis.normalize();
}

irr::core::vector3df CIrrOdeJointHinge::getHingeAxis() {
  return m_pAxis;
}

irr::core::vector3df CIrrOdeJointHinge::getAnchor1() {
  return m_pAnchor1;
}

irr::core::vector3df CIrrOdeJointHinge::getAnchor2() {
  return m_pAnchor2;
}

f32 CIrrOdeJointHinge::getHingeAngle() {
  return m_fAngle;
}

f32 CIrrOdeJointHinge::getHingeAngleRate() {
  return m_fAngleRate;
}

u16 CIrrOdeJointHinge::numParamGroups() const {
  return 1;
}

void CIrrOdeJointHinge::setParameter(u16 iGroup, eJointParameter iParam, f32 fValue) {
  #ifdef _TRACE_INIT_PHYSICS
    printf("CIrrOdeJointHinge::setParameter: %i, %i, %.2f\n",iGroup,(int)iParam,fValue);
  #endif
  CIrrOdeJoint::setParameter(iGroup,iParam,fValue);

  if (m_iJointId) m_pOdeDevice->jointSetHingeParam(m_iJointId,iParam,fValue);
}

f32 CIrrOdeJointHinge::getParameter(u16 iGroup, eJointParameter iParam) {
  return m_iJointId?m_pOdeDevice->jointGetHingeParam(m_iJointId,iParam):0.0f;
}

void CIrrOdeJointHinge::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
  CIrrOdeJoint::serializeAttributes(out,options);
  out->addVector3d("Axis",m_pAxis);
}

void CIrrOdeJointHinge::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
  CIrrOdeJoint::deserializeAttributes(in,options);
  m_pAxis=in->getAttributeAsVector3d("Axis");
}

irr::scene::ISceneNode *CIrrOdeJointHinge::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager) {
  CIrrOdeJointHinge *pRet=new CIrrOdeJointHinge(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  copyParams(pRet);
  CIrrOdeSceneNode::cloneChildren(pRet,newManager);
  return pRet;
}

void CIrrOdeJointHinge::copyParams(CIrrOdeSceneNode *pDest, bool bRecurse) {
  if (bRecurse) CIrrOdeJoint::copyParams(pDest);
  CIrrOdeJointHinge *pDst=(CIrrOdeJointHinge *)pDest;
  pDst->setHingeAxis(m_pAxis);
}

bool CIrrOdeJointHinge::onEvent(IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==eIrrOdeEventJoint) {
    CIrrOdeEventJoint *p=(CIrrOdeEventJoint *)pEvent;
    if (p->getJointEventType()==eIrrOdeEventJointHinge) {
      CIrrOdeEventJointHinge *pJh=(CIrrOdeEventJointHinge *)p;
      m_fAngle=pJh->getAngle();
      m_fAngleRate=pJh->getAngleRate();
    }
  }

  return false;
}

bool CIrrOdeJointHinge::handlesEvent(IIrrOdeEvent *pEvent) {
  return pEvent->getType()==eIrrOdeEventJoint;
}

} //namespace ode
} //namespace irr

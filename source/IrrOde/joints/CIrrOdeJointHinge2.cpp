  #include <joints/CIrrOdeJointHinge2.h>
  #include <CIrrOdeWorld.h>
  #include <IIrrOdeDevice.h>
  #include <event/CIrrOdeEventJoint.h>
  #include <event/CIrrOdeEventJointHinge2.h>

namespace irr {
namespace ode {

CIrrOdeJointHinge2::CIrrOdeJointHinge2(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                                     const irr::core::vector3df &position,const irr::core::vector3df &rotation,const irr::core::vector3df &scale) :
                                     CIrrOdeJoint(parent, mgr, id, position, rotation, scale) {
  m_pAxis1=irr::core::vector3df(0,0,1);
  m_pAxis2=irr::core::vector3df(1,0,0);
  m_fAngle1=0.0f;
  m_fAngleRate1=0.0f;
  m_fAngleRate2=0.0f;
}


CIrrOdeJointHinge2::~CIrrOdeJointHinge2() {
}

void CIrrOdeJointHinge2::OnRegisterSceneNode() {
  if (IsVisible) SceneManager->registerNodeForRendering(this);
  irr::scene::ISceneNode::OnRegisterSceneNode();
}

void CIrrOdeJointHinge2::render() {
  CIrrOdeSceneNode::render();
  #ifdef _DRAW_JOINT_INFO
    m_pVideoDriver->setMaterial(m_cMat);
    m_pVideoDriver->setTransform(irr::video::ETS_WORLD, core::matrix4());
    irr::core::vector3df axis=getAbsoluteTransformation().getRotationDegrees().rotationToDirection(m_pAxis1);
    irr::core::vector3df axisStart=getAbsolutePosition()-axis;
    irr::core::vector3df axisEnd=axisStart+10*axis;
    m_pVideoDriver->draw3DLine(axisStart,axisEnd,irr::video::SColor(128,0xFF,0,0));

    axis=getAbsoluteTransformation().getRotationDegrees().rotationToDirection(m_pAxis2);
    axisStart=getAbsolutePosition()-axis;
    axisEnd=getAbsolutePosition()+10*axis;
    m_pVideoDriver->draw3DLine(axisStart,axisEnd,irr::video::SColor(128,0XFF,0xFF,0));

    irr::core::vector3df pos1=m_pBody1?m_pBody1->getAbsolutePosition():getAbsolutePosition();
    irr::core::vector3df pos2=m_pBody2?m_pBody2->getAbsolutePosition():getAbsolutePosition();
    m_pVideoDriver->draw3DLine(pos1,getAbsolutePosition(),irr::video::SColor(128,0,0xFF,0));
    m_pVideoDriver->setTransform(irr::video::ETS_WORLD, core::matrix4());
    m_pVideoDriver->draw3DLine(pos2,getAbsolutePosition(),irr::video::SColor(128,0,0,0xFF));
  #endif
}

void CIrrOdeJointHinge2::initPhysics() {
  if (m_bPhysicsInitialized) return;
  CIrrOdeJoint::initPhysics();

  //create the joint
  m_iJointId=m_pOdeDevice->jointCreateHinge2(m_pWorld->getWorldId());

  //set the parent as body 1
  if (getParent()->getType()==IRR_ODE_BODY_ID)
    setBody1(reinterpret_cast<CIrrOdeBody *>(getParent()));

  irr::core::vector3df rot=getAbsoluteTransformation().getRotationDegrees(),
            axis1=rot.rotationToDirection(m_pAxis1),
            axis2=rot.rotationToDirection(m_pAxis2);

  m_pOdeDevice->jointAttach(m_iJointId,m_pBody1,m_pBody2);
  m_pOdeDevice->jointSetHinge2Anchor(m_iJointId,getAbsolutePosition());
  m_pOdeDevice->jointSetHinge2Axis1(m_iJointId,axis1);
  m_pOdeDevice->jointSetHinge2Axis2(m_iJointId,axis2);
  m_pOdeDevice->jointSetData(m_iJointId,this);

  for (s32 j=0; j<2; j++)
    for (s32 i=0; i<eParamEnd; i++)
      if (m_bParamUsed[i][0]) {
        #ifdef _TRACE_INIT_PHYSICS
          printf("CIrrOdeJointHinge::initPhysics: parameter %i (set %i) set: %.2f\n",i,j,m_fParam[i][0]);
        #endif
        setParameter(j,(eJointParameter)i,m_fParam[i][j]);
      }

  #ifdef _TRACE_INIT_PHYSICS
    printf("CIrrOdeJointHinge2: %i: %.2f, %.2f, %.2f\n",ID,getAbsolutePosition().X,getAbsolutePosition().Y,getAbsolutePosition().Z);
  #endif
}

s32 CIrrOdeJointHinge2::getID() const {
  return ID;
}

irr::scene::ESCENE_NODE_TYPE CIrrOdeJointHinge2::getType() const {
  return (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_JOINT_HINGE2_ID;
}

const wchar_t *CIrrOdeJointHinge2::getTypeName() {
  return IRR_ODE_JOINT_HINGE2_NAME;
}

void CIrrOdeJointHinge2::setHingeAxis1(irr::core::vector3df pAxis) {
  m_pAxis1=pAxis.normalize();
}

void CIrrOdeJointHinge2::setHingeAxis2(irr::core::vector3df pAxis) {
  m_pAxis2=pAxis.normalize();
}

irr::core::vector3df CIrrOdeJointHinge2::getHingeAxis1() {
  if (m_iJointId) m_pAxis1=m_pOdeDevice->jointGetHinge2Axis1(m_iJointId);
  return m_pAxis1;
}

irr::core::vector3df CIrrOdeJointHinge2::getHingeAxis2() {
  if (m_iJointId) m_pAxis2=m_pOdeDevice->jointGetHinge2Axis2(m_iJointId);
  return m_pAxis2;
}

irr::core::vector3df CIrrOdeJointHinge2::getAnchor1() {
  if (m_iJointId) m_pAnchor1=m_pOdeDevice->jointGetHinge2Anchor(m_iJointId);
  return m_pAnchor1;
}

irr::core::vector3df CIrrOdeJointHinge2::getAnchor2() {
  if (m_iJointId) m_pAnchor2=m_pOdeDevice->jointGetHinge2Anchor2(m_iJointId);
  return m_pAnchor2;
}

f32 CIrrOdeJointHinge2::getHingeAngle1() {
  return m_fAngle1;
}

f32 CIrrOdeJointHinge2::getHingeAngle1Rate() {
  return m_pOdeDevice->jointGetHinge2Angle1Rate(m_iJointId);
}

f32 CIrrOdeJointHinge2::getHingeAngle2Rate() {
  return m_pOdeDevice->jointGetHinge2Angle2Rate(m_iJointId);
}

u16 CIrrOdeJointHinge2::numParamGroups() const {
  return 2;
}

void CIrrOdeJointHinge2::setParameter(u16 iGroup, eJointParameter iParam, f32 fValue) {
  #ifdef _TRACE_INIT_PHYSICS
    printf("CIrrOdeJointHinge2::setParameter: %i .. %i .. %.2f\n",iGroup,iParam,fValue);
  #endif
  CIrrOdeJoint::setParameter(iGroup,iParam,fValue);

  if (m_iJointId) m_pOdeDevice->jointSetHinge2Param(m_iJointId,iGroup,iParam,fValue);
}

f32 CIrrOdeJointHinge2::getParameter(u16 iGroup, eJointParameter iParam) {
  return m_iJointId?m_pOdeDevice->jointGetHinge2Param(m_iJointId,iGroup,iParam):0.0f;
}

void CIrrOdeJointHinge2::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
  CIrrOdeJoint::serializeAttributes(out,options);
  out->addVector3d("Axis1",m_pAxis1);
  out->addVector3d("Axis2",m_pAxis2);
}

void CIrrOdeJointHinge2::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
  CIrrOdeJoint::deserializeAttributes(in,options);
  m_pAxis1=in->getAttributeAsVector3d("Axis1");
  m_pAxis2=in->getAttributeAsVector3d("Axis2");
}

irr::scene::ISceneNode *CIrrOdeJointHinge2::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager) {
  CIrrOdeJointHinge2 *pRet=new CIrrOdeJointHinge2(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  copyParams(pRet);
  CIrrOdeSceneNode::cloneChildren(pRet,newManager);
  return pRet;
}

void CIrrOdeJointHinge2::copyParams(CIrrOdeSceneNode *pDest, bool bRecurse) {
  if (bRecurse) CIrrOdeJoint::copyParams(pDest);
  CIrrOdeJointHinge2 *pDst=(CIrrOdeJointHinge2 *)pDest;
  pDst->setHingeAxis1(m_pAxis1);
  pDst->setHingeAxis2(m_pAxis2);
}

bool CIrrOdeJointHinge2::onEvent(IIrrOdeEvent *pEvent) {
  if (pEvent->getType()==eIrrOdeEventJoint) {
    CIrrOdeEventJoint *p=(CIrrOdeEventJoint *)pEvent;
    if (p->getJointEventType()==eIrrOdeEventJointHinge2) {
      CIrrOdeEventJointHinge2 *pEvt=(CIrrOdeEventJointHinge2 *)p;
      m_fAngle1=pEvt->getAngle1();
      m_fAngleRate1=pEvt->getAngleRate1();
      m_fAngleRate2=pEvt->getAngleRate2();
    }
  }

  return false;
}

bool CIrrOdeJointHinge2::handlesEvent(IIrrOdeEvent *pEvent) {
  return pEvent->getType()==eIrrOdeEventJoint;
}


} //namespace ode
} //namespace irr

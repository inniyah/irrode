  #include <motors/CIrrOdeServo.h>
  #include <joints/CIrrOdeJoint.h>
  #define _USE_MATH_DEFINES
  #include <math.h>
  #undef _USE_MATH_DEFINES

namespace irr {
namespace ode {

CIrrOdeServo::CIrrOdeServo(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                           const irr::core::vector3df &position,
		                       const irr::core::vector3df &rotation,
          		             const irr::core::vector3df &scale) : IIrrOdeMotor(parent, mgr, id, position, rotation, scale) {

  m_iAxis=0;
  m_fServoPos=0;
  m_pJoint=NULL;
}

CIrrOdeServo::~CIrrOdeServo() {
}

void CIrrOdeServo::setAxis(int iAxis) {
  m_iAxis=iAxis;
}

void CIrrOdeServo::setServoPos(f32 fPos) {
  while (fPos> 180) fPos-=180;
  while (fPos<-180) fPos+=180;

  m_fServoPos=fPos;

  if (m_bPhysicsInitialized) {
    m_pJoint->setParameter(m_iAxis,CIrrOdeJoint::eLoStop,(f32)(m_fServoPos*M_PI/180));
    m_pJoint->setParameter(m_iAxis,CIrrOdeJoint::eHiStop,(f32)(m_fServoPos*M_PI/180));
    m_pJoint->setParameter(m_iAxis,CIrrOdeJoint::eLoStop,(f32)(m_fServoPos*M_PI/180));

    m_pJoint->activateBodies();
  }
}

u32 CIrrOdeServo::getAxis() {
  return m_iAxis;
}

f32 CIrrOdeServo::getServoPos() {
  return m_fServoPos;
}

void CIrrOdeServo::setJoint(CIrrOdeJoint *pJoint) {
  m_pJoint=pJoint;
}

CIrrOdeJoint *CIrrOdeServo::getJoint() {
  return m_pJoint;
}

void CIrrOdeServo::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
  CIrrOdeSceneNode::serializeAttributes(out,options);

  out->addInt("Axis",m_iAxis);
  out->addFloat("ServoPos",m_fServoPos);
}

void CIrrOdeServo::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
  CIrrOdeSceneNode::deserializeAttributes(in,options);

  m_iAxis=in->getAttributeAsInt("Axis");
  m_fServoPos=in->getAttributeAsFloat("ServoPos");
}

void CIrrOdeServo::render() {
  CIrrOdeSceneNode::render();
  #ifdef _DRAW_BOUNDING_BOXES
    m_pVideoDriver->setTransform(irr::video::ETS_WORLD,AbsoluteTransformation);
    m_pVideoDriver->setMaterial(m_cMat);
    m_pVideoDriver->draw3DBox(m_cBoundingBox,irr::video::SColor(100,0x40,0x40,0x40));
  #endif
}

void CIrrOdeServo::OnRegisterSceneNode() {
  if (IsVisible) SceneManager->registerNodeForRendering(this);
  irr::scene::ISceneNode::OnRegisterSceneNode();
}

void CIrrOdeServo::initPhysics() {
  if (m_bPhysicsInitialized) return;
  CIrrOdeSceneNode::initPhysics();
  #ifdef _TRACE_INIT_PHYSICS
    printf("CIrrOdeServo::initPhysics (%i .. %.2f)\n",m_iAxis,m_fServoPos);
  #endif

  #ifdef _DRAW_BOUNDING_BOXES
    //m_pBoundingBox=&(getParent()->getBoundingBox());
  #endif

  m_pJoint->setParameter(m_iAxis,CIrrOdeJoint::eLoStop,m_fServoPos);
  m_pJoint->setParameter(m_iAxis,CIrrOdeJoint::eHiStop,m_fServoPos);
  m_pJoint->setParameter(m_iAxis,CIrrOdeJoint::eLoStop,m_fServoPos);
}

s32 CIrrOdeServo::getID() const {
  return ID;
}

irr::scene::ESCENE_NODE_TYPE CIrrOdeServo::getType() const {
  return (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_SERVO_ID;
}

const wchar_t *CIrrOdeServo::getTypeName() {
  return IRR_ODE_SERVO_NAME;
}

irr::scene::ISceneNode *CIrrOdeServo::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager) {
  CIrrOdeServo *pRet=new CIrrOdeServo(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  copyParams(pRet);
  CIrrOdeSceneNode::cloneChildren(pRet,newManager);
  return pRet;
}

void CIrrOdeServo::copyParams(CIrrOdeSceneNode *pDest, bool bRecurse) {
  if (bRecurse) CIrrOdeSceneNode::copyParams(pDest);
  pDest->setName(getName());
  CIrrOdeServo *pDst=(CIrrOdeServo *)pDest;
  pDst->setAxis(m_iAxis);
  pDst->setServoPos(m_fServoPos);
}

void CIrrOdeServo::setPosition(const irr::core::vector3df &newpos) {
	irr::scene::ISceneNode::setPosition(irr::core::vector3df(0.0f,0.0f,0.0f));
}

} //namespace ode
} //namespace irr

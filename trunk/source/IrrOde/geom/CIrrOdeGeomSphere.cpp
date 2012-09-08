  #include <geom/CIrrOdeGeomSphere.h>
  #include <CIrrOdeSpace.h>
  #include <CIrrOdeWorld.h>
  #include <CIrrOdeSpace.h>
  #include <CIrrOdeBody.h>
  #include <IIrrOdeDevice.h>

namespace irr {
namespace ode {

CIrrOdeGeomSphere::CIrrOdeGeomSphere(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                                     const irr::core::vector3df &position, const irr::core::vector3df &rotation, const irr::core::vector3df &scale) :
                                     CIrrOdeGeom(parent, mgr, id, position, rotation, scale) {

  #ifdef _TRACE_CONSTRUCTOR_DESTRUCTOR
    printf("CIrrOdeGeomSphere constructor\n");
  #endif

  m_fMass=1.0f;
  m_iGeomId=0;
  m_fRadius=0.0f;
}

CIrrOdeGeomSphere::~CIrrOdeGeomSphere() {
  #ifdef _TRACE_CONSTRUCTOR_DESTRUCTOR
    printf("CIrrOdeGeomSphere destructor\n");
  #endif
}

void CIrrOdeGeomSphere::OnRegisterSceneNode() {
  if (IsVisible) SceneManager->registerNodeForRendering(this);
  irr::scene::ISceneNode::OnRegisterSceneNode();
}

void CIrrOdeGeomSphere::render() {
  CIrrOdeSceneNode::render();
  #ifdef _DRAW_BOUNDING_BOXES
    m_pVideoDriver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
    m_pVideoDriver->setMaterial(m_cMat);
    m_pVideoDriver->draw3DBox(m_cBoundingBox,irr::video::SColor(100,0x40,0x40,0x40));
  #endif
}

void CIrrOdeGeomSphere::initPhysics() {
  if (m_bPhysicsInitialized) return;

  updateAbsolutePosition();
  m_pSpace=reinterpret_cast<CIrrOdeSpace *>(getAncestorOfType((irr::scene::ESCENE_NODE_TYPE)IRR_ODE_SPACE_ID));
  if (!m_pSpace) m_pSpace=m_pWorld->getSpace();

  if (m_fRadius==0.0f) {
    irr::scene::IMesh *pMesh=NULL;

    irr::scene::ISceneNode *pNode=getParent();

    switch (pNode->getType()) {
      case irr::scene::ESNT_MESH :
        pMesh=(reinterpret_cast<irr::scene::IMeshSceneNode *>(pNode))->getMesh();
        break;

      case irr::scene::ESNT_ANIMATED_MESH:
        pMesh=(reinterpret_cast<irr::scene::IAnimatedMeshSceneNode *>(pNode))->getMesh();
        break;

      default:
        printf("Non supported scene node type detected!\n");
        return;
    }

    m_fRadius=pMesh->getBoundingBox().getExtent().X*getParent()->getScale().X/2;

    #ifdef _TRACE_INIT_PHYSICS
      printf("CIrrOdeGeomSphere::initPhysics: getting radius from parent node\n");
      printf("CIrrOdeGeomSphere::initPhysics: radius=%.2f\n",m_fRadius);
    #endif
  }

  m_iGeomId=m_pOdeDevice->geomCreateSphere(m_pSpace->getSpaceId(),m_fRadius);

  if (m_iGeomId) {
    m_pOdeDevice->geomSetPosition(m_iGeomId,getAbsolutePosition());

    if (m_pBody) {
      if (m_fMass) {
        m_pOdeDevice->massSetZero(m_iMass);
        m_pOdeDevice->massSetSphereTotal(m_iMass,m_fMass,m_fRadius);
        m_pBody->addMass(m_iMass);
        //m_pOdeDevice->bodySetMass(m_pBody->getBodyId(),m_iMass);
      }
      setBody(m_pBody);
    }
    else {
      //m_pOdeDevice->geomSetCategoryBits(m_iGeomId,1);
      m_pOdeDevice->geomSetCollisionBits(m_iGeomId,0);
    }
  }
  #ifdef _TRACE_INIT_PHYSICS
    printf("CIrrOdeGeomSphere::initPhysics: %i\n",(int)m_iGeomId);
    printf("CIrrOdeGeomSphere radius: %.2f, mass: %.2f\n",m_fRadius,m_fMass);
  #endif

  #ifdef _DRAW_BOUNDING_BOXES
    m_cBoundingBox=getParent()->getBoundingBox();
  #endif

  m_pOdeDevice->geomSetData(m_iGeomId,this);
  CIrrOdeGeom::initPhysics();
}

s32 CIrrOdeGeomSphere::getID() const {
  return ID;
}

irr::scene::ESCENE_NODE_TYPE CIrrOdeGeomSphere::getType() const {
  return (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_GEOM_SPHERE_ID;
}

const wchar_t *CIrrOdeGeomSphere::getTypeName() {
  return IRR_ODE_GEOM_SPHERE_NAME;
}

void CIrrOdeGeomSphere::setMassTotal(f32 fMass) {
  m_fMass=fMass;
}

void CIrrOdeGeomSphere::setRadius(f32 fRadius) {
  m_fRadius=fRadius;
}

f32 CIrrOdeGeomSphere::getRadius() {
  return m_fRadius;
}

void CIrrOdeGeomSphere::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
  CIrrOdeGeom::serializeAttributes(out,options);
  out->addFloat("Radius",m_fRadius);
}

void CIrrOdeGeomSphere::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
  CIrrOdeGeom::deserializeAttributes(in,options);
  m_fRadius=in->getAttributeAsFloat("Radius");
}

irr::scene::ISceneNode *CIrrOdeGeomSphere::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager) {
  CIrrOdeGeomSphere *pRet=new CIrrOdeGeomSphere(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  copyParams(pRet);
  CIrrOdeSceneNode::cloneChildren(pRet,newManager);
  return pRet;
}

void CIrrOdeGeomSphere::copyParams(CIrrOdeSceneNode *pDest, bool bRecurse) {
  if (bRecurse) CIrrOdeGeom::copyParams(pDest);
  CIrrOdeGeomSphere *pDst=(CIrrOdeGeomSphere *)pDest;
  pDst->setRadius(m_fRadius);
}

} //namespace ode
} //namespace irr

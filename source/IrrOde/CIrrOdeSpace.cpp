  #include <irrlicht.h>
  #include <CIrrOdeSpace.h>
  #include <IIrrOdeDevice.h>
  #include <CIrrOdeWorld.h>

namespace irr {
namespace ode {

const c8 *const g_sSpaceTypes[]={
  "OdeSimpleSpace",
  "OdeHashSpace",
  "OdeQuadtreeSpace",
  NULL
};

CIrrOdeSpace::CIrrOdeSpace(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                           const irr::core::vector3df &position, const irr::core::vector3df &rotation, const irr::core::vector3df &scale) :
                           CIrrOdeSceneNode(parent, mgr, id, position, rotation, scale) {

  #ifdef _TRACE_CONSTRUCTOR_DESTRUCTOR
    printf("CIrrOdeBody contructor\n");
  #endif

  m_iSpaceId=0;
  m_iType=eIrrOdeSpaceSimple;
  m_pParentSpace=reinterpret_cast<CIrrOdeSpace *>(getAncestorOfType((irr::scene::ESCENE_NODE_TYPE)IRR_ODE_SPACE_ID));
  m_pWorld=reinterpret_cast<CIrrOdeWorld *>(getAncestorOfType((irr::scene::ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID));
  if (m_pWorld) m_pWorld->addSpace(this);

  if (!m_pParentSpace) if (m_pWorld && m_pWorld->getSpace()!=this) m_pParentSpace=m_pWorld->getSpace();

  m_cCenter=irr::core::vector3df(0,0,0);
  m_cExtents=irr::core::vector3df(0,0,0);
  m_iDepth=0;
}

CIrrOdeSpace::~CIrrOdeSpace() {
  if (m_iSpaceId) m_pOdeDevice->spaceDestroy(m_iSpaceId);
}

u32 CIrrOdeSpace::getSpaceId() {
  return m_iSpaceId;
}

void CIrrOdeSpace::initPhysics() {
  if (m_bPhysicsInitialized) return;
  CIrrOdeSceneNode::initPhysics();

  if (!m_pParentSpace) if (m_pWorld && m_pWorld->getSpace()!=this) m_pParentSpace=m_pWorld->getSpace();

  switch (m_iType) {
    case eIrrOdeSpaceSimple:
      #ifdef _TRACE_INIT_PHYSICS
        printf("**** simple space: \"%s\"\n",getName()!=NULL?getName():"<NULL>");
      #endif
      m_iSpaceId=m_pOdeDevice->spaceCreateSimple(m_pParentSpace?m_pParentSpace->getSpaceId():0);
      break;

    case eIrrOdeSpaceHash:
      #ifdef _TRACE_INIT_PHYSICS
        printf("**** hash space: \"%s\"\n",getName()!=NULL?getName():"<NULL>");
      #endif
      m_iSpaceId=m_pOdeDevice->spaceCreateHash(m_pParentSpace?m_pParentSpace->getSpaceId():0);
      break;

    case eIrrOdeSpaceQuadTree:
      #ifdef _TRACE_INIT_PHYSICS
        printf("**** quadtree space: \"%s\"\n",getName()!=NULL?getName():"<NULL>");
      #endif
      m_iSpaceId=m_pOdeDevice->spaceCreateQuadTree(m_pParentSpace?m_pParentSpace->getSpaceId():0,m_cCenter,m_cExtents,m_iDepth);
      break;

    default:
      break;
  }
}

void CIrrOdeSpace::setSpaceType(_IRR_ODE_SPACE_TYPE iType) {
  m_iType=iType;
}

_IRR_ODE_SPACE_TYPE CIrrOdeSpace::getSpaceType() {
  return m_iType;
}

void CIrrOdeSpace::setQuadTreeParams(irr::core::vector3df cCenter, irr::core::vector3df cExtents, s32 iDepth){
  m_cCenter=cCenter;
  m_cExtents=cExtents;
  m_iDepth=iDepth;
}

s32 CIrrOdeSpace::getID() {
  return ID;
}

void CIrrOdeSpace::render() {
  CIrrOdeSceneNode::render();
}

void CIrrOdeSpace::OnRegisterSceneNode() {
  if (IsVisible) SceneManager->registerNodeForRendering(this);
  irr::scene::ISceneNode::OnRegisterSceneNode();
}

irr::scene::ESCENE_NODE_TYPE CIrrOdeSpace::getType() const {
  return (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_SPACE_ID;
}

const wchar_t *CIrrOdeSpace::getTypeName() {
  return IRR_ODE_SPACE_NAME;
}

void CIrrOdeSpace::setType(_IRR_ODE_SPACE_TYPE iType) {
  m_iType=iType;
}

_IRR_ODE_SPACE_TYPE CIrrOdeSpace::getType() {
  return m_iType;
}

void CIrrOdeSpace::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
  CIrrOdeSceneNode::serializeAttributes(out,options);

  out->addEnum("SpaceType",m_iType,g_sSpaceTypes);

  out->addInt("Depth"    ,m_iDepth);

  out->addVector3d("Center" ,m_cCenter );
  out->addVector3d("Extents",m_cExtents);
}

void CIrrOdeSpace::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
  CIrrOdeSceneNode::deserializeAttributes(in,options);

  m_iType =(_IRR_ODE_SPACE_TYPE)in->getAttributeAsEnumeration("SpaceType",g_sSpaceTypes);//in->getAttributeAsInt("SpaceType");
  m_iDepth=in->getAttributeAsInt("Depth"    );

  m_cCenter =in->getAttributeAsVector3d("Center" );
  m_cExtents=in->getAttributeAsVector3d("Extents");
}

CIrrOdeSpace *CIrrOdeSpace::getParentSpace() {
  return m_pParentSpace;
}

irr::core::vector3df &CIrrOdeSpace::getCenter() {
  return m_cCenter;
}

irr::core::vector3df &CIrrOdeSpace::getExtents() {
  return m_cExtents;
}

s32 CIrrOdeSpace::getDepth() {
  return m_iDepth;
}

irr::scene::ISceneNode *CIrrOdeSpace::clone(irr::scene::ISceneNode* newParent, irr::scene::ISceneManager* newManager) {
  CIrrOdeSpace *pRet=new CIrrOdeSpace(newParent?newParent:getParent(),newManager?newManager:m_pSceneManager);
  copyParams(pRet);
  CIrrOdeSceneNode::cloneChildren(pRet,newManager);
  return pRet;
}

void CIrrOdeSpace::copyParams(CIrrOdeSceneNode *pDest, bool bRecurse) {
  #ifdef _TRACE_COPY_PARAMS
    printf("CIrrOdeBody copyParams\n");
  #endif
  if (bRecurse) CIrrOdeSceneNode::copyParams(pDest);

  CIrrOdeSpace *pDst=(CIrrOdeSpace *)pDest;
  m_iSpaceId=0;
  pDst->setSpaceType(m_iType);
  pDst->setQuadTreeParams(m_cCenter,m_cExtents,m_iDepth);
}

void CIrrOdeSpace::removeFromPhysics() {
  CIrrOdeSceneNode::removeFromPhysics();
  if (m_iSpaceId) {
    m_pOdeDevice->spaceDestroy(m_iSpaceId);
    m_iSpaceId=0;
  }
}

} //namespace ode
} //namespace irr

#include <CIrrOdeSceneNode.h>
#include <CIrrOdeManager.h>
#include <CIrrOdeWorld.h>

#ifdef _IRREDIT_PLUGIN
  #include <windows.h>
#endif

namespace irr {
namespace ode {

static s32 g_iNextOdeNodeId = 20000;       /**< the next id for ODE scene nodes */

CIrrOdeSceneNode::CIrrOdeSceneNode(irr::scene::ISceneNode *parent,irr::scene::ISceneManager *mgr,s32 id,
                                   const irr::core::vector3df &position, const irr::core::vector3df &rotation, const irr::core::vector3df &scale) :
                                   irr::scene::ISceneNode(parent, mgr, id, position, rotation, scale) {

  m_cMat.setFlag(irr::video::EMF_LIGHTING,false);

  m_pSceneManager=mgr;
  m_pVideoDriver=mgr->getVideoDriver();
  m_bPhysicsInitialized=false;
  m_sOdeClassName="";
  m_pSelector=NULL;
  m_pOdeManager=CIrrOdeManager::getSharedInstance();
  m_pOdeDevice=m_pOdeManager->getOdeDevice();

  m_pWorld=reinterpret_cast<CIrrOdeWorld *>(getAncestorOfType((irr::scene::ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID));
  if (m_pWorld)
    m_pWorld->addOdeSceneNode(this);
  else
    if (this->getType()==(irr::scene::ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID) {
      CIrrOdeWorld *p = reinterpret_cast<CIrrOdeWorld *>(this);
      p->addOdeSceneNode(this);
    }
    else printf("no world!\n");

  m_iIdent=-1;
  m_pUserData=NULL;
  #ifdef _IRREDIT_PLUGIN
    m_bDrawEditorMesh=true;
  	GetModuleFileName(NULL,m_sResources,1024);
	  if (strstr(m_sResources,"irrEdit.exe"))
      *strstr(m_sResources,"irrEdit.exe")='\0';
    else
      if (strrchr(m_sResources,'\\'))
        *strrchr(m_sResources,'\\')='\0';

  	strcat(m_sResources,"\\resources\\");
	  c8 sFileName[1024];
	  sprintf(sFileName,"%sIrrOde.3ds",m_sResources);
    m_pMesh=mgr->getMesh(sFileName);

    this->setID(m_pOdeManager->getNextId());

    if (m_pWorld!=NULL) {
      m_pWorld->addOdeChildNode(this);
    }
  #endif
}

CIrrOdeSceneNode::~CIrrOdeSceneNode() {
  if (m_pWorld) m_pWorld->removeOdeSceneNode(this);
}

/**
 * Retrieve the scene node's bounding box
 * @return the scene node's bounding box
 */
const irr::core::aabbox3df &CIrrOdeSceneNode::getBoundingBox() const {
  return m_cBoundingBox;
}

/**
 * Retreive the first ancestor of a given node type
 * @param iType the node type of the ancestor
 * @return the first ancestor of the given type or "NULL" if no node of the type was found
 */
irr::scene::ISceneNode *CIrrOdeSceneNode::getAncestorOfType(irr::scene::ESCENE_NODE_TYPE iType) {
  irr::scene::ISceneNode *pNode=this->getParent();
  while (pNode!=NULL && pNode->getType()!=iType) pNode=pNode->getParent();

  if (pNode == NULL) {
    //pNode = findFirstNodeOfType(iType, m_pSceneManager->getRootSceneNode());
  }

  return pNode;
}

/**
 * Is the physics already initialized?
 * @return "true" if initPhysics was already called, "false" otherwise
 */
bool CIrrOdeSceneNode::physicsInitialized() {
  return m_bPhysicsInitialized;
}

/**
 * convert a wchar_t string to a c8 string
 * @param sName the wchar_t string
 * @return the c8 string
 */
const c8 *CIrrOdeSceneNode::nodeNameToC8(const wchar_t *sName) {
  static irr::core::stringc sC8Name;
  sC8Name=irr::core::stringc(sName);
  return sC8Name.c_str();
}

void CIrrOdeSceneNode::serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options) const {
  irr::scene::ISceneNode::serializeAttributes(out,options);
  out->addString("OdeClassName",m_sOdeClassName.c_str());
  out->addInt("Ident",m_iIdent);
}

void CIrrOdeSceneNode::deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options) {
  irr::scene::ISceneNode::deserializeAttributes(in,options);
  m_sOdeClassName=in->getAttributeAsString("OdeClassName");
  m_iIdent=in->getAttributeAsInt("Ident");
  updateNextId(getID());
}

/**
 * Get the irrOde classname of this node
 * @return the ODE classname of this node
 */
irr::core::stringc CIrrOdeSceneNode::getOdeClassname() {
  return m_sOdeClassName;
}

/**
 * Change the irrOde classname of this node
 * @param sClassName the new ODE classname of this node
 */
void CIrrOdeSceneNode::setOdeClassname(irr::core::stringc sClassName) {
  m_sOdeClassName=sClassName;
}

/**
 * Get the irrOde identifier
 * @return the irrOde identifier
 */
int CIrrOdeSceneNode::getIdent() {
  return m_iIdent;
}

/**
 * Set the irrOde identifier
 * @return the irrOde identifier
 */
void CIrrOdeSceneNode::setIdent(int iIdent) {
  m_iIdent=iIdent;
}

u32 CIrrOdeSceneNode::getMaterialCount() {
  return 1;
}

irr::video::SMaterial &CIrrOdeSceneNode::getMaterial(u32 iIdx) {
  return m_cMat;
}

void CIrrOdeSceneNode::render() {
  #ifdef _IRREDIT_PLUGIN
    if (m_pMesh && m_pMesh->getMesh(0) && m_pMesh->getMesh(0)->getMeshBuffer(0) && m_bDrawEditorMesh) {
      irr::core::matrix4 tmpMatrix;
	  tmpMatrix.setTranslation(AbsoluteTransformation.getTranslation());
      m_pVideoDriver->setMaterial(m_cMat);
      m_pVideoDriver->setTransform(irr::video::ETS_WORLD,tmpMatrix);
      m_pSceneManager->getVideoDriver()->drawMeshBuffer(m_pMesh->getMesh(0)->getMeshBuffer(0));
    }
  #endif
}

/**
 * Copy the parameters to another IrrOde scene node
 * @param pDest the receiver of the parameters
 * @param bRecurse switch to tell the node wether or not the copying should be continued in the object hierarchy
 */
void CIrrOdeSceneNode::copyParams(CIrrOdeSceneNode *pDest, bool bRecurse) {
  pDest->setOdeClassname(m_sOdeClassName);
  pDest->setPosition(getPosition());
  pDest->setScale(getScale());
  pDest->setRotation(getRotation());
}

void CIrrOdeSceneNode::addChild (irr::scene::ISceneNode *child) {
  irr::scene::ISceneNode::addChild(child);
  irr::core::vector3df pos=child->getPosition();
  m_cBoundingBox.addInternalPoint(pos);
}

void CIrrOdeSceneNode::remove() {
  irr::scene::ISceneNode::remove();
  if (this->getReferenceCount()==1) removeFromPhysics();
  if (m_pWorld) m_pWorld->removeOdeSceneNode(this);
}

irr::scene::ISceneManager *CIrrOdeSceneNode::getSceneManager() {
  return m_pSceneManager;
}

irr::scene::ITriangleSelector *CIrrOdeSceneNode::getTriangleSelector() {
  return m_pSelector;
}

void CIrrOdeSceneNode::setUserData(void *p) {
  m_pUserData=p;
}

void *CIrrOdeSceneNode::getUserData() {
  return m_pUserData;
}

#ifdef _IRREDIT_PLUGIN
  void CIrrOdeSceneNode::setDrawEditorMesh(bool b) { m_bDrawEditorMesh=b; }
#endif

void CIrrOdeSceneNode::setParent(irr::scene::ISceneNode *newParent) {
  CIrrOdeWorld *pWorld=reinterpret_cast<CIrrOdeWorld *>(getAncestorOfType((irr::scene::ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID));
  #ifdef _IRREDIT_PLUGIN
    if (pWorld!=NULL) {
      pWorld->removeOdeChildNode(this);
    }
  #endif

  if (m_pWorld)
    m_pWorld->removeOdeSceneNode(this);
  else
    if (this->getType() == (irr::scene::ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID) {
      CIrrOdeWorld *p = reinterpret_cast<CIrrOdeWorld *>(this);
      p->removeOdeSceneNode(this);
    }

  irr::scene::ISceneNode::setParent(newParent);

  pWorld=reinterpret_cast<CIrrOdeWorld *>(getAncestorOfType((irr::scene::ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID));
  if (pWorld!=NULL)
    m_pWorld->addOdeSceneNode(this);
  else
    if (this->getType()==(irr::scene::ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID) {
      CIrrOdeWorld *p = reinterpret_cast<CIrrOdeWorld *>(this);
      p->addOdeSceneNode(this);
    }

  #ifdef _IRREDIT_PLUGIN
    if (pWorld!=NULL) {
      pWorld->addOdeChildNode(this);
    }
  #endif
}

void CIrrOdeSceneNode::initPhysics() {
  m_bPhysicsInitialized=true;
  if (m_pWorld)
    m_pWorld->sceneNodeInitialized(this);
  else
    if (getType() == (irr::scene::ESCENE_NODE_TYPE)(IRR_ODE_WORLD_ID)) {
      CIrrOdeWorld *p = reinterpret_cast<CIrrOdeWorld *>(this);
      p->sceneNodeInitialized(this);
    }
}

void CIrrOdeSceneNode::removeFromPhysics() {
  if (m_pWorld)
    m_pWorld->removeOdeSceneNode(this);
  else
    if (getType() == (irr::scene::ESCENE_NODE_TYPE)(IRR_ODE_WORLD_ID)) {
      CIrrOdeWorld *p = reinterpret_cast<CIrrOdeWorld *>(this);
      p->removeOdeSceneNode(this);
    }
}

void CIrrOdeSceneNode::cloneChildren(irr::scene::ISceneNode *pNewParent, irr::scene::ISceneManager *pNewManager) {
  irr::core::list<irr::scene::ISceneNode *> children=getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++)
    (*it)->clone(pNewParent,pNewManager);
}

irr::scene::ISceneNode *CIrrOdeSceneNode::getChildByName(const c8 *sName, irr::scene::ISceneNode *pParent) {
  if (!strcmp(sName,pParent->getName())) return pParent;

  irr::core::list<irr::scene::ISceneNode *> children=pParent->getChildren();
  irr::core::list<irr::scene::ISceneNode *>::Iterator it;

  for (it=children.begin(); it!=children.end(); it++) {
    irr::scene::ISceneNode *pRet=getChildByName(sName,*it);
    if (pRet!=NULL) return pRet;
  }

  return NULL;
}

static irr::core::array<irr::core::stringc> g_aParamNames;
static irr::core::array<const c8 *> g_aC8ParamNames;
static irr::core::list<irr::ode::CIrrOdeSurfaceParameters *> g_lParamList;      /**< global irr::core::list of surface parameters */
static CIrrOdeSurfaceParameters g_cNullSurface;

void CIrrOdeSceneNode::updateParameterList() const {
  g_aParamNames.clear();
  g_aC8ParamNames.clear();

  irr::core::list<CIrrOdeSurfaceParameters *>::Iterator it;
  for (it=g_lParamList.begin(); it!=g_lParamList.end(); it++) {
    irr::core::stringc s=((*it)->getName());
    if (s!="") {
      bool bAdd=true;
      for (u32 i=0; i<g_aParamNames.size() && bAdd; i++) if (g_aParamNames[i]==s) bAdd=false;
      if (bAdd) g_aParamNames.push_back(s);
    }
  }

  for (u32 i=0; i<g_aParamNames.size(); i++) g_aC8ParamNames.push_back(g_aParamNames[i].c_str());
  g_aC8ParamNames.push_back(NULL);
}

void CIrrOdeSceneNode::addParameter(CIrrOdeSurfaceParameters *pParam) const {
  g_lParamList.push_back(pParam);
  updateParameterList();
}

void CIrrOdeSceneNode::removeParameter(CIrrOdeSurfaceParameters *pParam) const {
  irr::core::list<CIrrOdeSurfaceParameters *>::Iterator it;
  for (it=g_lParamList.begin(); it!=g_lParamList.end(); it++)
    if ((*it)==pParam) {
      g_lParamList.erase(it);
      updateParameterList();
      return;
    }
}

CIrrOdeSurfaceParameters *CIrrOdeSceneNode::getParameter(irr::core::stringw sName) const {
  irr::core::list<CIrrOdeSurfaceParameters *>::Iterator it;

  #ifdef _TRACE_INIT_PHYSICS
    printf("\tsearching for surface parameters \"%s\" ... ",irr::core::stringc(sName).c_str());
  #endif

  if (sName=="") {
    #ifdef _TRACE_INIT_PHYSICS
      printf("no search name defined!\n");
    #endif
    return &g_cNullSurface;
  }

  for (it=g_lParamList.begin(); it!=g_lParamList.end(); it++)
    if (sName==irr::core::stringw((*it)->getName())) {
      #ifdef _TRACE_INIT_PHYSICS
        printf("OK\n");
      #endif
      return *it;
    }

  #ifdef _TRACE_INIT_PHYSICS
    printf("not found!\n");
  #endif
  return &g_cNullSurface;
}

const c8 *const *CIrrOdeSceneNode::getParameterList() const {
  return g_aC8ParamNames.const_pointer();
}

void CIrrOdeSceneNode::clearParameterList() const {
  g_lParamList.clear();
}

s32 CIrrOdeSceneNode::getNextId() {
  return ++g_iNextOdeNodeId;
}

void CIrrOdeSceneNode::updateNextId(s32 iId) {
  if (iId>g_iNextOdeNodeId) g_iNextOdeNodeId=iId;
}

} //namespace ode
} //namespace irr

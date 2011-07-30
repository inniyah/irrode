  #include <geom/CIrrOdeGeom.h>
  #include <IIrrOdeDevice.h>
  #include <CIrrOdeBody.h>
  #include <CIrrOdeWorld.h>
  #include <CIrrOdeManager.h>

namespace irr {
namespace ode {

CIrrOdeGeom::CIrrOdeGeom(ISceneNode *parent,ISceneManager *mgr,s32 id,
                         const vector3df &position, const vector3df &rotation, const vector3df &scale) :
                         CIrrOdeSceneNode(parent, mgr, id, vector3df(0.0f,0.0f,0.0f), rotation, scale) {

  #ifdef _TRACE_CONSTRUCTOR_DESTRUCTOR
    printf("CIrrOdeGeom constructor\n");
  #endif

  m_pSpace=NULL;

  m_iGeomId=0;
	m_iCollisionGroup=0;

  m_cCenterOfGravity=vector3df(0.0f,0.0f,0.0f);
  m_cInertia1=vector3df(0.0f,0.0f,0.0f);
  m_cInertia2=vector3df(0.0f,0.0f,0.0f);
  m_cMassTranslate=vector3df(0.0f,0.0f,0.0f);
  m_cOffsetPos=vector3df(0.0f,0.0f,0.0f);
  m_cOffsetRot=vector3df(0.0f,0.0f,0.0f);

  m_iMass=m_pOdeDevice->massCreate();
  m_pOdeDevice->massSetZero(m_iMass);

	m_pBody=reinterpret_cast<CIrrOdeBody *>(getAncestorOfType((ESCENE_NODE_TYPE)IRR_ODE_BODY_ID));

  m_aParamNames.push_back("DefaultSurface");

  m_bCollide=true;

  #ifndef _IRREDIT_PLUGIN
    if (m_pBody) {
      m_pBody->addGeom(this);
      m_pWorld=reinterpret_cast<CIrrOdeWorld *>(m_pBody->getWorld());
    }
    else {
      m_pWorld=reinterpret_cast<CIrrOdeWorld *>(getAncestorOfType((ESCENE_NODE_TYPE)IRR_ODE_WORLD_ID));
      if (m_pWorld) m_pWorld->addGeom(this);
    }
    #ifdef _TRACE_CONSTRUCTOR_DESTRUCTOR
      printf("CIrrOdeGeom: body=%i\n",m_pBody?m_pBody->getID():-2);
    #endif
  #endif
}

CIrrOdeGeom::~CIrrOdeGeom() {
  #ifdef _TRACE_CONSTRUCTOR_DESTRUCTOR
    printf("CIrrOdeGeom destructor\n");
  #endif

  if (m_pBody) {
    #ifdef _TRACE_CONSTRUCTOR_DESTRUCTOR
      printf("CIrrOdeGeom destructor: removing geom from body\n");
    #endif
    m_pBody->removeGeom(this);
    m_pOdeDevice->geomSetBody(m_iGeomId,NULL);
  }

  if (m_iGeomId) m_pOdeDevice->geomDestroy(m_iGeomId);
}

u32 CIrrOdeGeom::getGeomId(u32 iNum) {
  return m_iGeomId;
}

CIrrOdeBody *CIrrOdeGeom::getBody() {
  return m_pBody;
}

void CIrrOdeGeom::setBody(CIrrOdeBody *pBody) {
  m_pBody=pBody;
  #ifdef _TRACE_INIT_PHYSICS
    printf("CIrrOdeGeom::setBody: setting body of geom %i to %i\n",m_iGeomId,(int)(pBody?pBody->getBodyId():0));
  #endif
  if (m_iGeomId) m_pOdeDevice->geomSetBody(m_iGeomId,pBody);
}

u32 CIrrOdeGeom::getSurfaceParametersCount() const {
  return m_aParamNames.size();
}

CIrrOdeSurfaceParameters *CIrrOdeGeom::getSurfaceParameters(u32 iIdx) {
  return m_aParams[0];
}

CIrrOdeWorld *CIrrOdeGeom::getWorld() {
  return m_pWorld;
}

void CIrrOdeGeom::setMassParameters(f32 fMass, vector3df c, vector3df i1, vector3df i2) {
  m_cCenterOfGravity=c;
  m_cInertia1=i1;
  m_cInertia2=i2;
  m_fMass=fMass;

  m_pOdeDevice->massSetParameters(m_iMass,fMass,c,i1,i2);
}

void CIrrOdeGeom::serializeAttributes(IAttributes* out, SAttributeReadWriteOptions* options) const {
  CIrrOdeSceneNode::serializeAttributes(out,options);

	if (!m_pBody)
    for (u32 i=0; i<this->getSurfaceParametersCount(); i++) {
      c8 s[0xFF];
      if (i==0) strcpy(s,"Surface"); else sprintf(s,"Surface_mat%i",i);
      out->addEnum(s,m_aParamNames[i].c_str(),CIrrOdeManager::getSharedInstance()->getSurfaceParameterList());
    }
  else out->addEnum("Surface",m_aParamNames[0].c_str(),CIrrOdeManager::getSharedInstance()->getSurfaceParameterList());

  out->addFloat("Mass",m_fMass);

  out->addVector3d("CenterOfGravity",m_cCenterOfGravity);
  out->addVector3d("Inertia_1"      ,m_cInertia1       );
  out->addVector3d("Inertia_2"      ,m_cInertia2       );
  out->addVector3d("MassTranslation",m_cMassTranslate  );

  out->addBool("collides",m_bCollide);
	out->addInt("collisionGroup",m_iCollisionGroup);
}

void CIrrOdeGeom::deserializeAttributes(IAttributes* in, SAttributeReadWriteOptions* options) {
  CIrrOdeSceneNode::deserializeAttributes(in,options);
  for (u32 i=0; i<getSurfaceParametersCount(); i++) {
    c8 s[0xFF];
    if (i==0) strcpy(s,"Surface"); else sprintf(s,"Surface_mat%i",i);
    m_aParamNames[i]=in->getAttributeAsEnumeration(s);
  }

  m_fMass=in->getAttributeAsFloat("Mass");

  m_cCenterOfGravity=in->getAttributeAsVector3d("CenterOfGravity");
  m_cInertia1       =in->getAttributeAsVector3d("Inertia_1"      );
  m_cInertia2       =in->getAttributeAsVector3d("Inertia_2"      );
  m_cMassTranslate  =in->getAttributeAsVector3d("MassTranslation");

  m_bCollide=!in->existsAttribute("collides") || in->getAttributeAsBool("collides");

	m_iCollisionGroup=in->getAttributeAsInt("collisionGroup");
}

void CIrrOdeGeom::setMassTranslation(vector3df pos) {
  m_cMassTranslate=pos;
  if (m_iGeomId) m_pOdeDevice->massTranslate(m_iMass,pos);
}

void CIrrOdeGeom::initPhysics() {
  #ifdef _TRACE_INIT_PHYSICS
    printf("CIrrOdeGeom::initPhysics\n");
  #endif

  if (m_pBody!=NULL) {
    if (m_cCenterOfGravity.getLength()!=0.0f || m_cInertia1.getLength()!=0.0f || m_cInertia2.getLength()!=0.0f)
      m_pOdeDevice->massSetParameters(m_iMass,m_fMass,m_cCenterOfGravity,m_cInertia1,m_cInertia2);

    if (m_cMassTranslate.getLength()!=0.0f) m_pOdeDevice->massTranslate(m_iMass,m_cMassTranslate);

    m_cOffsetPos=getParent()->getPosition();
    if (m_cOffsetPos!=vector3df(0.0f,0.0f,0.0f)) {
      #ifdef _TRACE_INIT_PHYSICS
        printf("CIrrOdeGeom::initPhysics: offset position of geom set to (%.2f, %.2f, %.2f)\n",m_cOffsetPos.X,m_cOffsetPos.Y,m_cOffsetPos.Z);
      #endif
      m_pOdeDevice->geomSetOffsetPosition(m_iGeomId,m_cOffsetPos);
      m_pOdeDevice->massTranslate(m_iMass,m_cOffsetPos);
    }

    m_cOffsetRot=getParent()->getRotation();
    if (m_cOffsetRot!=vector3df(0.0f,0.0f,0.0f)) {
      #ifdef _TRACE_INIT_PHYSICS
        printf("CIrrOdeGeom::initPhysics: offset rotation of geom set to (%.2f, %.2f, %.2f)\n",m_cOffsetRot.X,m_cOffsetRot.Y,m_cOffsetRot.Z);
      #endif
      m_pOdeDevice->geomSetOffsetQuaternion(m_iGeomId,m_cOffsetRot);
    }
  }
  else {
    vector3df rot=getParent()->getRotation();
    m_pOdeDevice->geomSetRotation(m_iGeomId,rot);
  }

  m_aParams.clear();
  for (u32 i=0; i<m_aParamNames.size(); i++) {
    CIrrOdeSurfaceParameters *p=CIrrOdeManager::getSharedInstance()->getSurfaceParameter(stringw(m_aParamNames[i]));
    m_aParams.push_back(p);
    if (p==NULL) printf("*ERROR* unable to find surface parameter \"%s\"!\n",m_aParamNames[i].c_str());
  }

  CIrrOdeSceneNode::initPhysics();
}

const c8 *CIrrOdeGeom::getSurfaceParameterName(u32 iIdx) {
  return iIdx<m_aParamNames.size()?m_aParamNames[iIdx].c_str():NULL;
}

void CIrrOdeGeom::setSurfaceParameterName(u32 iIdx, const c8 *s) {
  while (iIdx>m_aParamNames.size()) m_aParamNames.push_back("");
  m_aParamNames[iIdx]=s;
}

void CIrrOdeGeom::copyParams(CIrrOdeSceneNode *pDest, bool bRecurse) {
  if (bRecurse) CIrrOdeSceneNode::copyParams(pDest);
  pDest->setName(getName());

  CIrrOdeGeom *pDst=(CIrrOdeGeom *)pDest;
  pDst->setCollide(doesCollide());

  for (u32 i=0; i<getSurfaceParametersCount() && i<pDst->getSurfaceParametersCount(); i++)
    pDst->setSurfaceParameterName(i,m_aParamNames[i].c_str());

  if (getSurfaceParametersCount()<pDst->getSurfaceParametersCount())
    for (u32 i=getSurfaceParametersCount(); i<pDst->getSurfaceParametersCount(); i++)
      pDst->setSurfaceParameterName(i,m_aParamNames[i].c_str());

  if (m_cInertia1.getLength()>0.0f)
    pDst->setMassParameters(m_fMass,m_cCenterOfGravity,m_cInertia1,m_cInertia2);
  else
    pDst->setMassTotal(m_fMass);

  pDst->setMassTranslation(m_cMassTranslate);
}

void CIrrOdeGeom::removeFromPhysics() {
  CIrrOdeSceneNode::removeFromPhysics();
  if (m_iGeomId) {
    m_pOdeDevice->geomDestroy(m_iGeomId);
    m_iGeomId=0;
    if (m_pBody)
      m_pBody->removeGeom(this);
    else
      if (m_pWorld)
        m_pWorld->removeGeom(this);
  }
  else
    if (!m_bPhysicsInitialized) {
      if (m_pBody) m_pBody->removeGeom(this); else m_pWorld->removeGeom(this);
    }
}

void CIrrOdeGeom::setOffsetPosition(vector3df pPos) {
  m_cOffsetPos=pPos;
  if (m_iGeomId) {
    #ifdef _TRACE_INIT_PHYSICS
      printf("CIrrOdeGeom::setOffsetPosition: setting offset position to (%.2f, %.2f, %.2f)\n",pPos.X,pPos.Y,pPos.Z);
    #endif
    m_pOdeDevice->geomSetOffsetPosition(m_iGeomId,m_cOffsetPos);
  }
}

void CIrrOdeGeom::setOffsetQuaternion(vector3df pRot) {
  m_cOffsetRot=pRot;
  if (m_iGeomId) {
    #ifdef _TRACE_INIT_PHYSICS
      printf("CIrrOdeGeom::setOffsetQuaternion: setting offset rotation to (%.2f, %.2f, %.2f)\n",pRot.X,pRot.Y,pRot.Z);
    #endif
    m_pOdeDevice->geomSetOffsetQuaternion(m_iGeomId,m_cOffsetRot);
  }
}

void CIrrOdeGeom::setPosition(const vector3df &newpos) {
	ISceneNode::setPosition(vector3df(0.0f,0.0f,0.0f));
}

void CIrrOdeGeom::setCollide(bool b) {
  m_bCollide=b;
}

bool CIrrOdeGeom::doesCollide() {
  return m_bCollide;
}

} //namespace ode
} //namespace irr

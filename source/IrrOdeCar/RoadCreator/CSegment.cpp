  #include <CSegment.h>
  #include <INotification.h>
  #include <CTextureParameters.h>

using namespace irr;

/**
 * The construtor. This one is used by the GUI of the editor
 * @param sName name of the segment
 * @param vPosition position of the segment
 * @param pDrv the Irrlicht video driver
 */
CSegment::CSegment(core::stringc sName, core::vector3df vPosition, video::IVideoDriver *pDrv) {
  //Default values: width: 10, length: 25, direction: (0,0,1), normal: (0,1,0), base offset: 10, level base: true
  m_fWidth=10.0f;
  m_fLength=25.0f;
  m_vPosition=vPosition;
  m_vDirection=core::vector3df(0.0f,0.0f,1.0f);
  m_vNormal=core::vector3df(0.0f,1.0f,0.0f);
  m_sName=sName;
  m_pDrv=pDrv;
  m_fBaseOffset=10.0f;
  m_bLevelBase=true;

  //initialize the buffer and texture parameter members
  for (u32 i=0; i<6; i++) {
    m_pBuffer[i]=NULL;
    m_pTexParams[i]=new CTextureParameters();
  }

  recalcMeshBuffer();
}

/**
 * The construtor. This one is used by the road loading routine
 * @param pDrv the Irrlicht video driver
 */
CSegment::CSegment(video::IVideoDriver *pDrv) {
  //Default values: width: 10, length: 25, direction: (0,0,1), normal: (0,1,0), base offset: 10, level base: true
  m_fWidth=10.0f;
  m_fLength=25.0f;
  m_vPosition=core::vector3df(0.0f,0.0f,0.0f);
  m_vDirection=core::vector3df(0.0f,0.0f,1.0f);
  m_vNormal=core::vector3df(0.0f,1.0f,0.0f);
  m_sName="name";
  m_pDrv=pDrv;
  m_fBaseOffset=10.0f;
  m_bLevelBase=true;

  //initialize the buffer and texture parameter members
  for (u32 i=0; i<6; i++) {
    m_pBuffer[i]=NULL;
    m_pTexParams[i]=new CTextureParameters();
  }

  recalcMeshBuffer();
}

CSegment::~CSegment() {
  core::list<INotification *>::Iterator it;
  for (it=m_lNotify.begin(); it!=m_lNotify.end(); it++) {
    (*it)->objectDeleted(this);
  }

  for (u32 i=0; i<6; i++) {
    m_pBuffer[i]->drop();
    delete m_pTexParams[i];
  }
}

/**
 * The destructor
 */
void CSegment::attributeChanged() {
  recalcMeshBuffer();

  core::list<INotification *>::Iterator it;
  for (it=m_lNotify.begin(); it!=m_lNotify.end(); it++) {
    (*it)->attributesChanged(this);
  }
}

void CSegment::setWidth (f32 f) { m_fWidth =f; attributeChanged(); }
void CSegment::setLength(f32 f) { m_fLength=f; attributeChanged(); }

f32 CSegment::getWidth () { return m_fWidth ; }
f32 CSegment::getLength() { return m_fLength; }

void CSegment::setBaseOffset(f32 f) { m_fBaseOffset=f; attributeChanged(); }

f32 CSegment::getBaseOffset() { return m_fBaseOffset; }

void CSegment::setLevelBase(bool b) { m_bLevelBase=b; attributeChanged(); }

bool CSegment::getLevelBase() { return m_bLevelBase; }

void CSegment::setPosition (core::vector3df v) { m_vPosition =v;                           attributeChanged(); }
void CSegment::setDirection(core::vector3df v) { m_vDirection=v; m_vDirection.normalize(); attributeChanged(); }
void CSegment::setNormal   (core::vector3df v) { m_vNormal   =v; m_vNormal   .normalize(); attributeChanged(); }

const core::vector3df &CSegment::getPosition () { return m_vPosition ; }
const core::vector3df &CSegment::getDirection() { return m_vDirection; }
const core::vector3df &CSegment::getNormal   () { return m_vNormal   ; }

void CSegment::setName(const core::stringc &sName) { m_sName=sName; }

const core::stringc &CSegment::getName() { return m_sName; }

/**
 * This method takes an array of vectors and converts them to vertices
 * @param vec the array of vectors
 * @param pTex the texture parameters to be used
 * @param vert the output vertex array
 */
void CSegment::fillVertexArray(core::vector3df vec[], CTextureParameters *pTex, video::S3DVertex *vert) {
  //initialize some help variables to help finding the right points for the top shape
  core::vector3df calcLength=(m_fLength/2)*m_vDirection,
                  calcWidth =(m_fWidth /2)*(m_vNormal.crossProduct(m_vDirection));

  //Some locals for texture creation
  f32 f=calcWidth.getLength()!=0 && !pTex->getStretch()?calcLength.getLength()/calcWidth.getLength():1.0f,
      fStart=pTex->getOffset();

  f*=pTex->getScale();
  f+=fStart;

  f32 fTex[4][2];

  //Calculate the texture positions of the shape points depending on the rotation
  switch (pTex->getRotate()) {
    case 0:
      fTex[0][0]=0.0f; fTex[0][1]=fStart;
      fTex[1][0]=1.0f; fTex[1][1]=fStart;
      fTex[2][0]=1.0f; fTex[2][1]=     f;
      fTex[3][0]=0.0f; fTex[3][1]=     f;
      break;

    case 1:
      fTex[0][0]=0.0f; fTex[0][1]=     f;
      fTex[1][0]=0.0f; fTex[1][1]=fStart;
      fTex[2][0]=1.0f; fTex[2][1]=fStart;
      fTex[3][0]=1.0f; fTex[3][1]=     f;
      break;

    case 2:
      fTex[0][0]=1.0f; fTex[0][1]=     f;
      fTex[1][0]=0.0f; fTex[1][1]=     f;
      fTex[2][0]=0.0f; fTex[2][1]=fStart;
      fTex[3][0]=1.0f; fTex[3][1]=fStart;
      break;

    case 3:
      fTex[0][0]=1.0f; fTex[0][1]=fStart;
      fTex[1][0]=1.0f; fTex[1][1]=     f;
      fTex[2][0]=0.0f; fTex[2][1]=     f;
      fTex[3][0]=0.0f; fTex[3][1]=fStart;
      break;

    default:
      printf("invalide texture rotation!\n");
      return;
  }

  //Create verices inside the output array
  for (u32 i=0; i<4; i++) {
    vert[i]=video::S3DVertex(vec[i].X,vec[i].Y,vec[i].Z,
                             0,1,0,video::SColor(0xFF,0xE0,0xFF,0xE0),
                             fTex[i][0],fTex[i][1]);
  }
}

/**
 * This method recalculates the segment's meshbuffers
 * @see CSegment::CSegment
 * @see CSegment::attributeChanged
 * @see CSegment::load
 * @see CSegment::update
 */
void CSegment::recalcMeshBuffer() {
  //delete the meshbuffers
  for (u32 i=0; i<6; i++) if (m_pBuffer[i]!=NULL) m_pBuffer[i]->drop();

  core::vector3df pos=m_vPosition,
                  calcLength=(m_fLength/2)*m_vDirection,
                  calcWidth =(m_fWidth /2)*(m_vNormal.crossProduct(m_vDirection));

  //Calculate the four top points
  m_vPoints[0]=pos+calcLength-calcWidth;
  m_vPoints[1]=pos+calcLength+calcWidth;
  m_vPoints[2]=pos-calcLength+calcWidth;
  m_vPoints[3]=pos-calcLength-calcWidth;

  //convert the calculated points to vertices and add them to the meshbuffer
  video::S3DVertex vVerts[4];
  fillVertexArray(m_vPoints,m_pTexParams[0],vVerts);

  u16 iIdx[]={ 0,1,3, 3,1,2 };

  m_pBuffer[0]=new scene::SMeshBuffer();
  m_pBuffer[0]->append(vVerts,4,iIdx,6);
  m_pBuffer[0]->recalculateBoundingBox();

  if (m_pTexParams[0]->getTexture()!="")
    m_pBuffer[0]->getMaterial().setTexture(0,m_pDrv->getTexture(m_pTexParams[0]->getTexture().c_str()));

  //Same procedure for the bottom polygon...
  core::vector3df vBasePoints[4];
  vBasePoints[0]=m_vPoints[3]-core::vector3df(0.0f,m_fBaseOffset,0.0f);
  vBasePoints[1]=m_vPoints[2]-core::vector3df(0.0f,m_fBaseOffset,0.0f);
  vBasePoints[2]=m_vPoints[1]-core::vector3df(0.0f,m_fBaseOffset,0.0f);
  vBasePoints[3]=m_vPoints[0]-core::vector3df(0.0f,m_fBaseOffset,0.0f);

  if (m_bLevelBase) {
    //if the "level base" attribute is set we calculate the average Y of all four
    //bottom points and use that value for all of them
    f32 fNewBase=(vBasePoints[0].Y+vBasePoints[1].Y+vBasePoints[2].Y+vBasePoints[3].Y)/4;

    vBasePoints[0].Y=fNewBase;
    vBasePoints[1].Y=fNewBase;
    vBasePoints[2].Y=fNewBase;
    vBasePoints[3].Y=fNewBase;
  }

  u16 iBaseIdx[]={ 2,0,1, 0,2,3 };
  fillVertexArray(vBasePoints,m_pTexParams[1],vVerts);

  m_pBuffer[1]=new scene::SMeshBuffer();
  m_pBuffer[1]->append(vVerts,4,iBaseIdx,6);
  m_pBuffer[1]->recalculateBoundingBox();

  if (m_pTexParams[1]->getTexture()!="")
    m_pBuffer[1]->getMaterial().setTexture(0,m_pDrv->getTexture(m_pTexParams[1]->getTexture().c_str()));

  //Now for the automatic part: the four sides can be calculated
  //within a loop, so we do this.
  for (u32 i=0; i<4; i++) {
    u16 iSideIdx[]={ 0,1,2, 2,3,0 };
    core::vector3df vSide[4];

    //Depending on the counter value we have to use different values so we switch a little
    switch (i) {
      case 0:
        vSide[0]=m_vPoints[1];
        vSide[1]=m_vPoints[0];
        vSide[2]=vBasePoints[3];
        vSide[3]=vBasePoints[2];
        break;

      case 1:
        vSide[0]=m_vPoints[2];
        vSide[1]=m_vPoints[1];
        vSide[2]=vBasePoints[2];
        vSide[3]=vBasePoints[1];
        break;

      case 2:
        vSide[0]=m_vPoints[3];
        vSide[1]=m_vPoints[2];
        vSide[2]=vBasePoints[1];
        vSide[3]=vBasePoints[0];
        break;

      case 3:
        vSide[0]=m_vPoints[0];
        vSide[1]=m_vPoints[3];
        vSide[2]=vBasePoints[0];
        vSide[3]=vBasePoints[3];
        break;
    }

    //Fill the array, create the meshbuffer, add the created array and do something about the texture...
    fillVertexArray(vSide,m_pTexParams[i+2],vVerts);
    m_pBuffer[i+2]=new scene::SMeshBuffer();
    m_pBuffer[i+2]->append(vVerts,4,iSideIdx,6);
    m_pBuffer[i+2]->recalculateBoundingBox();

    if (m_pTexParams[i+2]->getTexture()!="")
      m_pBuffer[i+2]->getMaterial().setTexture(0,m_pDrv->getTexture(m_pTexParams[i+2]->getTexture().c_str()));
  }
}

/**
 * Render method. Just for the editor
 */
void CSegment::render() {
  //Render all 6 meshbuffers
  for (u32 i=0; i<6; i++)
    if (m_pBuffer[i]!=NULL) {
      video::SMaterial cMat;

      cMat.Lighting=false;
      cMat.setTexture(0,m_pBuffer[i]->getMaterial().getTexture(0));

      m_pDrv->setMaterial(cMat);
      m_pDrv->setTransform(video::ETS_WORLD,core::CMatrix4<f32>());
      m_pDrv->drawMeshBuffer(m_pBuffer[i]);
    }
}

const core::vector3df &CSegment::getPoint(u32 i) {
  static const core::vector3df vErr=core::vector3df(0.0f,0.0f,0.0f);
  if (i>=4) return vErr; else return m_vPoints[i];
}

void CSegment::save(io::IAttributes *out) {
  out->addString("Name"   ,m_sName   .c_str());

  out->addFloat("Width" ,m_fWidth     );
  out->addFloat("Length",m_fLength    );
  out->addFloat("Offset",m_fBaseOffset);

  out->addBool("LevelBase",m_bLevelBase);

  out->addVector3d("Position" ,m_vPosition );
  out->addVector3d("Direction",m_vDirection);
  out->addVector3d("Normal"   ,m_vNormal   );
}

void CSegment::load(io::IAttributes *in) {
  m_sName   =in->getAttributeAsString("Name"   );

  m_fWidth     =in->getAttributeAsFloat("Width" );
  m_fLength    =in->getAttributeAsFloat("Length");
  m_fBaseOffset=in->getAttributeAsFloat("Offset");

  m_bLevelBase=in->getAttributeAsBool("LevelBase");

  m_vPosition =in->getAttributeAsVector3d("Position" );
  m_vDirection=in->getAttributeAsVector3d("Direction");
  m_vNormal   =in->getAttributeAsVector3d("Normal"   );

  recalcMeshBuffer();
}

void CSegment::addNotify(INotification *p) {
  core::list<INotification *>::Iterator it;
  for (it=m_lNotify.begin(); it!=m_lNotify.end(); it++) {
    if (*it==p) return;
  }
  m_lNotify.push_back(p);
}

void CSegment::delNotify(INotification *p) {
  core::list<INotification *>::Iterator it;
  for (it=m_lNotify.begin(); it!=m_lNotify.end(); it++) {
    if (*it==p) {
      m_lNotify.erase(it);
      return;
    }
  }
}

void CSegment::update() {
  recalcMeshBuffer();
}

CTextureParameters *CSegment::getTextureParameters(u32 i) {
  return i<6?m_pTexParams[i]:NULL;
}

scene::IMeshBuffer *CSegment::getMeshBuffer(u32 i) {
  return m_pBuffer[i];
}
  #include <CTextureParameters.h>
  #include <CSurface.h>

CSurface::CSurface(irr::IrrlichtDevice *pDevice, CTextureParameters *pInitParam) {
  m_pDevice=pDevice;
  m_pDrv=m_pDevice->getVideoDriver();
  m_pFs=m_pDevice->getFileSystem();

  m_cMinPos=irr::core::vector3df(-100.0f,0.0f,-100.0f);
  m_cMaxPos=irr::core::vector3df( 100.0f,0.0f, 100.0f);

  m_cTexRepeat=irr::core::vector2df(5.0f,5.0f);

  m_fFenceHeight=50.0f;

  m_bVisible=false;

  for (irr::u32 i=0; i<2; i++) {
    m_pBuffers[i]=NULL;
    m_pParams[i]=new CTextureParameters();
    if (pInitParam!=NULL) {
      pInitParam[i].copyTo(m_pParams[i]);
    }
  }
}

CSurface::~CSurface() {
}

void CSurface::recalcMeshBuffer() {
  for (irr::u32 i=0; i<2; i++) {
    if (m_pBuffers[i]!=NULL) m_pBuffers[i]->drop();
    m_pBuffers[i]=NULL;
  }

  irr::f32 y=m_cMinPos.Y;
  irr::video::S3DVertex cVert[4];

  cVert[0].Pos=irr::core::vector3df(m_cMinPos.X,y,m_cMinPos.Z);
  cVert[1].Pos=irr::core::vector3df(m_cMinPos.X,y,m_cMaxPos.Z);
  cVert[2].Pos=irr::core::vector3df(m_cMaxPos.X,y,m_cMaxPos.Z);
  cVert[3].Pos=irr::core::vector3df(m_cMaxPos.X,y,m_cMinPos.Z);

  cVert[0].Normal=irr::core::vector3df(0.0f,1.0f,0.0f);
  cVert[1].Normal=irr::core::vector3df(0.0f,1.0f,0.0f);
  cVert[2].Normal=irr::core::vector3df(0.0f,1.0f,0.0f);
  cVert[3].Normal=irr::core::vector3df(0.0f,1.0f,0.0f);

  cVert[0].TCoords.X=          0.0f; cVert[0].TCoords.Y=          0.0f;
  cVert[1].TCoords.X=m_cTexRepeat.X; cVert[1].TCoords.Y=          0.0f;
  cVert[2].TCoords.X=m_cTexRepeat.X; cVert[2].TCoords.Y=m_cTexRepeat.Y;
  cVert[3].TCoords.X=          0.0f; cVert[3].TCoords.Y=m_cTexRepeat.Y;

  cVert[0].Color=irr::video::SColor(0xFF,0xFF,0xFF,0xFF);
  cVert[1].Color=irr::video::SColor(0xFF,0xFF,0xFF,0xFF);
  cVert[2].Color=irr::video::SColor(0xFF,0xFF,0xFF,0xFF);
  cVert[3].Color=irr::video::SColor(0xFF,0xFF,0xFF,0xFF);

  irr::u16 idx[]={ 0,1,2, 0,2,3 };

  m_pBuffers[0]=new irr::scene::SMeshBuffer();
  m_pBuffers[0]->append(cVert,4,idx,6);

  if (m_fFenceHeight>0.0f) {
    m_pBuffers[1]=new irr::scene::SMeshBuffer();
    cVert[0].Pos=irr::core::vector3df(m_cMinPos.X,y               ,m_cMinPos.Z);
    cVert[1].Pos=irr::core::vector3df(m_cMinPos.X,y               ,m_cMaxPos.Z);
    cVert[2].Pos=irr::core::vector3df(m_cMinPos.X,y+m_fFenceHeight,m_cMaxPos.Z);
    cVert[3].Pos=irr::core::vector3df(m_cMinPos.X,y+m_fFenceHeight,m_cMinPos.Z);

    cVert[0].Normal=irr::core::vector3df(1.0f,0.0f,0.f);
    cVert[1].Normal=irr::core::vector3df(1.0f,0.0f,0.f);
    cVert[2].Normal=irr::core::vector3df(1.0f,0.0f,0.f);
    cVert[3].Normal=irr::core::vector3df(1.0f,0.0f,0.f);

    irr::u16 fenceIdx[]={ 0,2,1, 0,3,2 };

    m_pBuffers[1]->append(cVert,4,fenceIdx,6);

    cVert[0].Pos=irr::core::vector3df(m_cMaxPos.X,y               ,m_cMaxPos.Z);
    cVert[1].Pos=irr::core::vector3df(m_cMaxPos.X,y               ,m_cMinPos.Z);
    cVert[2].Pos=irr::core::vector3df(m_cMaxPos.X,y+m_fFenceHeight,m_cMinPos.Z);
    cVert[3].Pos=irr::core::vector3df(m_cMaxPos.X,y+m_fFenceHeight,m_cMaxPos.Z);

    cVert[0].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[1].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[2].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[3].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);

    m_pBuffers[1]->append(cVert,4,fenceIdx,6);

    cVert[0].Pos=irr::core::vector3df(m_cMinPos.X,y+m_fFenceHeight,m_cMinPos.Z);
    cVert[1].Pos=irr::core::vector3df(m_cMaxPos.X,y+m_fFenceHeight,m_cMinPos.Z);
    cVert[2].Pos=irr::core::vector3df(m_cMaxPos.X,y               ,m_cMinPos.Z);
    cVert[3].Pos=irr::core::vector3df(m_cMinPos.X,y               ,m_cMinPos.Z);

    cVert[0].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[1].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[2].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[3].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);

    m_pBuffers[1]->append(cVert,4,fenceIdx,6);

    cVert[0].Pos=irr::core::vector3df(m_cMaxPos.X,y+m_fFenceHeight,m_cMaxPos.Z);
    cVert[1].Pos=irr::core::vector3df(m_cMinPos.X,y+m_fFenceHeight,m_cMaxPos.Z);
    cVert[2].Pos=irr::core::vector3df(m_cMinPos.X,y               ,m_cMaxPos.Z);
    cVert[3].Pos=irr::core::vector3df(m_cMaxPos.X,y               ,m_cMaxPos.Z);

    cVert[0].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[1].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[2].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);
    cVert[3].Normal=irr::core::vector3df(-1.0f,0.0f,0.f);

    m_pBuffers[1]->append(cVert,4,fenceIdx,6);
  }
  else m_pBuffers[1]=NULL;

  for (irr::u32 i=0; i<_SURFACE_NUMBER_OF_BUFFERS; i++) {
    irr::video::ITexture *pTex=m_pDrv->getTexture(m_pParams[i]->getTexture().c_str());
    printf("==> %s\n",m_pParams[i]->getTexture().c_str());
    m_pBuffers[i]->getMaterial().setTexture(0,pTex);
  }
}

irr::u32 CSurface::getTextureCount() {
  return 2;
}

CTextureParameters *CSurface::getTextureParameters(irr::u32 i) {
  return i<2?m_pParams[i]:NULL;
}

irr::scene::IMeshBuffer *CSurface::getMeshBuffer(irr::u32 i) {
  return i<2?m_pBuffers[i]:NULL;
}

void CSurface::render() {
  if (!m_bVisible) return;

  for (irr::u32 i=0; i<2; i++)
    if (m_pBuffers[i]!=NULL) {
      irr::video::SMaterial cMat;

      cMat.Lighting=false;
      cMat.setTexture(0,m_pBuffers[i]->getMaterial().getTexture(0));

      m_pDrv->setMaterial(cMat);
      m_pDrv->setTransform(irr::video::ETS_WORLD,irr::core::CMatrix4<irr::f32>());
      m_pDrv->drawMeshBuffer(m_pBuffers[i]);
    }
}

void CSurface::save(irr::io::IAttributes *out) {
  irr::f32 y=(m_cMinPos.Y+m_cMaxPos.Y)/2;

  m_cMinPos.Y=y;
  m_cMaxPos.Y=y;

  out->addVector3d("MinPos",m_cMinPos);
  out->addVector3d("MaxPos",m_cMaxPos);

  out->addFloat("TextureRepeat_X",m_cTexRepeat.X);
  out->addFloat("TextureRepeat_Y",m_cTexRepeat.Y);

  out->addFloat("FenceHeight",m_fFenceHeight);
}

void CSurface::load(irr::io::IAttributes *in ){
  m_cMinPos=in->getAttributeAsVector3d("MinPos");
  m_cMaxPos=in->getAttributeAsVector3d("MaxPos");

  m_cTexRepeat.X=in->getAttributeAsFloat("TextureRepeat_X");
  m_cTexRepeat.Y=in->getAttributeAsFloat("TextureRepeat_Y");

  m_fFenceHeight=in->getAttributeAsFloat("FenceHeight");

  recalcMeshBuffer();
}

void CSurface::setCorners(const irr::core::vector3df &cMinPos, const irr::core::vector3df &cMaxPos) {
  m_cMinPos=cMinPos;
  m_cMaxPos=cMaxPos;
}

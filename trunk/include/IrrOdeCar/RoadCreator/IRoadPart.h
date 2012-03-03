#ifndef _I_ROAD_PART
  #define _I_ROAD_PART

  #include <irrlicht.h>

class CTextureParameters;

static irr::video::ITexture *s_pEmptyTex=NULL;

class IRoadPart {
  protected:
    irr::s32 m_iMeshBufferToDraw;
    irr::video::IVideoDriver *m_pDrv;

    irr::video::ITexture *getTexture(const irr::c8 *sPath) {
      if (sPath[0]=='\0')
        return s_pEmptyTex;
      else
        return m_pDrv->getTexture(sPath);
    }

  public:
    IRoadPart(irr::video::IVideoDriver *pDrv) {
      m_iMeshBufferToDraw=-1;
      m_pDrv=pDrv;
      if (s_pEmptyTex==NULL) s_pEmptyTex=m_pDrv->getTexture("");
    }

    virtual void recalcMeshBuffer()=0;

    virtual irr::u32 getTextureCount()=0;
    virtual CTextureParameters *getTextureParameters(irr::u32 i)=0;
    virtual irr::scene::IMeshBuffer *getMeshBuffer(irr::u32 i)=0;
    virtual void render()=0;
    virtual void save(irr::io::IAttributes *out)=0;
    virtual void load(irr::io::IAttributes *in )=0;
    virtual irr::s32 getNumberOfMeshBuffers()=0;

    virtual void renderMeshBuffer(irr::s32 iNum) {
      m_iMeshBufferToDraw=iNum;
    }
};

#endif

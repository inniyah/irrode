#ifndef _C_SURFACE
  #define _C_SURFACE

  #include <irrlicht.h>

#define _SURFACE_NUMBER_OF_BUFFERS 2

class CTextureParameters;

class CSurface {
  protected:
    bool m_bVisible;

    irr::core::vector3df m_cMinPos,
                    m_cMaxPos;
    irr::core::vector2df m_cTexRepeat;
    irr::f32 m_fFenceHeight;

    irr::scene::IMeshBuffer *m_pBuffers[_SURFACE_NUMBER_OF_BUFFERS];  /**<! Meshbuffer for ground and fence */

    CTextureParameters *m_pParams[_SURFACE_NUMBER_OF_BUFFERS]; /**<! Texture Parameters */

    irr::IrrlichtDevice *m_pDevice;
    irr::video::IVideoDriver *m_pDrv;
    irr::io::IFileSystem *m_pFs;

  public:
    CSurface(irr::IrrlichtDevice *pDevice, CTextureParameters *pInitParam);
    virtual ~CSurface();

    virtual void recalcMeshBuffer();

    virtual irr::u32 getTextureCount();
    virtual CTextureParameters *getTextureParameters(irr::u32 i);
    virtual irr::scene::IMeshBuffer *getMeshBuffer(irr::u32 i);
    virtual void render();
    virtual void save(irr::io::IAttributes *out);
    virtual void load(irr::io::IAttributes *in );

    void setCorners(const irr::core::vector3df &cMinPos, const irr::core::vector3df &cMaxPos);

    void setFenceHeight(irr::f32 f) {
      m_fFenceHeight=f;
    }

    const irr::core::vector3df &getMinPos() { return m_cMinPos; }
    const irr::core::vector3df &getMaxPos() { return m_cMaxPos; }

    irr::f32 getFenceHeight() { return m_fFenceHeight; }

    bool isVisible() { return m_bVisible; }
    void setVisible(bool b) { m_bVisible=b; }

    void setTextureRepeat(irr::core::vector2df &v) { m_cTexRepeat=v; }
    const irr::core::vector2df &getTextureRepeat() { return m_cTexRepeat; }
};

#endif


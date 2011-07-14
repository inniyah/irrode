#ifndef _I_RENDER_TO_TEXTURE
  #define _I_RENDER_TO_TEXTURE

  #include <irrlicht.h>

using namespace irr;

class IRenderToTexture {
  protected:
    IrrlichtDevice *m_pDevice;

  public:
    IRenderToTexture(IrrlichtDevice *pDevice) {
      m_pDevice=pDevice;
    }

    virtual ~IRenderToTexture() {
    }

    virtual void update()=0;
};

#endif



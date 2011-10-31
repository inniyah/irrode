#ifndef _C_REAR_VIEW
  #define _C_REAR_VIEW

  #include <irrlicht.h>
  #include <IRenderToTexture.h>

using namespace irr;

class CRearView : public IRenderToTexture {
  protected:
    scene::ICameraSceneNode *m_pCam;
    IrrlichtDevice *m_pDevice;

  public:
    CRearView(IrrlichtDevice *pDevice, const char *sName, scene::ICameraSceneNode *pCam);
    virtual ~CRearView();

    virtual void update(bool b);

    void setCameraParameters(const core::vector3df cPos, const core::vector3df cTgt, const core::vector3df cUp);
};

#endif


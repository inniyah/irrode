#ifndef _C_REAR_VIEW
  #define _C_REAR_VIEW

  #include <irrlicht.h>
  #include <IRenderToTexture.h>

class CRearView : public IRenderToTexture {
  protected:
    irr::scene::ICameraSceneNode *m_pCam;
    irr::IrrlichtDevice *m_pDevice;

  public:
    CRearView(irr::IrrlichtDevice *pDevice, const char *sName, irr::scene::ICameraSceneNode *pCam);
    virtual ~CRearView();

    virtual void update(bool b);

    void setCameraParameters(const irr::core::vector3df cPos, const irr::core::vector3df cTgt, const irr::core::vector3df cUp);
};

#endif


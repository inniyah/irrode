#ifndef _C_GUI_RENDER_TO_TEXTURE
  #define _C_GUI_RENDER_TO_TEXTURE

  #include <irrlicht.h>
  #include <IRenderToTexture.h>

namespace irr {
  namespace gui {
    class CNrpNeedleIndicator;
  }
}

using namespace irr;

class CCockpitPlane : public IRenderToTexture {
  protected:
    gui::IGUIEnvironment    *m_pGuienv;
    video::IVideoDriver     *m_pDrv;
    scene::ISceneManager    *m_pSmgr,
                            *m_pRttSmgr;
    video::ITexture         *m_pTarget,
                            *m_pElement,
                            *m_pWarnTexPlane[4][4],
                            *m_pWarnTexHeli[2][4];
    gui::IGUIImage          *m_pWarnImgPlane[4],
                            *m_pWarnImgHeli[2];
    gui::IGUITab            *m_pTab,
                            *m_pPlaneWarnings,
                            *m_pHeliWarnings;
    gui::IGUIStaticText     *m_pLblTgtDist,
                            *m_pLblTgtName,
                            *m_pLblShots,
                            *m_pLblHits;
    scene::ISceneNode       *m_pHorizon;
    scene::ICameraSceneNode *m_pCam;

    gui::CNrpNeedleIndicator *m_pInstruments[6];

    f32 m_fAltitude,
        m_fSpeed,
        m_fPower,
        m_fHeading,
        m_fVelVert;

  public:
    CCockpitPlane(IrrlichtDevice *pDevice, scene::ISceneManager *pRttSmgr);
    virtual ~CCockpitPlane();

    video::ITexture *getTexture();

    virtual void update(bool bPlane);

    void setAltitude(f32 f) { m_fAltitude=f; }
    void setSpeed   (f32 f) { m_fSpeed   =f; }
    void setPower   (f32 f) { m_fPower   =f; }
    void setHeading (f32 f) { m_fHeading =f; }
    void setVelVert (f32 f) { m_fVelVert =f; }

    void setHorizon(core::vector3df vRot, core::vector3df vUp);

    void setWarnStatePlane(u32 iWarn, u32 iState);
    void setWarnStateHeli(u32 iWarn, u32 iState);

    void setTargetName(const wchar_t *sName);
    void setTargetDist(f32 fDist);

    void setShotsFired(s32 iShots);
    void setHits(s32 iHits);
};

#endif

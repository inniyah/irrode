#ifndef _C_GUI_RENDER_TO_TEXTURE
  #define _C_GUI_RENDER_TO_TEXTURE

  #include <irrlicht.h>
  #include <IRenderToTexture.h>
  #include <event/IIrrOdeEventListener.h>
  #include <CIrrOdeBody.h>

namespace irr {
  namespace gui {
    class CGUINeedleIndicator;
  }
}
  namespace ode {
    class IIrrOdeEvent;
  }


using namespace irr;

class CCockpitPlane : public IRenderToTexture, public irr::ode::IIrrOdeEventListener {
  protected:
    scene::ISceneManager    *m_pRttSmgr;
    video::ITexture         *m_pElement,
                            *m_pWarnTexPlane[4][4],
                            *m_pWarnTexHeli[2][4];
    gui::IGUIImage          *m_pWarnImgPlane[4],
                            *m_pWarnImgHeli[2];
    gui::IGUITab            *m_pTab,
                            *m_pPlaneWarnings,
                            *m_pHeliWarnings,
                            *m_pWeaponInfo,
                            *m_pLapInfo;
    gui::IGUIStaticText     *m_pLblTgtDist,
                            *m_pLblTgtName,
                            *m_pLblShots,
                            *m_pLblHitsScored,
                            *m_pLblHitsTaken,
                            *m_stCurLap,
                            *m_stLastLap,
                            *m_stSplit;
    scene::ISceneNode       *m_pHorizon;
    scene::ICameraSceneNode *m_pCam;

    gui::CGUINeedleIndicator *m_pInstruments[6];

    f32 m_fAltitude,
        m_fSpeed,
        m_fPower,
        m_fHeading,
        m_fVelVert;
    u32 m_iInfoMode,
        m_iTime,
        m_iLapStart;
    s32 m_iBodyId;
    bool m_bLapStarted;

  public:
    CCockpitPlane(IrrlichtDevice *pDevice, const char *sName);
    virtual ~CCockpitPlane();

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
    void setHitsScored(s32 iHits);
    void setHitsTaken(s32 iHits);

    void activate(irr::ode::CIrrOdeBody *p) { if (p!=NULL) m_iBodyId = p->getID(); else m_iBodyId = -1; m_iInfoMode = 0; }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

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


class CCockpitPlane : public IRenderToTexture, public irr::ode::IIrrOdeEventListener {
  protected:
    irr::scene::ISceneManager     *m_pRttSmgr;
    irr::video::ITexture          *m_pElement,
                                  *m_pWarnTexPlane[4][4],
                                  *m_pWarnTexHeli[2][4];
    irr::gui::IGUIImage           *m_pWarnImgPlane[4],
                                  *m_pWarnImgHeli[2];
    irr::gui::IGUITab             *m_pTab,
                                  *m_pPlaneWarnings,
                                  *m_pHeliWarnings,
                                  *m_pWeaponInfo,
                                  *m_pLapInfo,
                                  *m_pApInfo;
    irr::gui::IGUIStaticText      *m_pLblTgtDist,
                                  *m_pLblTgtName,
                                  *m_pLblShots,
                                  *m_pLblHitsScored,
                                  *m_pLblHitsTaken,
                                  *m_stCurLap,
                                  *m_stLastLap,
                                  *m_stSplit,
                                  *m_stAutoPilot,
                                  *m_stApNextCp,
                                  *m_stApState;
    irr::scene::ISceneNode        *m_pHorizon,
                                  *m_pApTarget;
    irr::ode::CIrrOdeBody         *m_pObject;
    irr::scene::ICameraSceneNode  *m_pCam;
    irr::gui::CGUINeedleIndicator *m_pInstruments[6];

    irr::f32 m_fAltitude,
             m_fSpeed,
             m_fPower,
             m_fHeading,
             m_fVelVert;
    irr::u32 m_iInfoMode,
             m_iTime,
             m_iLapStart;
    irr::s32 m_iBodyId;
    bool m_bLapStarted;

    void updateApState(irr::s32 iApState);

  public:
    CCockpitPlane(irr::IrrlichtDevice *pDevice, const char *sName, irr::ode::CIrrOdeBody *pObject);
    virtual ~CCockpitPlane();

    virtual void update(bool bPlane);

    void setPower   (irr::f32 f) { m_fPower   =f; }

    void setHorizon(irr::core::vector3df vRot, irr::core::vector3df vUp);

    void setTargetName(const wchar_t *sName);
    void setTargetDist(irr::f32 fDist);

    void setShotsFired(irr::s32 iShots);
    void setHitsScored(irr::s32 iHits);
    void setHitsTaken(irr::s32 iHits);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    irr::u32 getInfoMode() { return m_iInfoMode; }

    irr::scene::ISceneNode *getApTarget() { return m_pApTarget; }
};

#endif

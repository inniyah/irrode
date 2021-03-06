#ifndef _C_GUI_RENDER_TO_TEXTURE
  #define _C_GUI_RENDER_TO_TEXTURE

  #include <irrlicht.h>
  #include <IRenderToTexture.h>
  #include <event/IIrrOdeEventListener.h>
  #include <CIrrOdeBody.h>

const irr::c8 g_sCockpitPlane[] = "CockpitPlane";

namespace irr {
  namespace gui {
    class CGUINeedleIndicator;
  }
  namespace ode {
    class IIrrOdeEvent;
  }
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
    irr::gui::IGUIStaticText      *m_stTgtDist,
                                  *m_stTgtName,
                                  *m_stShots,
                                  *m_stHitsScored,
                                  *m_stHitsTaken,
                                  *m_stCurLap,
                                  *m_stLastLap,
                                  *m_stSplit,
                                  *m_stAutoPilot,
                                  *m_stApNextCp,
                                  *m_stApState;
    irr::scene::ISceneNode        *m_pHorizon,
                                  *m_pApTarget,
                                  *m_pTarget;
    irr::ode::CIrrOdeBody         *m_pObject;
    irr::ode::CIrrOdeManager      *m_pOdeMgr;
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
    bool m_bLapStarted,
         m_bPlane;

    void updateApState(irr::s32 iApState);
    void setHorizon(irr::core::vector3df vRot, irr::core::vector3df vUp);

  public:
    CCockpitPlane(irr::IrrlichtDevice *pDevice, const char *sName, irr::ode::CIrrOdeBody *pObject, irr::ode::CIrrOdeManager *pOdeMgr);
    virtual ~CCockpitPlane();

    virtual void update();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

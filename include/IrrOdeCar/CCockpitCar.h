#ifndef _C_COCKPIT_CAR
  #define _C_COCKPIT_CAR

  #include <irrlicht.h>
  #include <IRenderToTexture.h>
  #include <event/IIrrOdeEventListener.h>
  #include <CIrrOdeBody.h>

namespace irr {
  namespace gui {
    class CGUINeedleIndicator;
  }

  namespace ode {
    class IIrrOdeEvent;
  }
}

class CCockpitCar : public IRenderToTexture, public irr::ode::IIrrOdeEventListener {
  protected:
    irr::f32 m_fSpeed,
             m_fDiff,
             m_fRpm;

    irr::s32 m_iBodyId,
             m_iLapStart,
             m_iTime;

    bool m_bDifferential,
         m_bLapStarted;

    irr::gui::CGUINeedleIndicator *m_pMeter,
                                  *m_pRpm,
                                  *m_pDiff;
    irr::gui::IGUIStaticText *m_stDifferential,
                             *m_stCurLap,
                             *m_stLastLap,
                             *m_stSplit;
    irr::gui::IGUITab   *m_pTab;
    irr::gui::IGUIImage *m_pBoostRed,
                        *m_pBoostGray;

  public:
    CCockpitCar(irr::IrrlichtDevice *pDevice, const char *sName);
    virtual ~CCockpitCar();

    virtual void update(bool b);

    void setSpeed(irr::f32 fSpeed) { m_fSpeed=fSpeed; }

    void setDiff(irr::f32 fDiff) { m_fDiff=fDiff; }

    void setRpm(irr::f32 fRpm) { m_fRpm=fRpm; }

    void setBoost(bool b);

    void setDifferentialEnabled(bool b) { m_bDifferential=b; }

    void activate(irr::ode::CIrrOdeBody *pBody) {
      if (pBody != NULL) m_iBodyId = pBody->getID(); else m_iBodyId = -1;
    }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

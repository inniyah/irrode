#ifndef _C_COCKPIT_CAR
  #define _C_COCKPIT_CAR

  #include <irrlicht.h>
  #include <IRenderToTexture.h>
  #include <event/IIrrOdeEventListener.h>
  #include <CIrrOdeBody.h>

const irr::c8 g_sCockpitCar[] = "CockpitCar";

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
             m_iTime,
             m_iBoost;

    bool m_bDifferential,
         m_bLapStarted,
         m_bBoost,
         m_bAdapt;

    irr::gui::CGUINeedleIndicator *m_pMeter,
                                  *m_pRpm,
                                  *m_pDiff;
    irr::gui::IGUIStaticText *m_stDifferential,
                             *m_stCurLap,
                             *m_stLastLap,
                             *m_stSplit,
                             *m_stBoost,
                             *m_stAdapt;
    irr::gui::IGUITab   *m_pTab;
    irr::gui::IGUIImage *m_pShiftRed,
                        *m_pShiftGray,
                        *m_pGear[6];

  public:
    CCockpitCar(irr::IrrlichtDevice *pDevice, const char *sName, irr::scene::ISceneNode *pBody);
    virtual ~CCockpitCar();

    virtual void update();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    virtual const irr::c8 *getName() { return g_sCockpitCar; }
};

#endif

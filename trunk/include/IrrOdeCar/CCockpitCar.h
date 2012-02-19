#ifndef _C_COCKPIT_CAR
  #define _C_COCKPIT_CAR

  #include <irrlicht.h>
  #include <IRenderToTexture.h>

namespace irr {
  namespace gui {
    class CGUINeedleIndicator;
  }
}

using namespace irr;

class CCockpitCar : public IRenderToTexture {
  protected:
    f32 m_fSpeed,
        m_fDiff,
        m_fRpm;

    bool m_bDifferential;

    gui::CGUINeedleIndicator *m_pMeter,
                             *m_pRpm,
                             *m_pDiff;
    gui::IGUIStaticText *m_stDifferential,
                        *m_stCurLap,
                        *m_stLastLap,
                        *m_stSplit;
    gui::IGUITab *m_pTab;
    gui::IGUIImage *m_pBoostRed,
                   *m_pBoostGray;

  public:
    CCockpitCar(IrrlichtDevice *pDevice, const char *sName);
    virtual ~CCockpitCar();

    virtual void update(bool b);

    void setSpeed(f32 fSpeed) { m_fSpeed=fSpeed; }

    void setDiff(f32 fDiff) { m_fDiff=fDiff; }

    void setRpm(f32 fRpm) { m_fRpm=fRpm; }

    void setBoost(bool b);

    void setDifferentialEnabled(bool b) { m_bDifferential=b; }

    void setCurrentLapTime(f32 fTime);
    void setLastLapTime(f32 fLastLap);
    void setSplitTime(f32 fTime);
    void cancelLap();
};

#endif

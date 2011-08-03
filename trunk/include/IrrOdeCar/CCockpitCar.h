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
    f32 m_fSpeed;

    gui::CGUINeedleIndicator *m_pMeter;
    gui::IGUITab *m_pTab;

  public:
    CCockpitCar(IrrlichtDevice *pDevice, const char *sName);
    virtual ~CCockpitCar();

    virtual void update(bool b);

    void setSpeed(f32 fSpeed) { m_fSpeed=fSpeed; }
};

#endif

#ifndef _C_IRR_ODE_CAR_TRACK
  #define _C_IRR_ODE_CAR_TRACK

  #include <irrlicht.h>
  #include <IrrOde.h>

class CIrrOdeCarTrack : public irr::ode::IIrrOdeEventListener {
  private:
    irr::s32 m_iBodyId,
             m_iNextCp;

    irr::u32 m_iCurStep,
             m_iLastLapStep;

    bool m_bPlane;

  public:
    CIrrOdeCarTrack(irr::ode::CIrrOdeBody *pBody);
    virtual ~CIrrOdeCarTrack();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

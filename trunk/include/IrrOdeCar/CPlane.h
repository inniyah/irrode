#ifndef _C_PLANE
  #define _C_PLANE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"
  #include <CEventVehicleState.h>
  #include <CAeroVehicle.h>

class CIrrOdeCarTrack;
class CTargetSelector;
class CProjectile;
class CAutoPilot;

class CPlane : public CAeroVehicle {
  protected:
    bool m_bLeftMissile,
         m_bAutoPilot;
    irr::ode::CIrrOdeJointHinge *m_pAxes[2];
    irr::ode::CIrrOdeJointHinge2 *m_pSteerAxis;
    irr::f32 m_fAngleRate[3];

    CIrrOdeCarTrack *m_pLap;

  public:
    CPlane(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, irr::ode::IIrrOdeEventQueue *pInputQueue);
    virtual ~CPlane();

    virtual void odeStep(irr::u32 iStep);

    virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"plane"; return s; }

    virtual void drawSpecifics();

    virtual irr::ode::IIrrOdeEvent *writeEvent();
    virtual irr::ode::eEventWriterType getEventWriterType();

    virtual enumStateType getType() { return eStatePlane; }
};

#endif



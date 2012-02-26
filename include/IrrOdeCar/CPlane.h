#ifndef _C_PLANE
  #define _C_PLANE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"
  #include <CEventVehicleState.h>
  #include <CAeroVehicle.h>

using namespace irr;

class CIrrOdeCarTrack;
class CTargetSelector;
class CCockpitPlane;
class CProjectile;
class CAutoPilot;
class CRearView;

class CPlane : public CAeroVehicle, public ode::IIrrOdeEventWriter {
  protected:
    bool m_bLeftMissile;
    ode::CIrrOdeJointHinge *m_pAxes[2];
    ode::CIrrOdeJointHinge2 *m_pSteerAxis;
    f32 m_fAngleRate[3];

    CIrrOdeCarTrack *m_pLap;

  public:
    CPlane(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, CRearView *pRView);
    virtual ~CPlane();

    virtual void odeStep(u32 iStep);

    virtual u32 update();

    virtual stringc &getButtonText() { static stringc s=stringc("select airplane"); return s; }
    virtual void drawSpecifics();

    virtual ode::IIrrOdeEvent *writeEvent();
    virtual ode::eEventWriterType getEventWriterType();

    virtual void activate();
};

#endif



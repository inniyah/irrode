#ifndef _C_PLANE
  #define _C_PLANE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"
  #include <CEventVehicleState.h>
  #include <CAeroVehicle.h>

class CIrrOdeCarTrack;
class CTargetSelector;
class CCockpitPlane;
class CProjectile;
class CAutoPilot;
class CRearView;

class CPlane : public CAeroVehicle, public irr::ode::IIrrOdeEventWriter {
  protected:
    bool m_bLeftMissile,
         m_bAutoPilot;
    irr::ode::CIrrOdeJointHinge *m_pAxes[2];
    irr::ode::CIrrOdeJointHinge2 *m_pSteerAxis;
    irr::f32 m_fAngleRate[3];
    irr::s32 m_iOldHitsTaken,
             m_iOldHitsScored;

    CIrrOdeCarTrack *m_pLap;

  public:
    CPlane(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, CRearView *pRView);
    virtual ~CPlane();

    virtual void odeStep(irr::u32 iStep);

    virtual irr::u32 update();

    virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"plane"; return s; }

    virtual void drawSpecifics();

    virtual irr::ode::IIrrOdeEvent *writeEvent();
    virtual irr::ode::eEventWriterType getEventWriterType();
};

#endif



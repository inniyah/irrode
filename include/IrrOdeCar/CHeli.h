#ifndef _C_HELICOPTER
  #define _C_HELICOPTER

  #include <irrlicht.h>
  #include <IrrOde.h>
  #include <CAeroVehicle.h>

  #include "CIrrOdeCarState.h"

enum eHeliControls {
  eHeliYawLeft,
  eHeliYawRight,
  eHeliRollLeft,
  eHeliRollRight,
  eHeliPitchUp,
  eHeliPitchDown,
  eHeliPowerUp,
  eHeliPowerDown,
  eHeliFire,
  eHeliToggleCam,
  eHeliFlip,
  eHeliTarget,
  eHeliAutoPilot,
  eHeliInternal,
  eHeliCamLeft,
  eHeliCamRight,
  eHeliCamCenter
};

class CTargetSelector;
class CCockpitPlane;
class CProjectile;
class CAutoPilot;
class CRearView;

class CHeli : public CAeroVehicle, public irr::ode::IIrrOdeEventWriter {
  protected:
    bool m_bLeft;
    irr::f32 m_fSound;
    irr::s32 m_iNodeId,
        m_iOldHitsTaken,
        m_iOldHitsScored;

  public:
    CHeli(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, CRearView *pRView);
    virtual ~CHeli();

    virtual irr::u32 update();

    virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"heli"; return s; }

    virtual void drawSpecifics();

    virtual void odeStep(irr::u32 iStep);

    virtual irr::ode::IIrrOdeEvent *writeEvent();
    virtual irr::ode::eEventWriterType getEventWriterType();

    virtual void activate();
};

#endif


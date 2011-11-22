#ifndef _C_HELICOPTER
  #define _C_HELICOPTER

  #include <irrlicht.h>
  #include <IrrOde.h>
  #include <CAeroVehicle.h>

  #include "CIrrOdeCarState.h"

using namespace irr;

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

class CHeli : public CAeroVehicle, public ode::IIrrOdeEventWriter {
  protected:
    bool m_bLeft;
    f32 m_fSound;
    s32 m_iNodeId;

  public:
    CHeli(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, CRearView *pRView);
    virtual ~CHeli();

    virtual u32 update();

    virtual stringc &getButtonText() { static stringc s=stringc("select helicopter"); return s; }
    virtual void drawSpecifics();

    virtual void odeStep(u32 iStep);

    virtual ode::IIrrOdeEvent *writeEvent();
    virtual ode::eEventWriterType getEventWriterType();
};

#endif


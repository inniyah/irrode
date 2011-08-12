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

class CHeli : public CAeroVehicle {
  protected:
    bool m_bLeft;

  public:
    CHeli(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit, irrklang::ISoundEngine *pSndEngine);
    virtual ~CHeli();

    virtual u32 update();

    virtual stringc &getButtonText() { static stringc s=stringc("select helicopter"); return s; }
    virtual void drawSpecifics();

    virtual void odeStep(u32 iStep);
};

#endif


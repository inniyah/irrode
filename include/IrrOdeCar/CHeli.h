#ifndef _C_HELICOPTER
  #define _C_HELICOPTER

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

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
  eHeliToggleCam
};

class CProjectile;

class CHeli : public CIrrOdeCarState, public IEventReceiver, public IIrrOdeEventListener {
  protected:
    f32 m_fYaw,m_fPitch,m_fRoll,m_iThrust,m_fThrust;
    bool m_bLeft,m_bMissileCam;

    IGUIStaticText *m_pInfo;

    irr::ode::CIrrOdeWorld *m_pWorld;
    irr::ode::CIrrOdeBody *m_pHeliBody;

    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::ode::CIrrOdeTorqueMotor *m_pTorque;
    irr::ode::CIrrOdeAeroDrag *m_pAero;

    ICameraSceneNode *m_pCam;
    ITerrainSceneNode *m_pTerrain;

    const u32 *m_pCtrls;

  public:
    CHeli(IrrlichtDevice *pDevice, CIrrCC *pCtrl);
    virtual ~CHeli();

    virtual void activate();
    virtual void deactivate();
    virtual u32 update();

    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }
};

#endif


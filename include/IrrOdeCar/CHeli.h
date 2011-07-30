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

class CHeli : public CIrrOdeCarState, public IEventReceiver, public IIrrOdeEventListener {
  protected:
    f32 m_fYaw,m_fPitch,m_fRoll,m_iThrust,m_fThrust,m_fLookAngle;
    bool m_bLeft,m_bMissileCam,m_bBackView,m_bInternal;

    IGUIStaticText *m_pInfo,
                   *m_pApInfo;
    IGUITab *m_pTab;

    irr::ode::CIrrOdeWorld *m_pWorld;
    irr::ode::CIrrOdeBody *m_pHeliBody;

    irr::ode::CIrrOdeImpulseMotor *m_pMotor;
    irr::ode::CIrrOdeTorqueMotor *m_pTorque;
    irr::ode::CIrrOdeAeroDrag *m_pAero;
    irr::ode::CIrrOdeGeomRay *m_pRay;

    ICameraSceneNode *m_pCam;
    ITerrainSceneNode *m_pTerrain;

    CAutoPilot *m_pAutoPilot;
    CTargetSelector *m_pTargetSelector;

    const u32 *m_pCtrls;
    u32 m_iLastShot,
        m_iShotsFired;
    s32 m_iNextCp;
    f32 m_fApDist;

    irr::core::array<irr::scene::ISceneNode *> m_aCheckPoints;

    CCockpitPlane *m_pCockpit;

  public:
    CHeli(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, CCockpitPlane *pCockpit);
    virtual ~CHeli();

    virtual void activate();
    virtual void deactivate();
    virtual u32 update();

    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual stringc &getButtonText() { static stringc s=stringc("select helicopter"); return s; }

    virtual void drawSpecifics();
};

#endif


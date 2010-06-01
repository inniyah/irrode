#ifndef _C_TANK
  #define _C_TANK

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

enum eTankCtrl {
  eTankForeward,
  eTankBackward,
  eTankLeft,
  eTankRight,
  eTankCannonLeft,
  eTankCannonRight,
  eTankCannonUp,
  eTankCannonDown,
  eTankFire,
  eTankCamTurret,
  eTankCamBullet,
  eTankFlip,
  eTankFastCollision
};

class CProjectile;

class CTank : public CIrrOdeCarState, public IEventReceiver, public irr::ode::IIrrOdeEventListener {
  protected:
    bool m_bBrake,
         m_bFlip,
         m_bFollowTurret,
         m_bFastCollision,
         m_bFire,
         m_bFollowBullet,
         m_bActive;
    ITexture *m_pTextures[2];

    IGUIStaticText *m_pInfo;
    f32 m_fSteer,m_fActSteer;

    CIrrOdeWorld *m_pWorld;
    CIrrOdeMotor *m_pMotor[4];
    CIrrOdeBody *m_pTankBody,*m_pTurret,*m_pCannon;
    CIrrOdeServo *m_pTurretServo,*m_pCannonServo;

    ICameraSceneNode *m_pCam;

    const u32 *m_pCtrls;

    int getSteer();
    int getAcc  ();

  public:
    CTank(IrrlichtDevice *pDevice, CIrrCC *pCtrl);
    virtual ~CTank();

    virtual void activate();
    virtual void deactivate();
    virtual u32 update();

    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }
};

#endif

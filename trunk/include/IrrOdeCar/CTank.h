#ifndef _C_TANK
  #define _C_TANK

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

namespace irrklang {
  class ISound;
}

using namespace irr;

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

class CTank : public CIrrOdeCarState, public IEventReceiver, public irr::ode::IIrrOdeEventListener, public ode::IIrrOdeEventWriter {
  protected:
    bool m_bBrake,
         m_bFollowTurret,
         m_bFastCollision,
         m_bFollowBullet;
    ITexture *m_pTextures[2];

    IGUIStaticText *m_pInfo;
    f32 m_fCannonAngle,
        m_fTurretAngle;

    ode::CIrrOdeWorld *m_pWorld;
    ode::CIrrOdeMotor *m_pMotor[4];
    ode::CIrrOdeBody *m_pTankBody,*m_pTurret,*m_pCannon;
    ode::CIrrOdeMotor *m_pTurretMotor;
    ode::CIrrOdeServo *m_pCannonServo;

    scene::ICameraSceneNode *m_pCam;

    list<irr::ode::CIrrOdeJointHinge *> m_lAxes;
    irr::ode::CIrrOdeJointHinge *m_pCannonHinge,
                                *m_pTurretHinge;
    irr::s8 m_aAxesAngles[4];

    const u32 *m_pCtrls;
    u32 m_iLastShot;
    f32 m_fSound;

    int getSteer();
    int getAcc  ();

  public:
    CTank(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl, irrklang::ISoundEngine *pSndEngine);
    virtual ~CTank();

    virtual void activate();
    virtual void deactivate();
    virtual u32 update();

    virtual bool OnEvent(const SEvent &event);

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual stringc &getButtonText() { static stringc s=stringc("select tank"); return s; }

    virtual void drawSpecifics() { }

    virtual ode::IIrrOdeEvent *writeEvent();
    virtual ode::eEventWriterType getEventWriterType();
};

#endif

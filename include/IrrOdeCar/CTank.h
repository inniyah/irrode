#ifndef _C_TANK
  #define _C_TANK

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

namespace irrklang {
  class ISound;
}

class CProjectile;

class CTank : public CIrrOdeCarState, public irr::ode::IIrrOdeEventWriter {
  protected:
    bool m_bBrake,
         m_bFollowTurret,
         m_bFastCollision,
         m_bFollowBullet;
    irr::video::ITexture *m_pTextures[2];

    irr::f32 m_fCannonAngle,
             m_fTurretAngle;

    irr::ode::CIrrOdeMotor *m_pMotor[4];
    irr::ode::CIrrOdeBody *m_pTankBody,*m_pTurret,*m_pCannon;
    irr::ode::CIrrOdeMotor *m_pTurretMotor;
    irr::ode::CIrrOdeServo *m_pCannonServo;

    irr::core::list<irr::ode::CIrrOdeJointHinge *> m_lAxes;
    irr::ode::CIrrOdeJointHinge *m_pCannonHinge,
                                *m_pTurretHinge;
    irr::s8 m_aAxesAngles[4];

    irr::u32 m_iLastShot;
    irr::f32 m_fSound;

    int getSteer();
    int getAcc  ();

  public:
    CTank(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, irr::ode::IIrrOdeEventQueue *pInputQueue);
    virtual ~CTank();

    virtual void activate();
    virtual void deactivate();
    virtual irr::u32 update();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"tank"; return s; }

    virtual void drawSpecifics() { }

    virtual irr::ode::IIrrOdeEvent *writeEvent();
    virtual irr::ode::eEventWriterType getEventWriterType();
    virtual irr::ode::CIrrOdeBody *getBody();

    virtual enumStateType getType() { return eStateTank; }
};

#endif

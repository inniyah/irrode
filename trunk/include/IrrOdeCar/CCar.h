#ifndef _C_CAR
  #define _C_CAR

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

class CIrrCC;
class CRearView;
class CCockpitCar;
class CIrrOdeCarTrack;
class CAdvancedParticleSystemNode;

enum eCarCtrl {
  eCarForeward,
  eCarBackward,
  eCarLeft,
  eCarRight,
  eCarFlip,
  eCarDifferential,
  eCarShiftUp,
  eCarShiftDown
};

class CCar : public CIrrOdeCarState, public irr::ode::IIrrOdeEventListener, public irr::ode::IIrrOdeEventWriter {
  protected:
    class CGearBox {
      protected:
        irr::f32 m_fVelocity[4],
                 m_fForce   [4];

        irr::ode::CIrrOdeMotor      *m_pMotor[2];
        irr::ode::CIrrOdeJointHinge *m_pAxesRear[2];
        irr::f32 m_fRpm,
                 m_fDiff,
                 m_fThrottle;
        irr::s8 m_iGear,
                m_iClutch;

        bool m_bDataChanged,
             m_bDifferential;   /*!< differential gear enabled? */

      public:
        CGearBox(irr::ode::CIrrOdeMotor *pMotor[2], irr::ode::CIrrOdeJointHinge *pAxesRear[2]);

        bool shiftUp();
        bool shiftDown();

        void update(irr::f32 fThrottle);

        irr::s8 getGear();
        irr::f32 getMaxVelocity();
        irr::f32 getRpm() { return m_fRpm; }
        irr::f32 getDiff() { return m_fDiff; }

        bool dataChanged();
        bool exhaustSmoke();
        bool differential();

        void toggleDifferential();
    };

    bool     m_bBrake,           /*!< is the handbrake active? */
             m_bHelp,            /*!< is the help screen visible? */
             m_bInternal,        /*!< internal view active? */
             m_bGasStation,      /*!< is the car in a gas station? */
             m_bGasLastStep,     /*!< was the car in a gas station in the last step? */
             m_bTouch;           /*!< at least one of the wheels has ground contact */
    irr::f32 m_fOldVel,          /*!< old velocity */
             m_fSteer,           /*!< the steering angle */
             m_fSpeed;           /*!< the speed of the car (for the cockpit) */
    irr::s32 m_iThrottle,        /*!< position of the throttle */
             m_iBodyId;          /*!< id of the car body */

    irr::core::vector3df m_vSuspNeutral;   /*!< the neutral position of the suspension */

    irr::ode::CIrrOdeBody              *m_pCarBody,        /*!< the car's body */
                                       *m_pSuspension,     /*!< the suspension */
                                       *m_pRearWheels[2],  /*!< the rear wheel bodies */
                                       *m_pFrontWheels[2]; /*!< the front wheel bodies */
    irr::ode::CIrrOdeJointSlider       *m_pJointSus;       /*!< the supension joint */
    irr::ode::CIrrOdeMotor             *m_pMotor    [2],   /*!< the motors attached to the rear wheels */
                                       *m_pBrkFr    [2],   /*!< front wheel brakes */
                                       *m_pBrkRe    [2];   /*!< rear wheel brakes */
    irr::ode::CIrrOdeServo             *m_pServo    [2];   /*!< the servos attached to the front wheels */
    irr::ode::CIrrOdeGeomSphere        *m_pWheels   [4];   /*!< the wheel geoms */
    irr::ode::CIrrOdeJointHinge2       *m_pAxesFront[2];   /*!< front left axis for speed measure */
    irr::ode::CIrrOdeJointHinge        *m_pAxesRear [2];   /*!< the rear axes */
    irr::ode::CIrrOdeSurfaceParameters *m_pParams   [4];   /*!< the contact parameters */

    CCockpitCar *m_pCockpit;
    CRearView *m_pRView;

    const irr::u32 *m_pCtrls;
    irr::f32 m_fSound,
             m_fOldSlider;

    irr::core::vector3df m_vOldSpeed;

    CIrrOdeCarTrack *m_pLap;

    CGearBox *m_pGearBox;

    void applyAeroEffect();

  public:
    CCar(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CIrrCC *pCtrl, CRearView *pRView);    /*!< the car's constructor */
    virtual ~CCar();                                                    /*!< the car's destructor */

    virtual void activate();      /*!< the activation method */
    virtual void deactivate();    /*!< the deactivation method */
    virtual irr::u32 update();         /*!< the update method called once per frame */

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    void setCtrl(const irr::u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"car"; return s; }

    virtual void drawSpecifics() { }

    virtual irr::ode::IIrrOdeEvent *writeEvent();
    virtual irr::ode::eEventWriterType getEventWriterType();

    virtual irr::ode::CIrrOdeBody *getBody() { return m_pCarBody; }
    void setCockpit(CCockpitCar *p) { m_pCockpit = p; }
};

#endif

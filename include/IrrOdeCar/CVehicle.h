#ifndef _C_VEHICLE
  #define _C_VEHICLE

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <CIrrOdeCarState.h>

class IRenderToTexture;
class CIrrOdeCarTrack;
class CTargetSelector;
class CProjectile;
class CAutoPilot;

class CVehicle : public irr::ode::IIrrOdeEventListener {
  private:
    class CCar : public CIrrOdeCarState, public irr::ode::IIrrOdeEventWriter {
      protected:
        class CGearBox {
          protected:
            irr::f32 m_fVelocity[5],
                     m_fForce   [5];

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

            void update(irr::f32 fThrottle, bool bBoost);

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
                 m_bTouch,           /*!< at least one of the wheels has ground contact */
                 m_bAdapt,           /*!< adaptive steer on / off */
                 m_bBoost,           /*!< boost on / off */
                 m_bFlip;
        irr::f32 m_fOldVel,          /*!< old velocity */
                 m_fSteer,           /*!< the steering angle */
                 m_fSpeed,           /*!< the speed of the car (for the cockpit) */
                 m_fAngle,           /*!< the sterring angle (for adaptive steer) */
                 m_fCtrlSteer,       /*!< control of steering */
                 m_fThrottle,        /*!< position of the throttle */
                 m_fSound,
                 m_fOldSlider;
        irr::s32 m_iBodyId,          /*!< id of the car body */
                 m_iBoost;           /*!< remaining boost */

        irr::core::vector3df m_vSuspNeutral,   /*!< the neutral position of the suspension */
                             m_vWheelOld[2];   /*!< old position of the wheels */

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

        irr::core::vector3df m_vOldSpeed;

        CIrrOdeCarTrack *m_pLap;

        CGearBox *m_pGearBox;

        void applyAeroEffect();

      public:
        CCar(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, irr::ode::IIrrOdeEventQueue *pInputQueue);    /*!< the car's constructor */
        virtual ~CCar();                                                                         /*!< the car's destructor */

        virtual void activate();      /*!< the activation method */
        virtual void deactivate();    /*!< the deactivation method */
        virtual irr::u32 update();         /*!< the update method called once per frame */

        virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
        virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

        virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"car"; return s; }

        virtual void drawSpecifics() { }

        virtual irr::ode::IIrrOdeEvent *writeEvent();
        virtual irr::ode::eEventWriterType getEventWriterType();

        virtual irr::ode::CIrrOdeBody *getBody() { return m_pCarBody; }

        virtual enumStateType getType() { return eStateCar; }
    };

    class CAeroVehicle : public CIrrOdeCarState, public irr::ode::IIrrOdeEventWriter {
      protected:
        irr::f32 m_fPitch,
                 m_fRoll,
                 m_fYaw,
                 m_fThrust,
                 m_fCamAngleH,
                 m_fCamAngleV;

        irr::s32 m_iNextCp,
                 m_aCtrlBuffer[0xFF];

        irr::u32 m_iLastShot1,
                 m_iLastShot2,
                 m_iShotsFired;

        bool m_bFirePrimary,
             m_bFireSecondary,
             m_bDataChanged,
             m_bBrakes,
             m_bFlip;

        irr::ode::CIrrOdeBody *m_pBody;

        irr::ode::CIrrOdeImpulseMotor *m_pMotor;
        irr::ode::CIrrOdeTorqueMotor  *m_pTorque;
        irr::ode::CIrrOdeAeroDrag     *m_pAero;
        irr::ode::CIrrOdeGeomRay      *m_pRay;
        irr::ode::CIrrOdeServo        *m_pSteer;
        irr::ode::CIrrOdeMotor        *m_pBrakes[2];

        irr::scene::ITerrainSceneNode *m_pTerrain;

        CAutoPilot *m_pAutoPilot;
        CTargetSelector *m_pTargetSelector;

        irr::core::array<irr::scene::ISceneNode *> m_aCheckPoints;

        void postShotEvent();

      public:
        CAeroVehicle(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, irr::ode::IIrrOdeEventQueue *pInputQueue);
        virtual ~CAeroVehicle();

        virtual void activate();
        virtual void deactivate();
        //virtual u32 update();

        virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
        virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

        virtual void odeStep(irr::u32 iStep)=0;

        irr::ode::CIrrOdeBody *getBody() { return m_pBody; }

        virtual void incHitsScored();
        virtual void incHitsTaken();
        void incShotsFired();
    };

    class CPlane : public CAeroVehicle {
      protected:
        bool m_bLeftMissile,
             m_bAutoPilot;
        irr::ode::CIrrOdeJointHinge *m_pAxes[2];
        irr::ode::CIrrOdeJointHinge2 *m_pSteerAxis;
        irr::f32 m_fAngleRate[3];

        CIrrOdeCarTrack *m_pLap;

      public:
        CPlane(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, irr::ode::IIrrOdeEventQueue *pInputQueue);
        virtual ~CPlane();

        virtual void odeStep(irr::u32 iStep);

        virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"plane"; return s; }

        virtual void drawSpecifics();

        virtual irr::ode::IIrrOdeEvent *writeEvent();
        virtual irr::ode::eEventWriterType getEventWriterType();

        virtual enumStateType getType() { return eStatePlane; }
    };

    class CHeli : public CAeroVehicle {
      protected:
        bool m_bLeft;
        irr::f32 m_fSound;
        irr::s32 m_iNodeId;

      public:
        CHeli(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, irr::ode::IIrrOdeEventQueue *pInputQueue);
        virtual ~CHeli();

        virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"heli"; return s; }

        virtual void drawSpecifics();

        virtual void odeStep(irr::u32 iStep);

        virtual irr::ode::IIrrOdeEvent *writeEvent();
        virtual irr::ode::eEventWriterType getEventWriterType();

        virtual enumStateType getType() { return eStateHeli; }
    };

    class CTank : public CIrrOdeCarState, public irr::ode::IIrrOdeEventWriter {
      protected:
        bool m_bBrake,
             m_bFollowTurret,
             m_bFastCollision,
             m_bFlip,
             m_bFire;
        irr::video::ITexture *m_pTextures[2];

        irr::f32 m_fCannonAngle,
                 m_fTurretAngle,
                 m_fThrottle,
                 m_fSteer,
                 m_fCannonLeft,
                 m_fCannonUp;

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

    irr::IrrlichtDevice *m_pDevice;
    irr::scene::ISceneManager *m_pSmgr;
    irr::ode::CIrrOdeWorld *m_pWorld;
    irr::ode::IIrrOdeEventQueue *m_pInputQueue;

    irr::core::list<irr::scene::ISceneNode *> m_lCars,
                                              m_lPlanes,
                                              m_lHelis,
                                              m_lTanks;
    irr::core::list<CIrrOdeCarState        *> m_lVehicles;

    void removeFromScene(irr::scene::ISceneNode *pNode, irr::ode::CIrrOdeWorld *pWorld);
    void fillBodyList(irr::core::list<irr::scene::ISceneNode *> &aVehicles, irr::scene::ISceneNode *pNode, const irr::c8 *sClassName, irr::u32 iMax, irr::ode::CIrrOdeWorld *pWorld);

  public:
    CVehicle(irr::IrrlichtDevice *pDevice, irr::u32 iNumCars, irr::u32 iNumPlanes, irr::u32 iNumHelis, irr::u32 iNumTanks, irr::ode::CIrrOdeWorld *pWorld, bool bRearView, irr::ode::IIrrOdeEventQueue *pInputQueue);
    ~CVehicle();

    const irr::core::list<CIrrOdeCarState *>getVehicles() {
      return m_lVehicles;
    }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
};

#endif

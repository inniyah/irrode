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
  eCarBrake,
  eCarBoost,
  eCarFlip,
  eCarToggleAdaptiveSteer,
  eCarInternal,
  eCarCamLeft,
  eCarCamRight,
  eCarCamUp,
  eCarCamDown,
  eCarCamCenter,
  eCarDifferential
};

class CCar : public CIrrOdeCarState, public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener, public irr::ode::IIrrOdeEventWriter {
  protected:
    bool m_bBrake,           /*!< is the handbrake active? */
         m_bBoost,           /*!< is the boos button pushed? */
         m_bAdaptSteer,      /*!< is the adaptive steer mode active? */
         m_bHelp,            /*!< is the help screen visible? */
         m_bInternal,        /*!< internal view active? */
         m_bDifferential,    /*!< differential gear enabled? */
         m_bGasStation,      /*!< is the car in a gas station? */
         m_bGasLastStep;     /*!< was the car in a gas station in the last step? */
    irr::f32 m_fActSteer,    /*!< the actual steering (-45.0, 0, +45.0) */
             m_fCamAngleH,   /*!< horizontal angle of camera */
             m_fCamAngleV,   /*!< vertical angle of camera */
             m_fOldVel,      /*!< old velocity */
             m_fSteer,       /*!< the steering angle */
             m_fSpeed;
    irr::s32 m_iThrottle,    /*!< position of the throttle */
             m_iBodyId,      /*!< id of the car body */
             m_iNextCp,      /*!< checkpoint for racetrack laps */
             m_iCurStep,
             m_iLastLapStep;

    irr::gui::IGUIStaticText *m_pInfo;    /*!< the info text (with speed...) */

    irr::core::vector3df m_vSuspNeutral;   /*!< the neutral position of the suspension */

    irr::ode::CIrrOdeBody              *m_pCarBody,        /*!< the car's body */
                                  *m_pSuspension,     /*!< the suspension */
                                  *m_pRearWheels[2];  /*!< the rear wheel bodies */
    irr::ode::CIrrOdeJointSlider       *m_pJointSus;       /*!< the supension joint */
    irr::ode::CIrrOdeMotor             *m_pMotor    [2],   /*!< the motors attached to the rear wheels */
                                  *m_pBrkFr    [2],   /*!< front wheel brakes */
                                  *m_pBrkRe    [2];   /*!< rear wheel brakes */
    irr::ode::CIrrOdeServo             *m_pServo    [2];   /*!< the servos attached to the front wheels */
    irr::ode::CIrrOdeGeomSphere        *m_pWheels   [4];   /*!< the wheel geoms */
    irr::ode::CIrrOdeJointHinge2       *m_pAxesFront[2];   /*!< front left axis for speed measure */
    irr::ode::CIrrOdeJointHinge        *m_pAxesRear [2];   /*!< the rear axes */
    irr::ode::CIrrOdeSurfaceParameters *m_pParams   [4];   /*!< the contact parameters */

    irr::scene::ICameraSceneNode *m_pCam; /*!< the camera scene node */

    CCockpitCar *m_pCockpit;
    CRearView *m_pRView;

    const irr::u32 *m_pCtrls;
    irr::f32 m_fSound,
        m_fRpm,
        m_fDiff,
        m_fOldSlider;

    irr::core::vector3df m_vOldSpeed;

    CIrrOdeCarTrack *m_pLap;

  public:
    CCar(irr::IrrlichtDevice *pDevice, irr::scene::ISceneNode *pNode, CIrrCC *pCtrl,CCockpitCar *pCockpit, CRearView *pRView);    /*!< the car's constructor */
    virtual ~CCar();                                                    /*!< the car's destructor */

    virtual void activate();      /*!< the activation method */
    virtual void deactivate();    /*!< the deactivation method */
    virtual irr::u32 update();         /*!< the update method called once per frame */

    virtual bool OnEvent(const irr::SEvent &event);  /*!< the Irrlicht event receiver callback */

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    void setCtrl(const irr::u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"car"; return s; }

    virtual void drawSpecifics() { }

    virtual irr::ode::IIrrOdeEvent *writeEvent();
    virtual irr::ode::eEventWriterType getEventWriterType();
};

#endif

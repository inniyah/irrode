#ifndef _C_CAR
  #define _C_CAR

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

class CIrrCC;
class CRearView;
class CCockpitCar;
class CAdvancedParticleSystemNode;

using namespace irr;

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

class CCar : public CIrrOdeCarState, public IEventReceiver, public ode::IIrrOdeEventListener, public ode::IIrrOdeEventWriter {
  protected:
    bool m_bBrake,         /*!< is the handbrake active? */
         m_bBoost,         /*!< is the boos button pushed? */
         m_bAdaptSteer,    /*!< is the adaptive steer mode active? */
         m_bHelp,          /*!< is the help screen visible? */
         m_bInternal,      /*!< internal view active? */
         m_bDifferential,  /*!< differential gear enabled? */
         m_bGasStation,    /*!< is the car in a gas station? */
         m_bGasLastStep;   /*!< was the car in a gas station in the last step? */
    f32 m_fActSteer,    /*!< the actual steering (-45.0, 0, +45.0) */
        m_fCamAngleH,   /*!< horizontal angle of camera */
        m_fCamAngleV,   /*!< vertical angle of camera */
        m_fOldVel,      /*!< old velocity */
        m_fSteer;       /*!< the steering angle */
    s32 m_iThrottle,    /*!< position of the throttle */
        m_iBodyId,      /*!< id of the car body */
        m_iNextCp,      /*!< checkpoint for racetrack laps */
        m_iCurStep,
        m_iLastLapStep;

    IGUIStaticText *m_pInfo;    /*!< the info text (with speed...) */

    gui::IGUITab *m_pTab;

    core::vector3df m_vSuspNeutral;   /*!< the neutral position of the suspension */

    ode::CIrrOdeBody              *m_pCarBody,        /*!< the car's body */
                                  *m_pSuspension,     /*!< the suspension */
                                  *m_pRearWheels[2];  /*!< the rear wheel bodies */
    ode::CIrrOdeJointSlider       *m_pJointSus;       /*!< the supension joint */
    ode::CIrrOdeMotor             *m_pMotor    [2],   /*!< the motors attached to the rear wheels */
                                  *m_pBrkFr    [2],   /*!< front wheel brakes */
                                  *m_pBrkRe    [2];   /*!< rear wheel brakes */
    ode::CIrrOdeServo             *m_pServo    [2];   /*!< the servos attached to the front wheels */
    ode::CIrrOdeGeomSphere        *m_pWheels   [4];   /*!< the wheel geoms */
    ode::CIrrOdeJointHinge2       *m_pAxesFront[2];   /*!< front left axis for speed measure */
    ode::CIrrOdeJointHinge        *m_pAxesRear [2];   /*!< the rear axes */
    ode::CIrrOdeSurfaceParameters *m_pParams   [4];   /*!< the contact parameters */

    scene::ICameraSceneNode *m_pCam; /*!< the camera scene node */

    CCockpitCar *m_pCockpit;
    CRearView *m_pRView;

    const u32 *m_pCtrls;
    f32 m_fSound,
        m_fRpm,
        m_fDiff,
        m_fOldSlider;

    core::vector3df m_vOldSpeed;

  public:
    CCar(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl,CCockpitCar *pCockpit, CRearView *pRView);    /*!< the car's constructor */
    virtual ~CCar();                                                    /*!< the car's destructor */

    virtual void activate();      /*!< the activation method */
    virtual void deactivate();    /*!< the deactivation method */
    virtual u32 update();         /*!< the update method called once per frame */

    virtual bool OnEvent(const SEvent &event);  /*!< the Irrlicht event receiver callback */

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual const stringw &getButton() { static core::stringw s=L"car"; return s; }

    virtual void drawSpecifics() { }

    virtual ode::IIrrOdeEvent *writeEvent();
    virtual ode::eEventWriterType getEventWriterType();
};

#endif

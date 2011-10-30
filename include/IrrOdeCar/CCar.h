#ifndef _C_CAR
  #define _C_CAR

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

namespace irrklang {
  class ISound;
};

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

class CCar : public CIrrOdeCarState, public IEventReceiver, public ode::IIrrOdeEventListener {
  protected:
    bool m_bBrake,         /*!< is the handbrake active? */
         m_bBoost,         /*!< is the boos button pushed? */
         m_bAdaptSteer,    /*!< is the adaptive steer mode active? */
         m_bHelp,          /*!< is the help screen visible? */
         m_bInternal,      /*!< internal view active? */
         m_bDifferential;  /*!< differential gear enabled? */
    f32 m_fActSteer,    /*!< the actual steering (-45.0, 0, +45.0) */
        m_fCamAngleH,   /*!< horizontal angle of camera */
        m_fCamAngleV;   /*!< vertical angle of camera */
    s32 m_iThrottle;    /*!< position of the throttle */

    IGUIStaticText *m_pInfo;    /*!< the info text (with speed...) */

    gui::IGUITab *m_pTab;

    CAdvancedParticleSystemNode *m_pSmoke[2];

    ode::CIrrOdeMotor *m_pMotor[2],  /*!< the motors attached to the rear wheels */
                      *m_pBrkFr[2],  /*!< front wheel brakes */
                      *m_pBrkRe[2];  /*!< rear wheel brakes */
    ode::CIrrOdeServo *m_pServo[2];  /*!< the servos attached to the front wheels */
    ode::CIrrOdeBody *m_pCarBody;    /*!< the car's body */

    ode::CIrrOdeJointHinge2 *m_pAxesFront[2]; /*!< front left axis for speed measure */
    ode::CIrrOdeJointHinge *m_pAxesRear[2];

    scene::ICameraSceneNode *m_pCam; /*!< the camera scene node */

    CCockpitCar *m_pCockpit;
    CRearView *m_pRView;

    irrklang::ISound *m_pSound;

    const u32 *m_pCtrls;
    f32 m_fSound,
        m_fRpm,
        m_fDiff;

    core::vector3df m_vOldSpeed;

  public:
    CCar(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl,CCockpitCar *pCockpit, CRearView *pRView, irrklang::ISoundEngine *pSndEngine);    /*!< the car's constructor */
    virtual ~CCar();                                                    /*!< the car's destructor */

    virtual void activate();      /*!< the activation method */
    virtual void deactivate();    /*!< the deactivation method */
    virtual u32 update();         /*!< the update method called once per frame */

    virtual bool OnEvent(const SEvent &event);  /*!< the Irrlicht event receiver callback */

    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual stringc &getButtonText() { static stringc s=stringc("select car"); return s; }

    virtual void drawSpecifics() { }
};

#endif

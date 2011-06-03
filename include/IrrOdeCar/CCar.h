#ifndef _C_CAR
  #define _C_CAR

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

class CIrrCC;

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

enum eCarCtrl {
  eCarForeward,
  eCarBackward,
  eCarLeft,
  eCarRight,
  eCarBrake,
  eCarBoost,
  eCarFlip,
  eCarToggleAdaptiveSteer,
  eCarBackview
};

class CCar : public CIrrOdeCarState, public IEventReceiver, public IIrrOdeEventListener {
  protected:
    bool m_bBrake,      /*!< is the handbrake active? */
         m_bBoost,      /*!< is the boos button pushed? */
         m_bAdaptSteer, /*!< is the adaptive steer mode active? */
         m_bHelp;       /*!< is the help screen visible? */
    f32 m_fActSteer;    /*!< the actual steering (-45.0, 0, +45.0) */
    s32 m_iThrottle;    /*!< position of the throttle */

    IGUIStaticText *m_pInfo;    /*!< the info text (with speed...) */

    CIrrOdeMotor *m_pMotor[2];  /*!< the motors attached to the rear wheels */
    CIrrOdeServo *m_pServo[2];  /*!< the servos attached to the front wheels */
    CIrrOdeBody *m_pCarBody;    /*!< the car's body */

    ICameraSceneNode *m_pCam; /*!< the camera scene node */

    const u32 *m_pCtrls;

  public:
    CCar(IrrlichtDevice *pDevice, ISceneNode *pNode, CIrrCC *pCtrl);    /*!< the car's constructor */
    virtual ~CCar();                                                    /*!< the car's destructor */

    virtual void activate();      /*!< the activation method */
    virtual void deactivate();    /*!< the deactivation method */
    virtual u32 update();         /*!< the update method called once per frame */

    virtual bool OnEvent(const SEvent &event);  /*!< the Irrlicht event receiver callback */

    virtual bool onEvent(IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(IIrrOdeEvent *pEvent);

    void setCtrl(const u32 *pCtrl) { m_pCtrls=pCtrl; }

    virtual stringc &getButtonText() { static stringc s=stringc("select car"); return s; }

    virtual void drawSpecifics() { }
};

#endif

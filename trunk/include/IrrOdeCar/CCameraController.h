#ifndef _C_CAMERA_CONTROLLER
  #define _C_CAMERA_CONTROLLER

  #include <irrlicht.h>
  #include <irrklang.h>
  #include <IrrOde.h>

namespace irr {
  namespace ode {
    class CIrrOdeBody;
  }
}

enum eCameraControls {
  eCameraLeft,
  eCameraRight,
  eCameraUp,
  eCameraDown,
  eCameraCenter,
  eCameraInternal
};

class CIrrCC;

class CCameraController : public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  private:
    irrklang::ISoundEngine *m_pSndEngine;
    irr::IrrlichtDevice *m_pDevice;
    irr::scene::ICameraSceneNode *m_pCam;
    irr::scene::ISceneManager *m_pSmgr;
    irr::ode::CIrrOdeBody *m_pTarget;

    irr::ode::CIrrOdeManager *m_pOdeMngr;

    irr::f32 m_fCamAngleV,
             m_fCamAngleH,
             m_fExtDist,
             m_fExtOffset,
             m_fExtFact;

    irr::core::dimension2du m_cScreen;
    irr::core::position2di m_cMousePos;
    irr::gui::ICursorControl *m_pCursor;

    irr::core::vector3df m_vInternalOffset,
                         m_vDirection,
                         m_vPosition,
                         m_vTarget,
                         m_vUp;

    bool m_bInternal,
         m_bRotateXY,
         m_bLeftMouse,
         m_bRghtMouse;

    CIrrCC *m_pController;

    const irr::u32 *m_pCtrls;

  public:
    CCameraController(irr::IrrlichtDevice *pDevice, irrklang::ISoundEngine *pSndEngine, CIrrCC *pCtrl, irr::ode::CIrrOdeManager *pOdeMngr);
    virtual ~CCameraController();

    void setTarget(irr::ode::CIrrOdeBody *pTarget);

    void update();

    virtual bool OnEvent(const irr::SEvent &event);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    void setCtrl(const irr::u32 *pCtrl) { m_pCtrls=pCtrl; }
};

#endif

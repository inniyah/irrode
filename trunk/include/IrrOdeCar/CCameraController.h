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
  eCameraInternal,
  eCameraButtonMove,
  eCameraNearFocus
};

class CIrrCC;
class CRearView;

class CCameraController : public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  private:
    irrklang::ISoundEngine *m_pSndEngine;
    irr::IrrlichtDevice *m_pDevice;
    irr::scene::ICameraSceneNode *m_pCam;
    irr::scene::ISceneManager *m_pSmgr;
    irr::ode::CIrrOdeBody *m_pTarget;
    CRearView *m_pRearView;

    irr::ode::CIrrOdeManager *m_pOdeMngr;

    irr::f32 m_fCamAngleV,
             m_fCamAngleH,
             m_fExtDist,
             m_fExtOffset,
             m_fExtFact,
             m_fTgtAngleV,
             m_fTgtAngleH,
             m_fInitialFOV,
             m_fInitialAR,
             m_fVrAr;

    irr::core::dimension2du m_cScreen;
    irr::core::position2di m_cMousePos;
    irr::gui::ICursorControl *m_pCursor;

    irr::core::vector3df m_vInternalOffset,
                         m_vDirection,
                         m_vPosition,
                         m_vTarget,
                         m_vUp,
                         m_vRViewPos,
                         m_vRViewTgt,
                         m_vRViewOffset;

    bool m_bInternal,
         m_bRotateXY,
         m_bLeftMouse,
         m_bRghtMouse,
         m_bButton,
         m_b3d,
         m_bLeft,
         m_bFocusNear,
         m_bVr;

    CIrrCC *m_pController;

    const irr::u32 *m_pCtrls;

  public:
    CCameraController(irr::IrrlichtDevice *pDevice, irrklang::ISoundEngine *pSndEngine, CIrrCC *pCtrl, irr::ode::CIrrOdeManager *pOdeMngr, irr::f32 fVrAr);
    virtual ~CCameraController();

    void setTarget(irr::ode::CIrrOdeBody *pTarget);

    void update();

    virtual bool OnEvent(const irr::SEvent &event);

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    void setCtrl(const irr::u32 *pCtrl) { m_pCtrls=pCtrl; }

    void set3d(bool b);
    void setVr(bool b);

    bool is3dEnabled() { return m_b3d; }
    bool isVrEnabled() { return m_bVr; }

    void setLeft() { m_bLeft = true; }

    void updateRearView();
};

#endif

#ifndef _C_CONTROL_RECEIVER
  #define _C_CONTROL_RECEIVER

  #include <irrlicht.h>
  #include <IrrOde.h>

#ifndef NO_IRRKLANG
namespace irrklang {
  class ISoundEngine;
}
#else
namespace irrklang {
  typedef void ISoundEngine;
}
#endif

class CCameraController;
class CIrrOdeCarState;
class CController;
class CIrrCC;
class CMenu;

class CControlReceiver : public irr::IEventReceiver, public irr::ode::IIrrOdeEventListener {
  private:
    enum eContolledVehicle {
      eControlNone,
      eControlCar,
      eControlPlane,
      eControlHeli,
      eControlTank
    };

    irr::IrrlichtDevice *m_pDevice;

    irr::s32 m_iNode,
             m_iClient;
    irr::u32 m_iCtrls[4][32],  //an array for all the controls we are going to define
             m_iCount,
             m_iLastEvent;
    irr::ITimer *m_pTimer;
    irr::core::array<CIrrOdeCarState *> m_aStates;
    irr::gui::IGUIStaticText  *m_pRecording,
                              *m_pSaveFile;

    irr::ode::CIrrOdeRecorder   *m_pRecorder;
    irr::ode::CIrrOdeWorld      *m_pWorld;

    irrklang::ISoundEngine *m_pSndEngine;

    eContolledVehicle m_eVehicle;

    CIrrOdeCarState *m_pActive;
    CIrrCC          *m_pController;
    CMenu           *m_pMenu;

    bool m_bUpdate;

    CCameraController *m_pCamCtrl;
    CController       *m_pCtrlDialog;

    void initControls();
    void initWorld(irr::scene::ISceneNode *pNode);
    void updateVehicle();

    void switchToState(irr::s32 iNewState);
  public:
    CControlReceiver(irr::IrrlichtDevice *pDevice, irrklang::ISoundEngine *pSndEngine, irr::f32 fVrAr);
    ~CControlReceiver();

    void createMenu(irr::u32 iCars, irr::u32 iPlanes, irr::u32 iHelis, irr::u32 iTanks, bool bRearCam);

    void setControlledVehicle(irr::s32 iType, irr::s32 iNode);
    void start();
    void update();

    CIrrCC *getController();

    virtual bool OnEvent(const irr::SEvent &event);

    const irr::u32 **getCtrls() { return (const irr::u32 **)(m_iCtrls); }

    void removeFromScene(irr::scene::ISceneNode *pNode);
    void removeFromScene(const irr::c8 *sName, irr::scene::ISceneManager *smgr);

    void drawSpecifics();

    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);

    void connect();

    CCameraController *getCameraController() { return m_pCamCtrl; }
};

#endif

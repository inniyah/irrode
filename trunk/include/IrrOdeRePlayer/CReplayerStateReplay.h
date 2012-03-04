  #include <irrlicht.h>
  #include <IState.h>

  #include <IrrOde.h>
  #include <observer/CIrrOdeRePlayer.h>

class CReplayerStateReplay : public irr::ode::IIrrOdeEventListener, public IState, public irr::IEventReceiver {
  protected:
    enum eCameraMode {
      eCamFree,
      eCamFollow
    };

    irr::gui::IGUIStaticText *m_pLblBodies,
                             *m_pLblPaused,
                             *m_pLblFinished;
    irr::core::array<irr::ode::CIrrOdeBody *> m_aBodies;
    irr::ode::CIrrOdeManager *m_pOdeManager;
    irr::ode::CIrrOdeRePlayer *m_pPlayer;
    irr::IrrlichtDevice *m_pDevice;
    irr::gui::IGUIEnvironment *m_pGuiEnv;
    irr::gui::IGUIFont *m_pFont;
    irr::gui::ICursorControl *m_pCrsCtrl;
    irr::scene::ISceneNode *m_pFocusedNode;
    irr::scene::ISceneManager *m_pSmgr;
    irr::u32 m_iRet,
             m_iCamMode,
             m_iFocusedNode,
             m_iDirection,
             m_iPos;
    irr::f32 m_fCamDist,
             m_fCamHeight;
    irr::c8 m_sReplay[0xFF];
    eCameraMode m_eCamMode;

    irr::core::vector3df m_vCamTarget,
                         m_vCamRotation,
                         m_vCamMove;

    irr::scene::ICameraSceneNode *m_pCam,*m_pFreeCam;

    bool m_bSceneLoaded;

    void updateBodyList();
    void removeNode(irr::scene::ISceneNode *pNode);

  public:
    CReplayerStateReplay(irr::IrrlichtDevice *pDevice, const irr::c8 *sReplay);
    virtual ~CReplayerStateReplay() { }
    virtual void activate();
    virtual void deactivate();
    virtual irr::u32 update();
    virtual bool onEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *pEvent);
    virtual bool OnEvent(const irr::SEvent &event);
};


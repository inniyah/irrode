  #include <irrlicht.h>
  #include <IState.h>

  #include <IrrOde.h>
  #include <observer/CIrrOdeRePlayer.h>

class CReplayerStateReplay : public ode::IIrrOdeEventListener, public IState, public IEventReceiver {
  protected:
    enum eCameraMode {
      eCamFree,
      eCamFollow
    };

    irr::gui::IGUIStaticText *m_pLblBodies,
                             *m_pLblPaused,
                             *m_pLblFinished;
    array<ode::CIrrOdeBody *> m_aBodies;
    irr::ode::CIrrOdeManager *m_pOdeManager;
    irr::ode::CIrrOdeRePlayer *m_pPlayer;
    irr::IrrlichtDevice *m_pDevice;
    irr::gui::IGUIEnvironment *m_pGuiEnv;
    irr::gui::IGUIFont *m_pFont;
    irr::gui::ICursorControl *m_pCrsCtrl;
    irr::scene::ISceneNode *m_pFocusedNode;
    ISceneManager *m_pSmgr;
    u32 m_iRet,
        m_iCamMode,
        m_iFocusedNode;
    f32 m_fCamDist,
        m_fCamHeight;
    c8 m_sReplay[0xFF];
    eCameraMode m_eCamMode;

    core::vector3df m_vCamTarget,
                    m_vCamRotation,
                    m_vCamMove;

    irr::scene::ICameraSceneNode *m_pCam,*m_pFreeCam;

    bool m_bSceneLoaded;

    void updateBodyList();
    void removeNode(ISceneNode *pNode);

  public:
    CReplayerStateReplay(irr::IrrlichtDevice *pDevice, const c8 *sReplay);
    virtual ~CReplayerStateReplay() { }
    virtual void activate();
    virtual void deactivate();
    virtual u32 update();
    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool OnEvent(const SEvent &event);
};


  #include <irrlicht.h>
  #include <IState.h>

  #include <IrrOde.h>
  #include <observer/CIrrOdeRePlayer.h>

class CReplayerStateReplay : public ode::IIrrOdeEventListener, public IState {
  protected:
    IGUIStaticText *m_pLblBodies;
    array<ode::CIrrOdeBody *> m_aBodies;
    irr::ode::CIrrOdeManager *m_pOdeManager;
    irr::ode::CIrrOdeRePlayer *m_pPlayer;
    irr::IrrlichtDevice *m_pDevice;
    ISceneManager *m_pSmgr;
    u32 m_iRet;
    c8 m_sReplay[0xFF];

    bool m_bSceneLoaded;

    void updateBodyList();
    void removeNode(ISceneNode *pNode);

  public:
    CReplayerStateReplay(irr::IrrlichtDevice *pDevice, const c8 *sReplay);
    virtual void activate();
    virtual void deactivate();
    virtual u32 update();
    virtual bool onEvent(ode::IIrrOdeEvent *pEvent);
    virtual bool handlesEvent(ode::IIrrOdeEvent *pEvent);
};


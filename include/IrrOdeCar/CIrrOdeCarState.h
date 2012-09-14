#ifndef _I_STATE
  #define _I_STATE

  #include <irrlicht.h>
  #include <IrrOde.h>

enum enumStateType {
  eStateMenu,
  eStateCtrl,
  eStateCar,
  eStatePlane,
  eStateHeli,
  eStateTank
};

/**
 * This class is the base class for all states that are used in this demo. It has three virtual methods
 * that need to be overriden by the actual states, activate, deactivate and update.
 */
class CIrrOdeCarState : public irr::ode::IIrrOdeEventListener {
  protected:
    bool m_bSwitchToMenu,   /*!< return to menu state */
         m_bInitialized;

    irr::s32 m_iHitsScored,
             m_iHitsTaken,
             m_iControllerBy;

    irr::IrrlichtDevice *m_pDevice;    /*!< the Irrlicht device */
    irr::scene::ISceneManager *m_pSmgr;       /*!< the current Irrlicht scene manager */
    irr::gui::IGUIEnvironment *m_pGuiEnv;   /*!< the current Irrlicht GUI environment */
    irr::ode::IIrrOdeEventQueue *m_pInputQueue;

    irr::c8 m_sHelpFile[1024];

  public:
    /**
     * The constructor
     * @param pDevice the used Irrlicht device
     * @param sVehicleName the name of the state (e.g. helicopter)
     * @param sHelpFile the help file. The content of this file is shown in the help screen
     */
    CIrrOdeCarState(irr::IrrlichtDevice *pDevice, const wchar_t *sVehicleName, irr::ode::IIrrOdeEventQueue *pInputQueue);

    virtual ~CIrrOdeCarState();  /*!< destructor */

    virtual void activate()=0;    /*!< This method is called on activation of a state */
    virtual void deactivate()=0;  /*!< This method is called on deactivation of a state */
    virtual irr::u32 update();         /*!< This method is called once per render frame. Subclasses should call this method */

    virtual bool OnEvent(const irr::SEvent &event); /*!< Irrlicht event handler. Called by subclasses */

    bool isInitialized() { return m_bInitialized; }

    virtual void drawSpecifics()=0;

    virtual const irr::core::stringw &getButton()=0;

    virtual void incHitsScored() { m_iHitsScored++; }

    virtual void incHitsTaken() { m_iHitsTaken++; }

    virtual irr::ode::CIrrOdeBody *getBody() = 0;

    virtual enumStateType getType() = 0;

    void setControlledBy(irr::s32 i) { m_iControllerBy = i; }
    irr::s32 getControlledBy() { return m_iControllerBy; }
};

#endif

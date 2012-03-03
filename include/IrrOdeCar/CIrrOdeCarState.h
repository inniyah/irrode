#ifndef _I_STATE
  #define _I_STATE

  #include <irrlicht.h>
  #include <IrrOde.h>

class CIrrCC;

/**
 * This class is the base class for all states that are used in this demo. It has three virtual methods
 * that need to be overriden by the actual states, activate, deactivate and update.
 */
class CIrrOdeCarState {
  protected:
    bool m_bSwitchToMenu,   /*!< return to menu state */
         m_bHelp,           /*!< display the help screen */
         m_bActive,
         m_bInitialized;

    s32 m_iHitsScored,
        m_iHitsTaken;

    IrrlichtDevice *m_pDevice;    /*!< the Irrlicht device */
    ISceneManager *m_pSmgr;       /*!< the current Irrlicht scene manager */
    IGUIEnvironment *m_pGuiEnv;   /*!< the current Irrlicht GUI environment */

    IGUIStaticText *m_pHelp;    /*!< the help text */

    c8 m_sHelpFile[1024];

    CIrrCC *m_pController;

    core::vector3df m_vCamVelocity;

  public:
    /**
     * The constructor
     * @param pDevice the used Irrlicht device
     * @param sVehicleName the name of the state (e.g. helicopter)
     * @param sHelpFile the help file. The content of this file is shown in the help screen
     */
    CIrrOdeCarState(IrrlichtDevice *pDevice, const wchar_t *sVehicleName, const c8 *sHelpFile, CIrrCC *pCtrl);

    virtual ~CIrrOdeCarState() { }   /*!< destructor */

    virtual void activate()=0;    /*!< This method is called on activation of a state */
    virtual void deactivate()=0;  /*!< This method is called on deactivation of a state */
    virtual u32 update();         /*!< This method is called once per render frame. Subclasses should call this method */

    virtual bool OnEvent(const SEvent &event); /*!< Irrlicht event handler. Called by subclasses */

    void loadHelpFile();

    bool isInitialized() { return m_bInitialized; }

    virtual void drawSpecifics()=0;

    virtual const stringw &getButton()=0;

    const core::vector3df &getCameraVelocity() {
      return m_vCamVelocity;
    }

    void incHitsScored() { m_iHitsScored++; }

    void incHitsTaken() { m_iHitsTaken++; }
};

#endif

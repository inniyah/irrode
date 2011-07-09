#ifndef _I_STATE
  #define _I_STATE

  #include <irrlicht.h>
  #include <IrrOde.h>

class CIrrCC;

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace ode;

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

    IrrlichtDevice *m_pDevice;    /*!< the Irrlicht device */
    ISceneManager *m_pSmgr;       /*!< the current Irrlicht scene manager */
    IGUIEnvironment *m_pGuiEnv;   /*!< the current Irrlicht GUI environment */

    IGUIStaticText *m_pHelp,    /*!< the help text */
                   *m_pFps;     /*!< the FPS info text */

    c8 m_sHelpFile[1024];

    CIrrCC *m_pController;
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

    void setFpsInfo(gui::IGUIStaticText *pFps);

    bool isInitialized() { return m_bInitialized; }

    virtual stringc &getButtonText()=0;

    virtual void drawSpecifics()=0;
};

#endif

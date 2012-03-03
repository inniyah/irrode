#ifndef _C_SETTINGS
  #define _C_SETTINGS

  #include <irrlicht.h>

/**
 * @class CSettings
 * This class provides the "select graphics options" dialog that is displayed at program start
 */
class CSettings : public irr::IEventReceiver {
  private:
    /**
     * class CSettings::_VRES
     * This class is used to store the available screen modes
     */
    class _VRES {
      public:
        irr::s32 iWidth;   /**< width of the mode */
        irr::s32 iHeight;  /**< height of the mode */
        irr::s32 iBpp;     /**< bit depth of the mode */
    };

    irr::gui::IGUIComboBox *m_pResolution;  /**< combobox to pick the screen mode */
    irr::gui::IGUICheckBox *m_pFullscreen;  /**< checkbox to choose whether or not fullscreen is wanted */

    irr::core::array<irr::gui::IGUICheckBox *> m_aActiveBodies;
    irr::core::array<irr::gui::IGUIComboBox *> m_aBodyCount;

    bool m_aAct[7];
    irr::u32 m_iCnt[4];

    irr::gui::IGUIButton *m_pOk,            /**< the "OK" button */
                    *m_pCancel;        /**< the "Cancel" button */
    irr::video::SColor m_cBackground;       /**< the background color */

    irr::c8 m_sSettingsFile[1024]; /**< the settings file */
    irr::u32 m_iClose,             /**< is the "OK" or the "Cancel" button (or "ENTER" or "ESCAPE") pressed? */
        m_iResolution,        /**< the selected sceen mode */
        m_iDriver;            /**< the selected driver */
    bool m_bFullscreen,       /**< is fullscreen wanted? */
         m_bSettingsChanged;  /**< have the settings changed and need to be saved? */

    irr::core::dimension2du m_iMinResolution;

    irr::IrrlichtDevice       *m_pDevice;       /**< the irrlicht device to be used */
    irr::gui::IGUIEnvironment *m_pGuiEnv;       /**< the GUI environment */
    irr::video::IVideoDriver  *m_pDriver;       /**< the driver */
    irr::scene::ISceneManager *m_pSceneManager; /**< the scene manager */

    irr::video::IVideoModeList *m_pVModes;    /**< list of the available video modes from the Irrlicht device */
    irr::core::array<_VRES *> m_aVModes;     /**< list of the video modes that are available in the combobox */

    irr::c8 m_sScene[0xFF];

    wchar_t m_sTitle[0xFF];

    irr::video::E_DRIVER_TYPE m_eDriver;

  public:
    CSettings(const irr::c8 *sSettingsFile, const wchar_t *sTitle, irr::video::SColor cBackground);
    ~CSettings();

    void createGUI();

    /**
     * run the dialog
     * @return "1" if the "OK" button was pressed, "2" for the "Cancel" button, "0" otherwise
     */
    irr::u32 run();

    /**
     * Irrlicht event receiver
     * @param evt the event to be handled
     * @return was the event handled?
     */
    virtual bool OnEvent(const irr::SEvent &evt);

    /**
     * create a new device from the user's settings
     * @return the newly created IrrlichtDevice instance
     */
    irr::IrrlichtDevice *createDeviceFromSettings();

		void setMinResolution(irr::core::dimension2du iMin) { m_iMinResolution=iMin; }

		bool isActive(irr::u32 i) { return m_aAct[i]; }

		irr::u32 getCountOf(irr::u32 i) { return m_iCnt[i]; }

		irr::video::E_DRIVER_TYPE getSelectedDriver() { return m_eDriver; }
};

#endif

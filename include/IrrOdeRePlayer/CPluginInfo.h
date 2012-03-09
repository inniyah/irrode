#ifndef _C_PLUGIN_INFO
  #define _C_PLUGIN_INFO

  #include <windows.h>
  #include <stdio.h>

  #include <Irrlicht.h>

namespace irr {
  class IrrlichtDevice;
  namespace ode {
    class CIrrOdeManager;
  }
}

class IPlugin {
  protected:
    typedef int(*PI_install )(irr::IrrlichtDevice *, void *);   /**< typedef for the "install" method of the dll */
    typedef int(*PI_destall )(irr::IrrlichtDevice *, void *);   /**< typedef for the "destall" method of the dll */

    typedef int(*PI_camera  )(void);    /**< typedef for the "handle camera" method of the dll */

    typedef bool(*PI_handleEvent)(const irr::SEvent &event);
    typedef void(*PI_physicsInitialized)(void);

  public:
    virtual ~IPlugin() { }
    virtual int pluginInstall(void *pUserData)=0;    /**< This method calls the dll's "install" method */
    virtual int pluginDestall(void *pUserData)=0;    /**< This method calls the dll's "destall" method */

    virtual bool pluginHandleCamera()=0;

    virtual bool dllLoaded()=0;

    virtual bool HandleEvent(const irr::SEvent &event)=0;

    virtual void physicsInitialized()=0;
};

/**
 * @class CPluginInfo
 * @author Christian Keimel / bulletbyte.de
 * @brief a class that encapsulates loading of IrrOdeRePlayer plugins
 * This class is used as an encapsulation of plugin dlls for the IrrOdeRePlayer program. The idea is that
 * all plugins within a folder are loaded, and as soon as a replay is started all plugins that have
 * registered for the application that generated the plugin will be installed (calling the dll's "install"
 * function. After the plugin has finished the "destall" function of the plugins will be called.
 *
 * The dlls will remain in memory from the start of the program to it's end. The "install" and "destall"
 * methods do just add or remove listeners to the IrrOde Event Queue or they add or remove Irrlicht scene
 * node and ODE event factories so that application specific events can be handled or special effects are
 * shown
 */
class CPluginInfo : public IPlugin {
  protected:
    HINSTANCE m_pDll;   /**<! handle to the loaded dll */

    PI_install m_pFuncInstall;    /**< pointer to the "install" method */
    PI_destall m_pFuncDestall;    /**< pointer to the "destall" method */
    PI_camera  m_pHandleCamera;   /**< pointer to the "handle camera" method */

    PI_handleEvent m_pHandleEvent;

    PI_physicsInitialized m_pPhysicsInitialized;

    irr::IrrlichtDevice *m_pDevice;

  public:
    /**
     * The constructor. Here the dll gets loaded and the adresses of the "install" and "destall" methods
     * are queried from the Windows API
     * @param sDllFileName filename of the dll to load
     * @param pDevice the Irrlicht device for this plugin
     */
    CPluginInfo(const char *sDllFileName, irr::IrrlichtDevice *pDevice);

    /**
     * The destructor. Here the dll gets unloaded
     */
    virtual ~CPluginInfo();

    virtual int pluginInstall(void *pUserData); /**< This method calls the dll's "install" method */
    virtual int pluginDestall(void *pUserData); /**< This method calls the dll's "destall" method */

    /**
     * This method checks whether or not the dll was loaded by verifying the adresses
     * of the "install" and "destall" methods. This instance can be destroyed right
     * away if no dll was loaded
     * @return "true" if the dll was loaded, "false" otherwise
     */
    virtual bool dllLoaded();

    irr::IrrlichtDevice *getIrrlichtDevice() { return m_pDevice; }

    virtual bool pluginHandleCamera();

    virtual bool HandleEvent(const irr::SEvent &event);

    virtual void physicsInitialized();
};

#endif


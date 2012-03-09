  #include "CPluginInfo.h"

/**
 * Load the plugin
 * @param sDllFileName the plugin to be loaded
 */
CPluginInfo::CPluginInfo(const char *sDllFileName, irr::IrrlichtDevice *pDevice) {
  //First step: initialize all members
  m_pFuncInstall=NULL;
  m_pFuncDestall=NULL;
  m_pDevice=pDevice;

  //now let's load the plugin
  m_pDll=LoadLibrary(sDllFileName);;

  //if the plugin is loaded we'll query the adresses of the install and the destall method
  if (m_pDll) {
    m_pFuncInstall=(PI_install)GetProcAddress(m_pDll,"install");
    m_pFuncDestall=(PI_destall)GetProcAddress(m_pDll,"destall");

    m_pHandleCamera=(PI_camera     )GetProcAddress(m_pDll,"handleCamera");
    m_pHandleEvent =(PI_handleEvent)GetProcAddress(m_pDll,"handleEvent" );

    m_pPhysicsInitialized=(PI_physicsInitialized)GetProcAddress(m_pDll,"physicsInitialized");
  }
}

/**
 * The destructor
 * Unload the dll if it was loaded
 */
CPluginInfo::~CPluginInfo() {
  if (m_pDll!=NULL) FreeLibrary(m_pDll);
}

/**
 * call the dll's "install" method
 */
int CPluginInfo::pluginInstall(void *pUserData) {
  return m_pFuncInstall(m_pDevice,pUserData);
}

/**
 * call the dll's "destall" method
 */
int CPluginInfo::pluginDestall(void *pUserData) {
  return m_pFuncDestall(m_pDevice,pUserData);
}

/**
 * This method checks whether or not the dll has been loaded by verifying all three function pointers
 * @return "true" if both of the function pointers are not equal NULL, false otherwise
 */
bool CPluginInfo::dllLoaded() {
  return m_pFuncInstall!=NULL && m_pFuncDestall!=NULL;
}

bool CPluginInfo::pluginHandleCamera() {
  if (!m_pHandleCamera)
    return false;

  m_pHandleCamera();
  return true;
}

bool CPluginInfo::HandleEvent(const irr::SEvent &event) {
  if (m_pHandleEvent)
    return m_pHandleEvent(event);
  else
    return false;
}

void CPluginInfo::physicsInitialized() {
  if (m_pPhysicsInitialized) m_pPhysicsInitialized();
}

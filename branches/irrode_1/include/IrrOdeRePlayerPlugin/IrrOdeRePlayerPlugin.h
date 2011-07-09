#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include <Irrlicht.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * This method is called when a replay from an application that this plugin is registered for is started.
 * @param pDevice the used Irrlicht device
 * @param pUserData some user data
 * @see CPluginInfo::pluginInstall
 * @return "0" if successful
 */
int DLL_EXPORT install(irr::IrrlichtDevice *pDevice, void *pUserData);

/**
 * This method is called when a replay from an application that this plugin is registered for has finished.
 * @param pDevice the used Irrlicht device
 * @param pUserData some user data
 * @see CPluginInfo::pluginDestall
 * @return "0" if successful
 */
int DLL_EXPORT destall(irr::IrrlichtDevice *pDevice, void *pUserData);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__

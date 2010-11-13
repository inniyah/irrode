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
 * @see CPluginInfo::pluginInstall
 * @return "0" if successful
 */
int DLL_EXPORT install(irr::IrrlichtDevice *pDevice);

/**
 * This method is called when a replay from an application that this plugin is registered for has finished.
 * @see CPluginInfo::pluginDestall
 * @return "0" if successful
 */
int DLL_EXPORT destall(irr::IrrlichtDevice *pDevice);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__

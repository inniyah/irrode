#ifndef _C_TARGET_SELECTOR
  #define _C_TARGET_SELECTOR

  #include <irrlicht.h>

namespace irr {
  namespace ode {
    class CIrrOdeBody;
    class CIrrOdeManager;
    class CIrrOdeSceneNode;
  }
}

using namespace irr;

class CTargetSelector {
  protected:
    ode::CIrrOdeBody *m_pCarrier,     /**<! the body that carries the selector */
                     *m_pTarget,      /**<! the selected target */
                     *m_pOption;      /**<! the optional target */

    IrrlichtDevice *m_pDevice;
    core::vector3df m_vForeward;
    ode::CIrrOdeManager *m_pManager;
    scene::ISceneCollisionManager *m_pColMgr;
    video::IVideoDriver *m_pDrv;

    core::list<irr::ode::CIrrOdeBody *> m_lOptions;

  public:
    CTargetSelector(ode::CIrrOdeBody *pCarrier, IrrlichtDevice *pDevice, core::vector3df vForeward);

    void update();
    void highlightTargets();
    bool selectOption();

    ode::CIrrOdeBody *getTarget();
};

#endif

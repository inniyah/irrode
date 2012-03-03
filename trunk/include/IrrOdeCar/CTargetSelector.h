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

class CTargetSelector {
  protected:
    irr::ode::CIrrOdeBody *m_pCarrier,     /**<! the body that carries the selector */
                          *m_pTarget,      /**<! the selected target */
                          *m_pOption;      /**<! the optional target */

    irr::IrrlichtDevice *m_pDevice;
    irr::core::vector3df m_vForeward;
    irr::ode::CIrrOdeManager *m_pManager;
    irr::scene::ISceneCollisionManager *m_pColMgr;
    irr::video::IVideoDriver *m_pDrv;

    irr::core::list<irr::ode::CIrrOdeBody *> m_lOptions;

  public:
    CTargetSelector(irr::ode::CIrrOdeBody *pCarrier, irr::IrrlichtDevice *pDevice, irr::core::vector3df vForeward);

    void update();
    void highlightTargets();
    bool selectOption();

    irr::ode::CIrrOdeBody *getTarget();
};

#endif

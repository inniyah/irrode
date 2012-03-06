#ifndef _C_CONTROLLER
  #define _C_CONTROLLER

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

class CIrrCC;

class CController : public CIrrOdeCarState, public irr::IEventReceiver {
  protected:
    irr::gui::IGUITabControl *m_pTabCtrl;

  public:
    CController(irr::IrrlichtDevice *pDevice, CIrrCC *pCtrl);  /*!< the constructor */
    virtual ~CController();                               /*!< the destructor */

    virtual void activate();      /*!< the activation method */
    virtual void deactivate();    /*!< the deactivation method */
    virtual irr::u32 update();         /*!< the update method called once per frame */

    virtual bool OnEvent(const irr::SEvent &event);  /*!< the Irrlicht event receiver callback */

    virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L"controls"; return s; }

    virtual void drawSpecifics() { }
    virtual irr::ode::CIrrOdeBody *getBody() { return NULL; }
};

#endif


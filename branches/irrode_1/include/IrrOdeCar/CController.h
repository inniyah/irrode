#ifndef _C_CONTROLLER
  #define _C_CONTROLLER

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

class CIrrCC;

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class CController : public CIrrOdeCarState, public IEventReceiver {
  protected:
    IGUITabControl *m_pTabCtrl;

  public:
    CController(IrrlichtDevice *pDevice, CIrrCC *pCtrl);  /*!< the constructor */
    virtual ~CController();                               /*!< the destructor */

    virtual void activate();      /*!< the activation method */
    virtual void deactivate();    /*!< the deactivation method */
    virtual u32 update();         /*!< the update method called once per frame */

    virtual bool OnEvent(const SEvent &event);  /*!< the Irrlicht event receiver callback */

    virtual stringc &getButtonText() { static stringc s=stringc("controller setup"); return s; }

    virtual void drawSpecifics() { }
};

#endif


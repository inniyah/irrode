#ifndef _C_MENU
  #define _C_MENU

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

class CMenu : public CIrrOdeCarState, public irr::IEventReceiver {
  protected:
    irr::core::list<irr::gui::IGUIButton *> m_aButtons;  /*!< a list of buttons */
    irr::u32 m_iMenuSelect;                              /*!< this member is used to store state changes */
    irr::core::dimension2di m_cDim;
    irr::core::position2di m_cPos;
    irr::u32 m_iIdx;
    irr::s32 m_iMouseX,m_iMouseY;
    irr::f32 m_fAngleH,m_fAngleV;
    bool m_bLeftBtn;
    irr::core::vector3df m_vCamLookAt;

  public:
    /**
     * constructor
     * @param pDevice the Irrlicht device
     */
    CMenu(irr::IrrlichtDevice *pDevice, CIrrCC *pCtrl);
    virtual ~CMenu();   /*!< the destructor */

    virtual void activate();    /*!< state activation */
    virtual void deactivate();  /*!< state deactivation */
    virtual irr::u32 update();  /*!< state update */

    virtual bool OnEvent(const irr::SEvent &event);  /*!< the Irrlicht event receiver */

    void setBtnEnabled(irr::u32 iNum, bool b);

    virtual irr::core::stringc &getButtonText() { static irr::core::stringc s=irr::core::stringc("menu"); return s; }

    void addButtonForState(CIrrOdeCarState *pState);

    virtual const irr::core::stringw &getButton() { static irr::core::stringw s=L""; return s; }

    virtual void drawSpecifics() { }
};

#endif

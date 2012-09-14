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
    irr::gui::IGUITab *m_pTab;

    irr::ode::IIrrOdeEventQueue *m_pInputQueue;

  public:
    /**
     * constructor
     * @param pDevice the Irrlicht device
     */
    CMenu(irr::IrrlichtDevice *pDevice, irr::ode::IIrrOdeEventQueue *pInputQueue);
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

    virtual irr::ode::CIrrOdeBody *getBody() { return NULL; }

    void setVisible(bool b) { m_pTab->setVisible(b); }

    virtual enumStateType getType() { return eStateMenu; }

    virtual bool onEvent(irr::ode::IIrrOdeEvent *p) { return false; }
    virtual bool handlesEvent(irr::ode::IIrrOdeEvent *p) { return false; }
};

#endif

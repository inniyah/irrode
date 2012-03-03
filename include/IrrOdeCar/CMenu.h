#ifndef _C_MENU
  #define _C_MENU

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include "CIrrOdeCarState.h"

class CMenu : public CIrrOdeCarState, public IEventReceiver {
  protected:
    list<IGUIButton *> m_aButtons;  /*!< a list of buttons */
    u32 m_iMenuSelect;              /*!< this member is used to store state changes */
    dimension2di m_cDim;
    position2di m_cPos;
    u32 m_iIdx;
    s32 m_iMouseX,m_iMouseY;
    f32 m_fAngleH,m_fAngleV;
    bool m_bLeftBtn;
    core::vector3df m_vCamLookAt;

  public:
    /**
     * constructor
     * @param pDevice the Irrlicht device
     */
    CMenu(IrrlichtDevice *pDevice, CIrrCC *pCtrl);
    virtual ~CMenu();   /*!< the destructor */

    virtual void activate();    /*!< state activation */
    virtual void deactivate();  /*!< state deactivation */
    virtual u32 update();       /*!< state update */

    virtual bool OnEvent(const SEvent &event);  /*!< the Irrlicht event receiver */

    void setBtnEnabled(u32 iNum, bool b);

    virtual stringc &getButtonText() { static stringc s=stringc("menu"); return s; }

    void addButtonForState(CIrrOdeCarState *pState);

    virtual const stringw &getButton() { static core::stringw s=L""; return s; }

    virtual void drawSpecifics() { }
};

#endif

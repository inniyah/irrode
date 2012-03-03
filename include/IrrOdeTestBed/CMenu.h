#ifndef _C_MENU
  #define _C_MENU

  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>

class CMenuState : public IState, public irr::IEventReceiver {
  protected:
    irr::s32 m_iRet;

    irr::gui::IGUIListBox *m_pList;
    irr::gui::IGUIButton  *m_pStart;

    irr::core::array<IState *> *m_aStates;

    irr::core::dimension2du m_cSize;

  public:
    CMenuState(irr::IrrlichtDevice *pDevice, IRunner *pRunner, irr::core::array<IState *> *aStates);

    virtual ~CMenuState() {
    }

    virtual void activate();

    virtual void deactivate();

    virtual irr::s32 update();

    virtual bool OnEvent(const irr::SEvent &event);
};


#endif

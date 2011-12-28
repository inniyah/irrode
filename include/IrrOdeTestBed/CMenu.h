#ifndef _C_MENU
  #define _C_MENU
  
  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>
  
using namespace irr; 

class CMenuState : public IState, public IEventReceiver {
  protected:
    s32 m_iRet;
    
    gui::IGUIListBox *m_pList;
    gui::IGUIButton  *m_pStart;
    
    core::array<IState *> *m_aStates;
    
    core::dimension2du m_cSize;

  public:
    CMenuState(IrrlichtDevice *pDevice, IRunner *pRunner, core::array<IState *> *aStates);
    
    virtual ~CMenuState() {
    }
    
    virtual void activate();
    
    virtual void deactivate();
    
    virtual s32 update();
    
    virtual bool OnEvent(const SEvent &event);
};


#endif

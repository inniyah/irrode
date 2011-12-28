  #include <irrlicht.h>
  #include <IrrOde.h>

  #include <IRunner.h>
  #include <IState.h>
  #include <CMenu.h>
  
using namespace irr; 

CMenuState::CMenuState(IrrlichtDevice *pDevice, IRunner *pRunner, core::array<IState *> *aStates) : IState(pDevice,pRunner) {
  m_aStates=aStates;
}

void CMenuState::activate() {
  m_iRet=0;
  m_cSize=core::dimension2du(0,0);
  core::position2di cPos=core::position2di(10,10);
  
  m_pStart=m_pGui->addButton (core::rect<s32>(cPos,core::dimension2di(200, 20)),NULL, 1,L"Start Test"); cPos.Y+=30;
  m_pList =m_pGui->addListBox(core::rect<s32>(cPos,core::dimension2di(200,500)),NULL,-1,true);
  
  for (u32 i=1; i<(*m_aStates).size(); i++) m_pList->addItem((*m_aStates)[i]->getTestName());
  
  m_pRunner->setEventReceiver(this);
}

void CMenuState::deactivate() {
  m_pGui->clear();
  m_pRunner->setEventReceiver(NULL);
}

s32 CMenuState::update() {
  core::dimension2du cSize=m_pDrv->getScreenSize();
  if (cSize!=m_cSize) {
    m_cSize=cSize;
    m_pList->setRelativePosition(core::rect<s32>(10,40,200,cSize.Height-10));
  }
  
  return m_iRet;
}

bool CMenuState::OnEvent(const SEvent &event) {
  bool bRet=false;
  
  if (event.EventType==EET_GUI_EVENT) {
    if (event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED) {
      if (event.GUIEvent.Caller==m_pStart && m_pList->getSelected()!=-1) {
        //Switch to another state (state "0" is the menu and can only be called with the "Escape" key)
        m_iRet=m_pList->getSelected()+1;
      }
    }
  }
  
  return bRet;
}


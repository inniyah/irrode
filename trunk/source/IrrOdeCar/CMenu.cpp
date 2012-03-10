  #include "CMenu.h"
  #include "CProjectile.h"

CMenu::CMenu(irr::IrrlichtDevice *pDevice, CIrrCC *pCtrl) : CIrrOdeCarState(pDevice,L"Vehicle Select Menu","../../data/irrOdeVehicleHelp.txt", pCtrl) {
  irr::gui::IGUIButton *b=NULL;

  m_cDim=irr::core::dimension2di(128,30);
  m_cPos=irr::core::position2di(5,5);
  m_iIdx=2;

  //create the buttons necessary to select the vehicle to control
  b=m_pGuiEnv->addButton(irr::core::rect<irr::s32>(m_cPos,m_cDim),0,m_iIdx++,L"");

  b->setImage       (m_pDevice->getVideoDriver()->getTexture("../../data/textures/buttons/controls_off.png"));
  b->setPressedImage(m_pDevice->getVideoDriver()->getTexture("../../data/textures/buttons/controls_on.png" ));
  b->setUseAlphaChannel(true);

  m_aButtons.push_back(b);
  m_cPos.Y+=35;

  irr::core::list<irr::gui::IGUIButton *>::Iterator i;
  for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(false);

  m_iMouseX=-100;
  m_iMouseY=-100;
  m_bLeftBtn=false;
  m_fAngleH=0.0f;
  m_fAngleV=0.0f;
}

CMenu::~CMenu() {
}

void CMenu::activate() {
  //register this class as Irrlicht event receiver

  //on the first activation we want the camera to focus the info panel floating somewhere between the heli and the car
  static bool bFirstCall=true;
  if (bFirstCall) {
    m_pSmgr->getActiveCamera()->setPosition(irr::core::vector3df(3502.5f,1000.0f,2490.0f));
    m_pSmgr->getActiveCamera()->setTarget(irr::core::vector3df(3500.0f,1000.0f,2500.0f));
    m_vCamLookAt=m_pSmgr->getActiveCamera()->getTarget();
    bFirstCall=false;
  }
  m_pSmgr->getActiveCamera()->setUpVector(irr::core::vector3df(0,1,0));

  //returning 0 means that no state change is wished, so we init the member returned by CMenu::update
  m_iMenuSelect=0;

  //show the buttons
  irr::core::list<irr::gui::IGUIButton *>::Iterator i;
  for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(true);

  m_iMouseX=-100;
  m_iMouseY=-100;
}

void CMenu::deactivate() {
  //hide the buttons
  irr::core::list<irr::gui::IGUIButton *>::Iterator i;
  for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(false);
}

irr::u32 CMenu::update() {
  //show the info text and the buttons if help is visible
  if (!m_bHelp && m_pHelp->isVisible()) {
    irr::core::list<irr::gui::IGUIButton *>::Iterator i;
    for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(true);
  }

  //hide the info text and the buttons if help is visible
  if (m_bHelp && !m_pHelp->isVisible()) {
    irr::core::list<irr::gui::IGUIButton *>::Iterator i;
    for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(false);
  }

  //call superclass update
  CIrrOdeCarState::update();

  //return 0 if no state change is wanted, new state index+1 otherwise
  return m_iMenuSelect;
}

bool CMenu::OnEvent(const irr::SEvent &event) {
  if (CIrrOdeCarState::OnEvent(event)) return true;

  if (event.EventType==irr::EET_GUI_EVENT) {
    if (event.GUIEvent.EventType==irr::gui::EGET_BUTTON_CLICKED) {
      //the ID of the button is the return value necessary to switch to the wanted state
      m_iMenuSelect=event.GUIEvent.Caller->getID();
    }
  }

  return false;
}

void CMenu::setBtnEnabled(irr::u32 iNum, bool b) {
  if (iNum>=m_aButtons.getSize()) return;
  irr::core::list<irr::gui::IGUIButton *>::Iterator it;
  for (it=m_aButtons.begin(); it!=m_aButtons.end(); it++)
    if (iNum--==0) {
      (*it)->setEnabled(b);
      return;
    }
}

void CMenu::addButtonForState(CIrrOdeCarState *pState) {
  irr::gui::IGUIButton *b=NULL;

  //create the buttons necessary to select the vehicle to control
  b=m_pGuiEnv->addButton(irr::core::rect<irr::s32>(m_cPos,m_cDim),0,m_iIdx++,L"");
  b->setVisible(false);

  irr::core::stringw sBtn = "../../data/textures/buttons/";
  sBtn += pState->getButton();

  irr::core::stringw sOn  = sBtn+"_on.png",
                     sOff = sBtn+"_off.png";

  irr::video::ITexture *pOn  = m_pDevice->getVideoDriver()->getTexture(sOn ),
                       *pOff = m_pDevice->getVideoDriver()->getTexture(sOff);

  b->setImage       (pOff);
  b->setPressedImage(pOn );

  b->setUseAlphaChannel(true);

  m_aButtons.push_back(b);
  m_cPos.Y+=35;
}

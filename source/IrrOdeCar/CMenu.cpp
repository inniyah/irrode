  #include "CMenu.h"
  #include "CProjectile.h"

CMenu::CMenu(IrrlichtDevice *pDevice, CIrrCC *pCtrl) : CIrrOdeCarState(pDevice,L"Vehicle Select Menu","../../data/irrOdeVehicleHelp.txt", pCtrl) {
  IGUIButton *b=NULL;

  m_cDim=dimension2di(128,30);
  m_cPos=position2di(5,5);
  m_iIdx=2;

  //create the buttons necessary to select the vehicle to control
  b=m_pGuiEnv->addButton(rect<s32>(m_cPos,m_cDim),0,m_iIdx++,L"");

  b->setImage       (m_pDevice->getVideoDriver()->getTexture("../../data/textures/buttons/controls_off.png"));
  b->setPressedImage(m_pDevice->getVideoDriver()->getTexture("../../data/textures/buttons/controls_on.png" ));
  b->setUseAlphaChannel(true);

  m_aButtons.push_back(b);
  m_cPos.Y+=35;

  list<IGUIButton *>::Iterator i;
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
  m_pDevice->setEventReceiver(this);

  //on the first activation we want the camera to focus the info panel floating somewhere between the heli and the car
  static bool bFirstCall=true;
  if (bFirstCall) {
    m_pSmgr->getActiveCamera()->setPosition(vector3df(3502.5f,1000.0f,2490.0f));
    m_pSmgr->getActiveCamera()->setTarget(vector3df(3500.0f,1000.0f,2500.0f));
    m_vCamLookAt=m_pSmgr->getActiveCamera()->getTarget();
    bFirstCall=false;
  }
  m_pSmgr->getActiveCamera()->setUpVector(core::vector3df(0,1,0));

  //we need to see the mouse cursor
  m_pDevice->getCursorControl()->setVisible(true);

  //returning 0 means that no state change is wished, so we init the member returned by CMenu::update
  m_iMenuSelect=0;

  //show the buttons
  list<IGUIButton *>::Iterator i;
  for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(true);

  m_iMouseX=-100;
  m_iMouseY=-100;
}

void CMenu::deactivate() {
  //hide the buttons
  list<IGUIButton *>::Iterator i;
  for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(false);
}

u32 CMenu::update() {
  //show the info text and the buttons if help is visible
  if (!m_bHelp && m_pHelp->isVisible()) {
    list<IGUIButton *>::Iterator i;
    for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(true);
  }

  //hide the info text and the buttons if help is visible
  if (m_bHelp && !m_pHelp->isVisible()) {
    list<IGUIButton *>::Iterator i;
    for (i=m_aButtons.begin(); i!=m_aButtons.end(); i++) (*i)->setVisible(false);
  }

  //call superclass update
  CIrrOdeCarState::update();

  //return 0 if no state change is wanted, new state index+1 otherwise
  return m_iMenuSelect;
}

bool CMenu::OnEvent(const SEvent &event) {
  if (CIrrOdeCarState::OnEvent(event)) return true;

  if (event.EventType==EET_GUI_EVENT) {
    if (event.GUIEvent.EventType==EGET_BUTTON_CLICKED) {
      //the ID of the button is the return value necessary to switch to the wanted state
      m_iMenuSelect=event.GUIEvent.Caller->getID();
    }
  }

  //Handle mouse input events to update the camera (or the position of a segment when the arrow is dragged)
  if (event.EventType==EET_MOUSE_INPUT_EVENT) {
    scene::ICameraSceneNode *pCam=m_pSmgr->getActiveCamera();
    if (m_iMouseX!=-100 && m_iMouseY!=-100) {
      if (event.MouseInput.isLeftPressed()) {
        m_bLeftBtn=true;
        m_fAngleH-=0.5f*((f32)m_iMouseX-event.MouseInput.X);
        m_fAngleV+=0.5f*((f32)m_iMouseY-event.MouseInput.Y);

        while (m_fAngleV> 80.0f) m_fAngleV= 80.0f;
        while (m_fAngleV<-80.0f) m_fAngleV=-80.0f;

        f32 fCamX=(irr::f32)(1.0f*sin(m_fAngleH*ode::GRAD_PI2)*cos(m_fAngleV*ode::GRAD_PI2)),
        fCamY=(irr::f32)(1.0f*sin(m_fAngleV*ode::GRAD_PI2)),
        fCamZ=(irr::f32)(1.0f*cos(m_fAngleH*ode::GRAD_PI2)*cos(m_fAngleV*ode::GRAD_PI2));

        m_vCamLookAt=core::vector3df(fCamX,fCamY,fCamZ);
        pCam->setTarget(pCam->getPosition()+m_vCamLookAt);
      }
      else m_bLeftBtn=false;

      if (event.MouseInput.isRightPressed()) {
        core::vector3df vStrafe=m_vCamLookAt.crossProduct(core::vector3df(0,1,0)),
        v=pCam->getPosition()+((f32)(m_iMouseY-event.MouseInput.Y))*m_vCamLookAt+((f32)(m_iMouseX-event.MouseInput.X))*vStrafe;
        pCam->setPosition(v);
        pCam->setTarget(v+m_vCamLookAt);
      }
    }

    m_iMouseX=event.MouseInput.X;
    m_iMouseY=event.MouseInput.Y;
  }

  return false;
}

void CMenu::setBtnEnabled(u32 iNum, bool b) {
  if (iNum>=m_aButtons.getSize()) return;
  list<IGUIButton *>::Iterator it;
  for (it=m_aButtons.begin(); it!=m_aButtons.end(); it++)
    if (iNum--==0) {
      (*it)->setEnabled(b);
      return;
    }
}

void CMenu::addButtonForState(CIrrOdeCarState *pState) {
  IGUIButton *b=NULL;

  //create the buttons necessary to select the vehicle to control
  b=m_pGuiEnv->addButton(rect<s32>(m_cPos,m_cDim),0,m_iIdx++,L"");
  b->setVisible(false);

  core::stringw sBtn = "../../data/textures/buttons/";
  sBtn += pState->getButton();

  core::stringw sOn  = sBtn+"_on.png",
                sOff = sBtn+"_off.png";

  ITexture *pOn  = m_pDevice->getVideoDriver()->getTexture(sOn ),
           *pOff = m_pDevice->getVideoDriver()->getTexture(sOff);

  b->setImage       (pOff);
  b->setPressedImage(pOn );

  b->setUseAlphaChannel(true);

  m_aButtons.push_back(b);
  m_cPos.Y+=35;
}
